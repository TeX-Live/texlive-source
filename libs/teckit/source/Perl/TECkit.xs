#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "TECkit_Engine.h"

#define STYLE_InUni     1
#define STYLE_OutUni    2

typedef TECkit_Converter Encode__TECkit;

MODULE = Encode::TECkit     PACKAGE = Encode::TECkit

Encode::TECkit
new_conv(mapping, mapForward, style)
        char    *mapping
        Byte    mapForward
        Byte    style
    PREINIT:
        UInt16  sourceForm;
        UInt16  targetForm;
        TECkit_Status   hr;
        PerlIO *infile;
        UInt32  filelen;
        Byte    *fBuff;
        SV      *mySv;
    PPCODE:
        if (style & 1)
            sourceForm = kForm_UTF8;
        else
            sourceForm = kForm_Bytes;
        if (style & 2)
            targetForm = kForm_UTF8;
        else
            targetForm = kForm_Bytes;
        
        infile = PerlIO_open(mapping, "rb");
        if (!infile)
            XSRETURN_UNDEF;
        PerlIO_seek(infile, 0, 2);
        filelen = PerlIO_tell(infile);
        New(1, fBuff, filelen, Byte);
        PerlIO_seek(infile, 0, 0);
        PerlIO_read(infile, fBuff, filelen);
        PerlIO_close(infile);
        
        hr = TECkit_CreateConverter(fBuff, filelen, mapForward, sourceForm, targetForm, (TECkit_Converter *)&RETVAL);
        EXTEND(SP, 2);
        XSprePUSH;
        if (hr)
        {
            hr = -hr;
            PUSHs(&PL_sv_undef);
            PUSHs(sv_2mortal(newSViv(hr)));
        }
        else
        {
            mapForward = 0;
        	mySv = sv_newmortal();
    	    sv_setref_pv(mySv, "Encode::TECkit", (void*)RETVAL);
            PUSHs(mySv);
            PUSHs(sv_2mortal(newSViv(0)));
        }
        XSRETURN(2);
        
Encode::TECkit
new_conv_scalar(mapping, mapForward, style)
        SV     *mapping
        Byte    mapForward
        Byte    style
    PREINIT:
        UInt16  sourceForm;
        UInt16  targetForm;
        TECkit_Status   hr;
        Byte   *fBuff;
        STRLEN  filelen;
        SV     *mySv;
    PPCODE:
        if (style & 1)
            sourceForm = kForm_UTF8;
        else
            sourceForm = kForm_Bytes;
        if (style & 2)
            targetForm = kForm_UTF8;
        else
            targetForm = kForm_Bytes;
        
        fBuff = (Byte *)SvPV(mapping, filelen);
        
        hr = TECkit_CreateConverter(fBuff, (long)filelen, mapForward, sourceForm, targetForm, (TECkit_Converter *)&RETVAL);
        EXTEND(SP, 2);
        XSprePUSH;
        if (hr)
        {
            hr = -hr;
            PUSHs(&PL_sv_undef);
            PUSHs(sv_2mortal(newSViv(hr)));
        }
        else
        {
            mapForward = 0;
        	mySv = sv_newmortal();
    	    sv_setref_pv(mySv, "Encode::TECkit", (void*)RETVAL);
            PUSHs(mySv);
            PUSHs(sv_2mortal(newSViv(0)));
        }
        XSRETURN(2);
        
SV *
convert(converter, input, style, isComplete)
        Encode::TECkit converter
        SV      *input
        Byte    style
        Byte    isComplete
    PREINIT:
        Byte    *inBuff;
        Byte    *outBuff;
        STRLEN  pvLen;
        UInt32  inLen;
        UInt32  outLen;
        TECkit_Status   hr;
    CODE:
        inBuff = (Byte *)SvPV(input, pvLen);
        inLen = (UInt32)pvLen;
        New(0, outBuff, inLen * 4, Byte);
        outLen = inLen * 4;
        while ((hr = TECkit_ConvertBuffer((TECkit_Converter)converter, inBuff, inLen, 0, outBuff, 
                        outLen, &outLen, isComplete)) == kStatus_OutputBufferFull)
        {
            Safefree(outBuff);
            New(0, outBuff, outLen * 2, Byte);
            outLen = outLen * 2;
            if (isComplete)
                TECkit_ResetConverter((TECkit_Converter)converter);
        }
        hr = -hr;
        if (isComplete)
            TECkit_ResetConverter((TECkit_Converter)converter);
        isComplete = (Byte)hr;
        RETVAL = newSVpv((char *)outBuff, (STRLEN)outLen);
        if (style & 2)
            SvUTF8_on(RETVAL);
        else
            SvUTF8_off(RETVAL);
    OUTPUT:
        RETVAL
        isComplete

SV *
flush(converter, style, hr)
        Encode::TECkit converter
        Byte    style
        Byte    hr = NO_INIT
    PREINIT:
        Byte    *outBuff;
        UInt32  outLen;
    CODE:
        outLen = 128;
        New(0, outBuff, outLen, Byte);
        hr = (Byte)TECkit_Flush((TECkit_Converter)converter, outBuff, outLen, &outLen);
        RETVAL = newSVpv((char *)outBuff, (STRLEN)outLen);
        if (style & 2)
            SvUTF8_on(RETVAL);
        else
            SvUTF8_off(RETVAL);
        TECkit_ResetConverter((TECkit_Converter)converter);
    OUTPUT:
        RETVAL

void
DESTROY(converter)
        Encode::TECkit converter
    CODE:
        TECkit_DisposeConverter((TECkit_Converter)converter);
        
