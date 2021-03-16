#include "dep/json-builder.h"
#include "otfcc/sfnt.h"
#include "otfcc/font.h"
#include "otfcc/sfnt-builder.h"
#include "aliases.h"
#include "platform.h"
//#include "stopwatch.h"

#ifndef _MSC_VER
#include <unistd.h>
#endif /* _MSC_VER */

/* to reset the getopt_long */
extern int optind;

int otfcc_build(int b_argc, char *b_argv[]);
int otfcc_dump(int b_argc, char *b_argv[]);

extern char *xstrdup(const char *);


#ifndef MAIN_VER
#define MAIN_VER 0
#endif
#ifndef SECONDARY_VER
#define SECONDARY_VER 0
#endif
#ifndef PATCH_VER
#define PATCH_VER 0
#endif

static void dump_printInfo() {
	fprintf(stdout, "This is dump, version %d.%d.%d.\n", MAIN_VER, SECONDARY_VER,
	        PATCH_VER);
}

static void build_printInfo() {
	fprintf(stdout, "This is build, version %d.%d.%d.\n", MAIN_VER, SECONDARY_VER,
	        PATCH_VER);
}

static void dump_printHelp() {
	fprintf(stdout,
	        "\n"
	        "Usage : dump [OPTIONS] input.[otf|ttf|ttc]\n\n"
	        " -h, --help              : Display this help message and exit.\n"
	        " -v, --version           : Display version information and exit.\n"
	        " -o <file>               : Set output file path to <file>. When absent the dump\n"
	        "                           will be written to STDOUT.\n"
	        " -n <n>, --ttc-index <n> : Use the <n>th subfont within the input font.\n"
	        " --pretty                : Prettify the output JSON.\n"
	        " --ugly                  : Force uglify the output JSON.\n"
	        " --verbose               : Show more information when building.\n"
	        " -q, --quiet             : Be silent when building.\n\n"
	        " --ignore-glyph-order    : Do not export glyph order information.\n"
	        " --glyph-name-prefix pfx : Add a prefix to the glyph names.\n"
	        " --ignore-hints          : Do not export hinting information.\n"
	        " --decimal-cmap          : Export 'cmap' keys as decimal number.\n"
	        " --hex-cmap              : Export 'cmap' keys as hex number (U+FFFF).\n"
	        " --name-by-hash          : Name glyphs using its hash value.\n"
	        " --name-by-gid           : Name glyphs using its glyph id.\n"
	        " --add-bom               : Add BOM mark in the output. (It is default on Windows\n"
	        "                           when redirecting to another program. Use --no-bom to\n"
	        "                           turn it off.)\n"
	        "\n");
}


