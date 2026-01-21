/* This is xdvipsk, an eXtended version of dvips(k) by Tomas Rokicki.

	Copyright (C) 2016 by VTeX Ltd (www.vtex.lt),
	the xdvipsk project team - Sigitas Tolusis and Arunas Povilaitis.

    Program original code copyright by Floris van den Berg,
	Hervé Drolon and Karl-Heinz Bussian, the FreeImage 3 project team.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

// ==========================================================
// JPEG Loader
// Based on code developed by The Independent JPEG Group
//
// ==========================================================

#ifdef _MSC_VER 
#pragma warning (disable : 4786) // identifier was truncated to 'number' characters
#endif

extern "C" {
#define XMD_H
#undef FAR
#include <setjmp.h>

#include "jinclude.h"
#ifndef HAVE_BOOLEAN
#define HAVE_BOOLEAN
typedef int boolean;
#endif
#include "jpeglib.h"
#include "jerror.h"
}

#include "bitmap.h"
#include "utilities.h"


// ----------------------------------------------------------
//   Constant declarations
// ----------------------------------------------------------

#define INPUT_BUF_SIZE  4096	// choose an efficiently fread'able size 
#define OUTPUT_BUF_SIZE 4096    // choose an efficiently fwrite'able size

#define EXIF_MARKER		(JPEG_APP0+1)	// EXIF marker / Adobe XMP marker
#define ICC_MARKER		(JPEG_APP0+2)	// ICC profile marker
#define IPTC_MARKER		(JPEG_APP0+13)	// IPTC marker / BIM marker 

#define ICC_HEADER_SIZE 14				// size of non-profile data in APP2
#define MAX_BYTES_IN_MARKER 65533L		// maximum data length of a JPEG marker
#define MAX_DATA_BYTES_IN_MARKER 65519L	// maximum data length of a JPEG APP2 marker

#define MAX_JFXX_THUMB_SIZE (MAX_BYTES_IN_MARKER - 5 - 1)

#define JFXX_TYPE_JPEG 	0x10	// JFIF extension marker: JPEG-compressed thumbnail image
#define JFXX_TYPE_8bit 	0x11	// JFIF extension marker: palette thumbnail image
#define JFXX_TYPE_24bit	0x13	// JFIF extension marker: RGB thumbnail image

// ----------------------------------------------------------
//   Typedef declarations
// ----------------------------------------------------------

typedef struct tagErrorManager {
	/// "public" fields
	struct jpeg_error_mgr pub;
	/// for return to caller
	jmp_buf setjmp_buffer;
} ErrorManager;

typedef struct tagSourceManager {
	/// public fields
	struct jpeg_source_mgr pub;
	/// source stream
	fi_handle infile;
	BitmapIO *m_io;
	/// start of buffer
	JOCTET * buffer;
	/// have we gotten any data yet ?
	boolean start_of_file;
} SourceManager;

typedef SourceManager*		bitmap_src_ptr;
typedef ErrorManager*		bitmap_error_ptr;

// ----------------------------------------------------------
//   Error handling
// ----------------------------------------------------------

/** Fatal errors (print message and exit) */
static inline void
JPEG_EXIT(j_common_ptr cinfo, int code) {
	bitmap_error_ptr error_ptr = (bitmap_error_ptr)cinfo->err;
	error_ptr->pub.msg_code = code;
	error_ptr->pub.error_exit(cinfo);
}

/** Nonfatal errors (we can keep going, but the data is probably corrupt) */
static inline void
JPEG_WARNING(j_common_ptr cinfo, int code) {
	bitmap_error_ptr error_ptr = (bitmap_error_ptr)cinfo->err;
	error_ptr->pub.msg_code = code;
	error_ptr->pub.emit_message(cinfo, -1);
}

/**
	Receives control for a fatal error.  Information sufficient to
	generate the error message has been stored in cinfo->err; call
	output_message to display it.  Control must NOT return to the caller;
	generally this routine will exit() or longjmp() somewhere.
*/
METHODDEF(void)
jpeg_error_exit (j_common_ptr cinfo) {
	bitmap_error_ptr error_ptr = (bitmap_error_ptr)cinfo->err;

	// always display the message
	error_ptr->pub.output_message(cinfo);

	// allow JPEG with unknown markers
	if(error_ptr->pub.msg_code != JERR_UNKNOWN_MARKER) {
	
		// let the memory manager delete any temp files before we die
		jpeg_destroy(cinfo);
		
		// return control to the setjmp point
		longjmp(error_ptr->setjmp_buffer, 1);		
	}
}

/**
	Actual output of any JPEG message.  Note that this method does not know
	how to generate a message, only where to send it.
*/
METHODDEF(void)
jpeg_output_message (j_common_ptr cinfo) {
	char buffer[JMSG_LENGTH_MAX];
	bitmap_error_ptr error_ptr = (bitmap_error_ptr)cinfo->err;

	// create the message
	error_ptr->pub.format_message(cinfo, buffer);
	// send it to user's message proc
	Bitmap_OutputMessageProc(FIF_JPEG, buffer);
}

// ----------------------------------------------------------
//   Source manager
// ----------------------------------------------------------

/**
	Initialize source.  This is called by jpeg_read_header() before any
	data is actually read. Unlike init_destination(), it may leave
	bytes_in_buffer set to 0 (in which case a fill_input_buffer() call
	will occur immediately).
*/
METHODDEF(void)
init_source (j_decompress_ptr cinfo) {
	bitmap_src_ptr src = (bitmap_src_ptr)cinfo->src;

	/* We reset the empty-input-file flag for each image,
 	 * but we don't clear the input buffer.
	 * This is correct behavior for reading a series of images from one source.
	*/

	src->start_of_file = TRUE;
}