static void build_printHelp() {
	fprintf(stdout,
	        "\n"
	        "Usage : build [OPTIONS] [input.json] -o output.[ttf|otf]\n\n"
	        " input.json                : Path to input file. When absent the input will be\n"
	        "                             read from the STDIN.\n\n"
	        " -h, --help                : Display this help message and exit.\n"
	        " -v, --version             : Display version information and exit.\n"
	        " -o <file>                 : Set output file path to <file>.\n"
	        " -s, --dummy-dsig          : Include an empty DSIG table in the font. For some\n"
	        "                             Microsoft applications, DSIG is required to enable\n"
	        "                             OpenType features.\n"
	        " -O<n>                     : Specify the level for optimization.\n"
	        "     -O0                     Turn off any optimization.\n"
	        "     -O1                     Default optimization.\n"
	        "     -O2                     More aggressive optimizations for web font. In this\n"
	        "                             level, the following options will be set:\n"
	        "                               --merge-features\n"
	        "                               --short-post\n"
	        "                               --subroutinize\n"
	        "     -O3                     Most aggressive opptimization strategy will be\n"
	        "                             used. In this level, these options will be set:\n"
	        "                               --force-cid\n"
	        "                               --ignore-glyph-order\n"
	        " --verbose                 : Show more information when building.\n"
	        " -q, --quiet               : Be silent when building.\n\n"
	        " --ignore-hints            : Ignore the hinting information in the input.\n"
	        " --keep-average-char-width : Keep the OS/2.xAvgCharWidth value from the input\n"
	        "                             instead of stating the average width of glyphs.\n"
	        "                             Useful when creating a monospaced font.\n"
	        " --keep-unicode-ranges     : Keep the OS/2.ulUnicodeRange[1-4] as-is.\n"
	        " --keep-modified-time      : Keep the head.modified time in the json, instead of\n"
	        "                             using current time.\n\n"
	        " --short-post              : Don't export glyph names in the result font.\n"
	        " --ignore-glyph-order, -i  : Ignore the glyph order information in the input.\n"
	        " --keep-glyph-order, -k    : Keep the glyph order information in the input.\n"
	        "                             Use to preserve glyph order under -O2 and -O3.\n"
	        " --dont-ignore-glyph-order : Same as --keep-glyph-order.\n"
	        " --merge-features          : Merge duplicate OpenType feature definitions.\n"
	        " --dont-merge-features     : Keep duplicate OpenType feature definitions.\n"
	        " --merge-lookups           : Merge duplicate OpenType lookups.\n"
	        " --dont-merge-lookups      : Keep duplicate OpenType lookups.\n"
	        " --force-cid               : Convert name-keyed CFF OTF into CID-keyed.\n"
	        " --subroutinize            : Subroutinize CFF table.\n"
	        " --stub-cmap4              : Create a stub `cmap` format 4 subtable if format\n"
	        "                             12 subtable is present.\n"
	        "\n");
}


static void build_readEntireFile(char *inPath, char **_buffer, long *_length) {
	char *buffer = NULL;
	long length = 0;
	FILE *f = u8fopen(inPath, "rb");
	if (!f) {
		fprintf(stderr, "Cannot read JSON file \"%s\". Exit.\n", inPath);
		exit(EXIT_FAILURE);
	}
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = malloc(length);
	if (buffer) { fread(buffer, 1, length, f); }
	fclose(f);

	if (!buffer) {
		fprintf(stderr, "Cannot read JSON file \"%s\". Exit.\n", inPath);
		exit(EXIT_FAILURE);
	}
	*_buffer = buffer;
	*_length = length;
}