/**
	This is called whenever bytes_in_buffer has reached zero and more
	data is wanted.  In typical applications, it should read fresh data
	into the buffer (ignoring the current state of next_input_byte and
	bytes_in_buffer), reset the pointer & count to the start of the
	buffer, and return TRUE indicating that the buffer has been reloaded.
	It is not necessary to fill the buffer entirely, only to obtain at
	least one more byte.  bytes_in_buffer MUST be set to a positive value
	if TRUE is returned.  A FALSE return should only be used when I/O
	suspension is desired.
*/
METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo) {
	bitmap_src_ptr src = (bitmap_src_ptr)cinfo->src;

	size_t nbytes = src->m_io->read_proc(src->buffer, 1, INPUT_BUF_SIZE, src->infile);

	if (nbytes <= 0) {
		if (src->start_of_file)	{
			// treat empty input file as fatal error

			// let the memory manager delete any temp files before we die
			jpeg_destroy((j_common_ptr)cinfo);

			JPEG_EXIT((j_common_ptr)cinfo, JERR_INPUT_EMPTY);
		}

		JPEG_WARNING((j_common_ptr)cinfo, JWRN_JPEG_EOF);

		/* Insert a fake EOI marker */

		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;

		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

/**
	Skip num_bytes worth of data.  The buffer pointer and count should
	be advanced over num_bytes input bytes, refilling the buffer as
	needed. This is used to skip over a potentially large amount of
	uninteresting data (such as an APPn marker). In some applications
	it may be possible to optimize away the reading of the skipped data,
	but it's not clear that being smart is worth much trouble; large
	skips are uncommon.  bytes_in_buffer may be zero on return.
	A zero or negative skip count should be treated as a no-op.
*/
METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
	bitmap_src_ptr src = (bitmap_src_ptr)cinfo->src;

	/* Just a dumb implementation for now.  Could use fseek() except
     * it doesn't work on pipes.  Not clear that being smart is worth
	 * any trouble anyway --- large skips are infrequent.
	*/

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
		  num_bytes -= (long) src->pub.bytes_in_buffer;

		  (void) fill_input_buffer(cinfo);

		  /* note we assume that fill_input_buffer will never return FALSE,
		   * so suspension need not be handled.
		   */
		}

		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

/**
	Terminate source --- called by jpeg_finish_decompress
	after all data has been read.  Often a no-op.

	NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
	application must deal with any cleanup that should happen even
	for error exit.
*/
METHODDEF(void)
term_source (j_decompress_ptr cinfo) {
  // no work necessary here
}

// ----------------------------------------------------------
//   Source manager setup
// ----------------------------------------------------------

/**
	Prepare for input from a stdio stream.
	The caller must have already opened the stream, and is responsible
	for closing it after finishing decompression.
*/
GLOBAL(void)
jpeg_bitmap_src(j_decompress_ptr cinfo, fi_handle infile, BitmapIO *io) {
	bitmap_src_ptr src;

	// allocate memory for the buffer. is released automatically in the end

	if (cinfo->src == NULL) {
		cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
			((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(SourceManager));

		src = (bitmap_src_ptr)cinfo->src;

		src->buffer = (JOCTET *) (*cinfo->mem->alloc_small)
			((j_common_ptr) cinfo, JPOOL_PERMANENT, INPUT_BUF_SIZE * sizeof(JOCTET));
	}

	// initialize the jpeg pointer struct with pointers to functions

	src = (bitmap_src_ptr)cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; // use default method 
	src->pub.term_source = term_source;
	src->infile = infile;
	src->m_io = io;
	src->pub.bytes_in_buffer = 0;		// forces fill_input_buffer on first read 
	src->pub.next_input_byte = NULL;	// until buffer loaded 
}

// ----------------------------------------------------------
//   Special markers read functions
// ----------------------------------------------------------

/**
	Read JPEG_COM marker (comment)
*/
static BOOL 
jpeg_read_comment(FIBITMAP *dib, const BYTE *dataptr, unsigned int datalen) {
	size_t length = datalen;
	BYTE *profile = (BYTE*)dataptr;

	// read the comment
	char *value = (char*)malloc((length + 1) * sizeof(char));
	if(value == NULL) return FALSE;
	memcpy(value, profile, length);
	value[length] = '\0';

	free(value);

	return TRUE;
}

/** 
	Read JPEG_APP2 marker (ICC profile)
*/

/**
Handy subroutine to test whether a saved marker is an ICC profile marker.
*/
static BOOL 
marker_is_icc(jpeg_saved_marker_ptr marker) {
    // marker identifying string "ICC_PROFILE" (null-terminated)
	const BYTE icc_signature[12] = { 0x49, 0x43, 0x43, 0x5F, 0x50, 0x52, 0x4F, 0x46, 0x49, 0x4C, 0x45, 0x00 };

	if(marker->marker == ICC_MARKER) {
		// verify the identifying string
		if(marker->data_length >= ICC_HEADER_SIZE) {
			if(memcmp(icc_signature, marker->data, sizeof(icc_signature)) == 0) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

/**
  See if there was an ICC profile in the JPEG file being read;
  if so, reassemble and return the profile data.

  TRUE is returned if an ICC profile was found, FALSE if not.
  If TRUE is returned, *icc_data_ptr is set to point to the
  returned data, and *icc_data_len is set to its length.
  
  IMPORTANT: the data at **icc_data_ptr has been allocated with malloc()
  and must be freed by the caller with free() when the caller no longer
  needs it.  (Alternatively, we could write this routine to use the
  IJG library's memory allocator, so that the data would be freed implicitly
  at jpeg_finish_decompress() time.  But it seems likely that many apps
  will prefer to have the data stick around after decompression finishes.)
  
  NOTE: if the file contains invalid ICC APP2 markers, we just silently
  return FALSE.  You might want to issue an error message instead.
*/
static BOOL 
jpeg_read_icc_profile(j_decompress_ptr cinfo, JOCTET **icc_data_ptr, unsigned *icc_data_len) {
	jpeg_saved_marker_ptr marker;
	int num_markers = 0;
	int seq_no;
	JOCTET *icc_data;
	unsigned total_length;

	const int MAX_SEQ_NO = 255;			// sufficient since marker numbers are bytes
	BYTE marker_present[MAX_SEQ_NO+1];	// 1 if marker found
	unsigned data_length[MAX_SEQ_NO+1];	// size of profile data in marker
	unsigned data_offset[MAX_SEQ_NO+1];	// offset for data in marker
	
	*icc_data_ptr = NULL;		// avoid confusion if FALSE return
	*icc_data_len = 0;
	
	/**
	this first pass over the saved markers discovers whether there are
	any ICC markers and verifies the consistency of the marker numbering.
	*/
	
	memset(marker_present, 0, (MAX_SEQ_NO + 1));
	
	for(marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
		if (marker_is_icc(marker)) {
			if (num_markers == 0) {
				// number of markers
				num_markers = GETJOCTET(marker->data[13]);
			}
			else if (num_markers != GETJOCTET(marker->data[13])) {
				return FALSE;		// inconsistent num_markers fields 
			}
			// sequence number
			seq_no = GETJOCTET(marker->data[12]);
			if (seq_no <= 0 || seq_no > num_markers) {
				return FALSE;		// bogus sequence number 
			}
			if (marker_present[seq_no]) {
				return FALSE;		// duplicate sequence numbers 
			}
			marker_present[seq_no] = 1;
			data_length[seq_no] = marker->data_length - ICC_HEADER_SIZE;
		}
	}
	
	if (num_markers == 0)
		return FALSE;
		
	/**
	check for missing markers, count total space needed,
	compute offset of each marker's part of the data.
	*/
	
	total_length = 0;
	for(seq_no = 1; seq_no <= num_markers; seq_no++) {
		if (marker_present[seq_no] == 0) {
			return FALSE;		// missing sequence number
		}
		data_offset[seq_no] = total_length;
		total_length += data_length[seq_no];
	}
	
	if (total_length <= 0)
		return FALSE;		// found only empty markers ?
	
	// allocate space for assembled data 
	icc_data = (JOCTET *) malloc(total_length * sizeof(JOCTET));
	if (icc_data == NULL)
		return FALSE;		// out of memory
	
	// and fill it in
	for (marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
		if (marker_is_icc(marker)) {
			JOCTET FAR *src_ptr;
			JOCTET *dst_ptr;
			unsigned length;
			seq_no = GETJOCTET(marker->data[12]);
			dst_ptr = icc_data + data_offset[seq_no];
			src_ptr = marker->data + ICC_HEADER_SIZE;
			length = data_length[seq_no];
			while (length--) {
				*dst_ptr++ = *src_ptr++;
			}
		}
	}
	
	*icc_data_ptr = icc_data;
	*icc_data_len = total_length;
	
	return TRUE;
}

/**
	Read JPEG_APPD marker (IPTC or Adobe Photoshop profile)
*/
static BOOL 
jpeg_read_iptc_profile(FIBITMAP *dib, const BYTE *dataptr, unsigned int datalen) {
	return TRUE;
}

/**
	Read JPEG_APP1 marker (XMP profile)
	@param dib Input FIBITMAP
	@param dataptr Pointer to the APP1 marker
	@param datalen APP1 marker length
	@return Returns TRUE if successful, FALSE otherwise
*/
static BOOL  
jpeg_read_xmp_profile(FIBITMAP *dib, const BYTE *dataptr, unsigned int datalen) {
	// marker identifying string for XMP (null terminated)
	const char *xmp_signature = "http://ns.adobe.com/xap/1.0/";
	// XMP signature is 29 bytes long
	const size_t xmp_signature_size = strlen(xmp_signature) + 1;

	size_t length = datalen;
	BYTE *profile = (BYTE*)dataptr;

	if(length <= xmp_signature_size) {
		// avoid reading corrupted or empty data 
		return FALSE;
	}

	// verify the identifying string

	if(memcmp(xmp_signature, profile, strlen(xmp_signature)) == 0) {
		// XMP profile

		profile += xmp_signature_size;
		length  -= xmp_signature_size;

		return TRUE;
	}

	return FALSE;
}

/**
	Read JFIF "JFXX" extension APP0 marker
	@param dib Input FIBITMAP
	@param dataptr Pointer to the APP0 marker
	@param datalen APP0 marker length
	@return Returns TRUE if successful, FALSE otherwise
*/
static BOOL 
jpeg_read_jfxx(FIBITMAP *dib, const BYTE *dataptr, unsigned int datalen) {
	if(datalen < 6) {
		return FALSE;
	}
	
	const int id_length = 5;
	const BYTE *data = dataptr + id_length;
	unsigned remaining = datalen - id_length;
		
	const BYTE type = *data;
	++data, --remaining;

	switch(type) {
		case JFXX_TYPE_JPEG:
		{
			break;
		}
		case JFXX_TYPE_8bit:
			// colormapped uncompressed thumbnail (no supported)
			break;
		case JFXX_TYPE_24bit:
			// truecolor uncompressed thumbnail (no supported)
			break;
		default:
			break;
	}

	return TRUE;
}


/**
	Read JPEG special markers
*/
static BOOL 
read_markers(j_decompress_ptr cinfo, FIBITMAP *dib) {
	jpeg_saved_marker_ptr marker;

	for(marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
		switch(marker->marker) {
			case JPEG_APP0:
				// JFIF is handled by libjpeg already, handle JFXX
				if(memcmp(marker->data, "JFIF" , 5) == 0) {
					continue;
				}
				if(memcmp(marker->data, "JFXX" , 5) == 0) {
					if(!cinfo->saw_JFIF_marker || cinfo->JFIF_minor_version < 2) {
						Bitmap_OutputMessageProc(FIF_JPEG, "Warning: non-standard JFXX segment");
					}					
					jpeg_read_jfxx(dib, marker->data, marker->data_length);
				}
				// other values such as 'Picasa' : ignore safely unknown APP0 marker
				break;
			case JPEG_COM:
				// JPEG comment
				jpeg_read_comment(dib, marker->data, marker->data_length);
				break;
			case EXIF_MARKER:
				// Exif or Adobe XMP profile
				break;
			case IPTC_MARKER:
				// IPTC/NAA or Adobe Photoshop profile
				jpeg_read_iptc_profile(dib, marker->data, marker->data_length);
				break;
		}
	}

	// ICC profile
	BYTE *icc_profile = NULL;
	unsigned icc_length = 0;

	if( jpeg_read_icc_profile(cinfo, &icc_profile, &icc_length) ) {
		// copy ICC profile data
		Bitmap_CreateICCProfile(dib, icc_profile, icc_length);
		// clean up
		free(icc_profile);
	}

	return TRUE;
}


BOOL
ValidateJPEG(BitmapIO *io, fi_handle handle) {
	BYTE jpeg_signature[] = { 0xFF, 0xD8 };
	BYTE signature[2] = { 0, 0 };

	io->read_proc(signature, 1, sizeof(jpeg_signature), handle);

	return (memcmp(jpeg_signature, signature, sizeof(jpeg_signature)) == 0);
}

FIBITMAP *
LoadJPEG(BitmapIO *io, fi_handle handle, int page, int flags) {
	if (handle) {
		FIBITMAP *dib = NULL;

		BOOL header_only = (flags & FIF_LOAD_NOPIXELS) == FIF_LOAD_NOPIXELS;

		// set up the jpeglib structures

		struct jpeg_decompress_struct cinfo;
		ErrorManager fi_error_mgr;

		try {

			// step 1: allocate and initialize JPEG decompression object

			// we set up the normal JPEG error routines, then override error_exit & output_message
			cinfo.err = jpeg_std_error(&fi_error_mgr.pub);
			fi_error_mgr.pub.error_exit     = jpeg_error_exit;
			fi_error_mgr.pub.output_message = jpeg_output_message;
			
			// establish the setjmp return context for jpeg_error_exit to use
			if (setjmp(fi_error_mgr.setjmp_buffer)) {
				// If we get here, the JPEG code has signaled an error.
				// We need to clean up the JPEG object, close the input file, and return.
				jpeg_destroy_decompress(&cinfo);
				throw (const char*)NULL;
			}

			jpeg_create_decompress(&cinfo);

			// step 2a: specify data source (eg, a handle)

			jpeg_bitmap_src(&cinfo, handle, io);

			// step 2b: save special markers for later reading
			
			jpeg_save_markers(&cinfo, JPEG_COM, 0xFFFF);
			for(int m = 0; m < 16; m++) {
				jpeg_save_markers(&cinfo, JPEG_APP0 + m, 0xFFFF);
			}

			// step 3: read handle parameters with jpeg_read_header()

			jpeg_read_header(&cinfo, TRUE);

			// step 4: set parameters for decompression

			unsigned int scale_denom = 1;		// fraction by which to scale image
			int	requested_size = flags >> 16;	// requested user size in pixels
			if(requested_size > 0) {
				// the JPEG codec can perform x2, x4 or x8 scaling on loading
				// try to find the more appropriate scaling according to user's need
				double scale = MAX((double)cinfo.image_width, (double)cinfo.image_height) / (double)requested_size;
				if(scale >= 8) {
					scale_denom = 8;
				} else if(scale >= 4) {
					scale_denom = 4;
				} else if(scale >= 2) {
					scale_denom = 2;
				}
			}
			cinfo.scale_num = 1;
			cinfo.scale_denom = scale_denom;

			if ((flags & JPEG_ACCURATE) != JPEG_ACCURATE) {
				cinfo.dct_method          = JDCT_IFAST;
				cinfo.do_fancy_upsampling = FALSE;
			}

			if ((flags & JPEG_GREYSCALE) == JPEG_GREYSCALE) {
				// force loading as a 8-bit greyscale image
				cinfo.out_color_space = JCS_GRAYSCALE;
			}

			// step 5a: start decompressor and calculate output width and height

			jpeg_start_decompress(&cinfo);

			// step 5b: allocate dib and init header

			if((cinfo.output_components == 4) && (cinfo.out_color_space == JCS_CMYK)) {
				// CMYK image
				if((flags & JPEG_CMYK) == JPEG_CMYK) {
					// load as CMYK
					dib = Bitmap_AllocateHeader(header_only, cinfo.output_width, cinfo.output_height, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					if(!dib) throw FI_MSG_ERROR_DIB_MEMORY;
					Bitmap_GetICCProfile(dib)->flags |= FIICC_COLOR_IS_CMYK;
				} else {
					// load as CMYK and convert to RGB
					dib = Bitmap_AllocateHeader(header_only, cinfo.output_width, cinfo.output_height, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					if(!dib) throw FI_MSG_ERROR_DIB_MEMORY;
				}
			} else {
				// RGB or greyscale image
				dib = Bitmap_AllocateHeader(header_only, cinfo.output_width, cinfo.output_height, 8 * cinfo.output_components, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
				if(!dib) throw FI_MSG_ERROR_DIB_MEMORY;

				if (cinfo.output_components == 1) {
					// build a greyscale palette
					RGBQUAD *colors = Bitmap_GetPalette(dib);

					for (int i = 0; i < 256; i++) {
						colors[i].rgbRed   = (BYTE)i;
						colors[i].rgbGreen = (BYTE)i;
						colors[i].rgbBlue  = (BYTE)i;
					}
				}
			}

			// step 5c: handle metrices

			if (cinfo.density_unit == 1) {
				// dots/inch
				Bitmap_SetDotsPerMeterX(dib, (unsigned)(((float)cinfo.X_density) / 0.0254000 + 0.5));
				Bitmap_SetDotsPerMeterY(dib, (unsigned)(((float)cinfo.Y_density) / 0.0254000 + 0.5));
			} else if (cinfo.density_unit == 2) {
				// dots/cm
				Bitmap_SetDotsPerMeterX(dib, (unsigned)(cinfo.X_density * 100));
				Bitmap_SetDotsPerMeterY(dib, (unsigned)(cinfo.Y_density * 100));
			}
			
			// step 6: read special markers
			
			read_markers(&cinfo, dib);

			// --- header only mode => clean-up and return

			if (header_only) {
				// release JPEG decompression object
				jpeg_destroy_decompress(&cinfo);
				// return header data
				return dib;
			}

			// step 7a: while (scan lines remain to be read) jpeg_read_scanlines(...);

			if((cinfo.out_color_space == JCS_CMYK) && ((flags & JPEG_CMYK) != JPEG_CMYK)) {
				// convert from CMYK to RGB

				JSAMPARRAY buffer;		// output row buffer
				unsigned row_stride;	// physical row width in output buffer

				// JSAMPLEs per row in output buffer
				row_stride = cinfo.output_width * cinfo.output_components;
				// make a one-row-high sample array that will go away when done with image
				buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

				while (cinfo.output_scanline < cinfo.output_height) {
					JSAMPROW src = buffer[0];
					JSAMPROW dst = Bitmap_GetScanLine(dib, cinfo.output_height - cinfo.output_scanline - 1);

					jpeg_read_scanlines(&cinfo, buffer, 1);

					for(unsigned x = 0; x < cinfo.output_width; x++) {
						WORD K = (WORD)src[3];
						dst[FI_RGBA_RED]   = (BYTE)((K * src[0]) / 255);	// C -> R
						dst[FI_RGBA_GREEN] = (BYTE)((K * src[1]) / 255);	// M -> G
						dst[FI_RGBA_BLUE]  = (BYTE)((K * src[2]) / 255);	// Y -> B
						src += 4;
						dst += 3;
					}
				}
			} else if((cinfo.out_color_space == JCS_CMYK) && ((flags & JPEG_CMYK) == JPEG_CMYK)) {
				// convert from LibJPEG CMYK to standard CMYK

				JSAMPARRAY buffer;		// output row buffer
				unsigned row_stride;	// physical row width in output buffer

				// JSAMPLEs per row in output buffer
				row_stride = cinfo.output_width * cinfo.output_components;
				// make a one-row-high sample array that will go away when done with image
				buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

				while (cinfo.output_scanline < cinfo.output_height) {
					JSAMPROW src = buffer[0];
					JSAMPROW dst = Bitmap_GetScanLine(dib, cinfo.output_height - cinfo.output_scanline - 1);

					jpeg_read_scanlines(&cinfo, buffer, 1);

					for(unsigned x = 0; x < cinfo.output_width; x++) {
						// CMYK pixels are inverted
						dst[0] = ~src[0];	// C
						dst[1] = ~src[1];	// M
						dst[2] = ~src[2];	// Y
						dst[3] = ~src[3];	// K
						src += 4;
						dst += 4;
					}
				}

			} else {
				// normal case (RGB or greyscale image)

				while (cinfo.output_scanline < cinfo.output_height) {
					JSAMPROW dst = Bitmap_GetScanLine(dib, cinfo.output_height - cinfo.output_scanline - 1);

					jpeg_read_scanlines(&cinfo, &dst, 1);
				}

				// step 7b: swap red and blue components (see LibJPEG/jmorecfg.h: #define RGB_RED, ...)
				// The default behavior of the JPEG library is kept "as is" because LibTIFF uses 
				// LibJPEG "as is".

#if BITMAP_COLORORDER == BITMAP_COLORORDER_BGR
				SwapRedBlue32(dib);
#endif
			}

			// step 8: finish decompression

			jpeg_finish_decompress(&cinfo);

			// step 9: release JPEG decompression object

			jpeg_destroy_decompress(&cinfo);

			// check for automatic Exif rotation
			if(!header_only && ((flags & JPEG_EXIFROTATE) == JPEG_EXIFROTATE)) {
//				RotateExif(&dib);
			}

			// everything went well. return the loaded dib

			return dib;

		} catch (const char *text) {
			jpeg_destroy_decompress(&cinfo);
			if(NULL != dib) {
				Bitmap_Unload(dib);
			}
			if(NULL != text) {
				Bitmap_OutputMessageProc(FIF_JPEG, text);
			}
		}
	}

	return NULL;
}