int otfcc_build(int b_argc, char *b_argv[]) {
//	struct timespec begin;
//	time_now(&begin);

	bool show_help = false;
	bool show_version = false;
	sds outputPath = NULL;
	sds inPath = NULL;
	int option_index = 0;
	int c;

	optind=0;
        if ((b_argc==0)||(b_argv==NULL))
          return 0;

	otfcc_Options *options = otfcc_newOptions();
	options->logger = otfcc_newLogger(otfcc_newStdErrTarget());
	options->logger->indent(options->logger, "build");
	otfcc_Options_optimizeTo(options, 1);

	struct option longopts[] = {{"version", no_argument, NULL, 'v'},
	                            {"help", no_argument, NULL, 'h'},
	                            {"time", no_argument, NULL, 0},
	                            {"ignore-glyph-order", no_argument, NULL, 0},
	                            {"keep-glyph-order", no_argument, NULL, 0},
	                            {"dont-ignore-glyph-order", no_argument, NULL, 0},
	                            {"ignore-hints", no_argument, NULL, 0},
	                            {"keep-average-char-width", no_argument, NULL, 0},
	                            {"keep-unicode-ranges", no_argument, NULL, 0},
	                            {"keep-modified-time", no_argument, NULL, 0},
	                            {"merge-lookups", no_argument, NULL, 0},
	                            {"merge-features", no_argument, NULL, 0},
	                            {"dont-merge-lookups", no_argument, NULL, 0},
	                            {"dont-merge-features", no_argument, NULL, 0},
	                            {"short-post", no_argument, NULL, 0},
	                            {"force-cid", no_argument, NULL, 0},
	                            {"subroutinize", no_argument, NULL, 0},
	                            {"stub-cmap4", no_argument, NULL, 0},
	                            {"dummy-dsig", no_argument, NULL, 's'},
	                            {"ship", no_argument, NULL, 0},
	                            {"verbose", no_argument, NULL, 0},
	                            {"quiet", no_argument, NULL, 0},
	                            {"optimize", required_argument, NULL, 'O'},
	                            {"output", required_argument, NULL, 'o'},
	                            {0, 0, 0, 0}};

	while ((c = getopt_long(b_argc, b_argv, "vhqskiO:o:", longopts, &option_index)) != (-1)) {
		switch (c) {
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (longopts[option_index].flag != 0) {
					break;
				} else if (strcmp(longopts[option_index].name, "time") == 0) {
				} else if (strcmp(longopts[option_index].name, "ignore-hints") == 0) {
					options->ignore_hints = true;
				} else if (strcmp(longopts[option_index].name, "keep-average-char-width") == 0) {
					options->keep_average_char_width = true;
				} else if (strcmp(longopts[option_index].name, "keep-unicode-ranges") == 0) {
					options->keep_unicode_ranges = true;
				} else if (strcmp(longopts[option_index].name, "keep-modified-time") == 0) {
					options->keep_modified_time = true;
				} else if (strcmp(longopts[option_index].name, "merge-features") == 0) {
					options->merge_features = true;
				} else if (strcmp(longopts[option_index].name, "merge-lookups") == 0) {
					options->merge_lookups = true;
				} else if (strcmp(longopts[option_index].name, "dont-merge-features") == 0) {
					options->merge_features = false;
				} else if (strcmp(longopts[option_index].name, "dont-merge-lookups") == 0) {
					options->merge_lookups = false;
				} else if (strcmp(longopts[option_index].name, "ignore-glyph-order") == 0) {
					options->ignore_glyph_order = true;
				} else if (strcmp(longopts[option_index].name, "keep-glyph-order") == 0) {
					options->ignore_glyph_order = false;
				} else if (strcmp(longopts[option_index].name, "dont-keep-glyph-order") == 0) {
					options->ignore_glyph_order = false;
				} else if (strcmp(longopts[option_index].name, "short-post") == 0) {
					options->short_post = true;
				} else if (strcmp(longopts[option_index].name, "force-cid") == 0) {
					options->force_cid = true;
				} else if (strcmp(longopts[option_index].name, "subroutinize") == 0) {
					options->cff_doSubroutinize = true;
				} else if (strcmp(longopts[option_index].name, "stub-cmap4") == 0) {
					options->stub_cmap4 = true;
				} else if (strcmp(longopts[option_index].name, "ship") == 0) {
					options->ignore_glyph_order = true;
					options->short_post = true;
					options->dummy_DSIG = true;
				} else if (strcmp(longopts[option_index].name, "verbose") == 0) {
					options->verbose = true;
				} else if (strcmp(longopts[option_index].name, "quiet") == 0) {
					options->quiet = true;
				}
				break;
			case 'v':
				show_version = true;
				break;
			case 'h':
				show_help = true;
				break;
			case 'k':
				options->ignore_glyph_order = false;
				break;
			case 'i':
				options->ignore_glyph_order = true;
				break;
			case 'o':
				outputPath = sdsnew(optarg);
				break;
			case 's':
				options->dummy_DSIG = true;
				break;
			case 'q':
				options->quiet = true;
				break;
			case 'O':
				otfcc_Options_optimizeTo(options, atoi(optarg));
				break;
		}
	}
	options->logger->setVerbosity(options->logger,
	                              options->quiet ? 0 : options->verbose ? 0xFF : 1);
	if (show_help) {
		build_printInfo();
		build_printHelp();
		return 0;
	}
	if (show_version) {
		build_printInfo();
		return 0;
	}

	if (optind >= b_argc) {
		inPath = NULL; // read from STDIN
	} else {
		inPath = sdsnew(b_argv[optind]); // read from file
	}
	if (!outputPath) {
		logError("Unable to build OpenType font tile : output path not "
		         "specified. Exit.\n");
		build_printHelp();
		exit(EXIT_FAILURE);
	}

	char *buffer;
	long length;
	loggedStep("Load file") {
		if (inPath) {
			loggedStep("Load from file %s", inPath) {
				build_readEntireFile(inPath, &buffer, &length);
				sdsfree(inPath);
			}
		} else {
			/*loggedStep("Load from stdin") {
				build_readEntireStdin(&buffer, &length);
			}*/
		}
		//logStepTime;
	}

	json_value *jsonRoot = NULL;
	loggedStep("Parse into JSON") {
		jsonRoot = json_parse(buffer, length);
		free(buffer);
		//logStepTime;
		if (!jsonRoot) {
			logError("Cannot parse JSON file \"%s\". Exit.\n", inPath);
			exit(EXIT_FAILURE);
		}
	}

	otfcc_Font *font;
	loggedStep("Parse") {
		otfcc_IFontBuilder *parser = otfcc_newJsonReader();
		font = parser->read(jsonRoot, 0, options);
		if (!font) {
			logError("Cannot parse JSON file \"%s\" as a font. Exit.\n", inPath);
			exit(EXIT_FAILURE);
		}
		parser->free(parser);
		json_value_free(jsonRoot);
		//logStepTime;
	}
	loggedStep("Consolidate") {
		otfcc_iFont.consolidate(font, options);
		//logStepTime;
	}
	loggedStep("Build") {
		otfcc_IFontSerializer *writer = otfcc_newOTFWriter();
		caryll_Buffer *otf = (caryll_Buffer *)writer->serialize(font, options);
		loggedStep("Write to file") {
			FILE *outfile = u8fopen(outputPath, "wb");
			if (!outfile) {
				logError("Cannot write to file \"%s\". Exit.\n", outputPath);
				exit(EXIT_FAILURE);
			}
			fwrite(otf->data, sizeof(uint8_t), buflen(otf), outfile);
			fclose(outfile);
		}
		//logStepTime;
		buffree(otf), writer->free(writer), otfcc_iFont.free(font), sdsfree(outputPath);
	}
	otfcc_deleteOptions(options);

	return 0;
}



int otfcc_dump(int b_argc, char *b_argv[]) {
	bool show_help = false;
	bool show_version = false;
	bool show_pretty = false;
	bool show_ugly = false;
	bool add_bom = false;
	bool no_bom = false;
	uint32_t ttcindex = 0;
	struct option longopts[] = {{"version", no_argument, NULL, 'v'},
	                            {"help", no_argument, NULL, 'h'},
	                            {"pretty", no_argument, NULL, 'p'},
	                            {"ugly", no_argument, NULL, 0},
	                            {"time", no_argument, NULL, 0},
	                            {"ignore-glyph-order", no_argument, NULL, 0},
	                            {"ignore-hints", no_argument, NULL, 0},
	                            {"hex-cmap", no_argument, NULL, 0},
	                            {"decimal-cmap", no_argument, NULL, 0},
	                            {"instr-as-bytes", no_argument, NULL, 0},
	                            {"name-by-hash", no_argument, NULL, 0},
	                            {"name-by-gid", no_argument, NULL, 0},
	                            {"glyph-name-prefix", required_argument, NULL, 0},
	                            {"verbose", no_argument, NULL, 0},
	                            {"quiet", no_argument, NULL, 0},
	                            {"add-bom", no_argument, NULL, 0},
	                            {"no-bom", no_argument, NULL, 0},
	                            {"output", required_argument, NULL, 'o'},
	                            {"ttc-index", required_argument, NULL, 'n'},
	                            {"debug-wait-on-start", no_argument, NULL, 0},
	                            {0, 0, 0, 0}};

	optind=0;
        if ((b_argc==0)||(b_argv==NULL))
          return 0;
	otfcc_Options *options = otfcc_newOptions();
	options->logger = otfcc_newLogger(otfcc_newStdErrTarget());
	options->logger->indent(options->logger, "dump");
	options->decimal_cmap = true;

	int option_index = 0;
	int c;

	sds outputPath = NULL;
	sds inPath = NULL;

	while ((c = getopt_long(b_argc, b_argv, "vhqpio:n:", longopts, &option_index)) != (-1)) {
		switch (c) {
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (longopts[option_index].flag != 0) {
					break;
				} else if (strcmp(longopts[option_index].name, "ugly") == 0) {
					show_ugly = true;
				} else if (strcmp(longopts[option_index].name, "time") == 0) {
				} else if (strcmp(longopts[option_index].name, "add-bom") == 0) {
					add_bom = true;
				} else if (strcmp(longopts[option_index].name, "no-bom") == 0) {
					no_bom = true;
				} else if (strcmp(longopts[option_index].name, "ignore-glyph-order") == 0) {
					options->ignore_glyph_order = true;
				} else if (strcmp(longopts[option_index].name, "verbose") == 0) {
					options->verbose = true;
				} else if (strcmp(longopts[option_index].name, "quiet") == 0) {
					options->quiet = true;
				} else if (strcmp(longopts[option_index].name, "ignore-hints") == 0) {
					options->ignore_hints = true;
				} else if (strcmp(longopts[option_index].name, "decimal-cmap") == 0) {
					options->decimal_cmap = true;
				} else if (strcmp(longopts[option_index].name, "hex-cmap") == 0) {
					options->decimal_cmap = false;
				} else if (strcmp(longopts[option_index].name, "name-by-hash") == 0) {
					options->name_glyphs_by_hash = true;
				} else if (strcmp(longopts[option_index].name, "name-by-gid") == 0) {
					options->name_glyphs_by_gid = true;
				} else if (strcmp(longopts[option_index].name, "instr-as-bytes") == 0) {
					options->instr_as_bytes = true;
				} else if (strcmp(longopts[option_index].name, "glyph-name-prefix") == 0) {
					options->glyph_name_prefix = xstrdup(optarg);
				} else if (strcmp(longopts[option_index].name, "debug-wait-on-start") == 0) {
					options->debug_wait_on_start = true;
				}
				break;
			case 'v':
				show_version = true;
				break;
			case 'i':
				options->ignore_glyph_order = true;
				break;
			case 'h':
				show_help = true;
				break;
			case 'p':
				show_pretty = true;
				break;
			case 'o':
				outputPath = sdsnew(optarg);
				break;
			case 'q':
				options->quiet = true;
				break;
			case 'n':
				ttcindex = atoi(optarg);
				break;
		}
	}

	if (options->debug_wait_on_start) { getchar(); }

	options->logger->setVerbosity(options->logger,
	                              options->quiet ? 0 : options->verbose ? 0xFF : 1);

	if (show_help) {
		dump_printInfo();
		dump_printHelp();
		return 0;
	}
	if (show_version) {
		dump_printInfo();
		return 0;
	}

	if (optind >= b_argc) {
		logError("Expected argument for input file name.\n");
		dump_printHelp();
		exit(EXIT_FAILURE);
	} else {
		inPath = sdsnew(b_argv[optind]);
	}

//	struct timespec begin;

//	time_now(&begin);

	otfcc_SplineFontContainer *sfnt;
	loggedStep("Read SFNT") {
		logProgress("From file %s", inPath);
		FILE *file = u8fopen(inPath, "rb");
		sfnt = otfcc_readSFNT(file);
		if (!sfnt || sfnt->count == 0) {
			logError("Cannot read SFNT file \"%s\". Exit.\n", inPath);
			exit(EXIT_FAILURE);
		}
		if (ttcindex >= sfnt->count) {
			logError("Subfont index %d out of range for \"%s\" (0 -- %d). Exit.\n", ttcindex,
			         inPath, (sfnt->count - 1));
			exit(EXIT_FAILURE);
		}
		//logStepTime;
	}

	otfcc_Font *font;
	loggedStep("Read Font") {
		otfcc_IFontBuilder *reader = otfcc_newOTFReader();
		font = reader->read(sfnt, ttcindex, options);
		if (!font) {
			logError("Font structure broken or corrupted \"%s\". Exit.\n", inPath);
			exit(EXIT_FAILURE);
		}
		reader->free(reader);
		if (sfnt) otfcc_deleteSFNT(sfnt);
		//logStepTime;
	}
	loggedStep("Consolidate") {
		otfcc_iFont.consolidate(font, options);
		//logStepTime;
	}
	json_value *root;
	loggedStep("Dump") {
		otfcc_IFontSerializer *dumper = otfcc_newJsonWriter();
		root = (json_value *)dumper->serialize(font, options);
		if (!root) {
			logError("Font structure broken or corrupted \"%s\". Exit.\n", inPath);
			exit(EXIT_FAILURE);
		}
		//logStepTime;
		dumper->free(dumper);
	}

	char *buf;
	size_t buflen;
	loggedStep("Serialize to JSON") {
		json_serialize_opts jsonOptions;
		jsonOptions.mode = json_serialize_mode_packed;
		jsonOptions.opts = 0;
		jsonOptions.indent_size = 4;
#ifdef WIN32
		if (show_pretty || (!outputPath && isatty(fileno(stdout)))) {
			jsonOptions.mode = json_serialize_mode_multiline;
		}
#else
		if (show_pretty || (!outputPath && isatty(STDOUT_FILENO))) {
			jsonOptions.mode = json_serialize_mode_multiline;
		}
#endif
		if (show_ugly) jsonOptions.mode = json_serialize_mode_packed;
		buflen = json_measure_ex(root, jsonOptions);
		buf = calloc(1, buflen);
		json_serialize_ex(buf, root, jsonOptions);
		//logStepTime;
	}

	loggedStep("Output") {
		if (outputPath) {
			FILE *outputFile = u8fopen(outputPath, "wb");
			if (!outputFile) {
				logError("Cannot write to file \"%s\". Exit.", outputPath);
				exit(EXIT_FAILURE);
			}
			if (add_bom) {
				fputc(0xEF, outputFile);
				fputc(0xBB, outputFile);
				fputc(0xBF, outputFile);
			}
			size_t actualLen = buflen - 1;
			while (!buf[actualLen])
				actualLen -= 1;
			fwrite(buf, sizeof(char), actualLen + 1, outputFile);
			fclose(outputFile);
		} else {
#ifdef WIN32
			if (isatty(fileno(stdout))) {
				LPWSTR pwStr;
				DWORD dwNum = widen_utf8(buf, &pwStr);
				DWORD actual = 0;
				DWORD written = 0;
				const DWORD chunk = 0x10000;
				while (written < dwNum) {
					DWORD len = dwNum - written;
					if (len > chunk) len = chunk;
					WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), pwStr + written, len, &actual,
					              NULL);
					written += len;
				}
				free(pwStr);
			} else {
				if (!no_bom) {
					fputc(0xEF, stdout);
					fputc(0xBB, stdout);
					fputc(0xBF, stdout);
				}
				fputs(buf, stdout);
			}
#else
			(void)(no_bom);
			if (add_bom) {
				fputc(0xEF, stdout);
				fputc(0xBB, stdout);
				fputc(0xBF, stdout);
			}
			fputs(buf, stdout);
#endif
		}
		//logStepTime;
	}

	loggedStep("Finalize") {
		free(buf);
		if (font) otfcc_iFont.free(font);
		if (root) json_builder_free(root);
		if (inPath) sdsfree(inPath);
		if (outputPath) sdsfree(outputPath);
		//logStepTime;
	}
	otfcc_deleteOptions(options);

	return 0;
}



