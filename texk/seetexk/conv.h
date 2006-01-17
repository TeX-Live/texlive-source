/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Conversions.  Conversion factors convert between values in scaled
 * points and values in device-dependent units.  The results of all
 * conversions are rounded to the nearest integral value, of type (i32).
 */

/*
 * This is now done using `double' values, but may be changed to
 * fixed-point or some other `fast' method, as long as the results
 * are consistent and reasonably accurate.  The structure `conversion'
 * holds the conversion-method-dependent quantities; the macros
 * fromSP and toSP apply the conversion to a value.  (Note that
 * fromSP and toSP need not be macros, but should be fast.)
 *
 * SetConversion sets the (single, global) conversion factor.
 * If a driver needs special conversions, there is another routine,
 * CSetConversion that sets a specific conversion, and cfromSP and
 * ctoSP to apply these.
 *
 * IS USING DOTS PER INCH SUFFICIENT?  (Pixels per point might be better.)
 *
 * Note that it is necessary to set the global conversion factor before
 * using any fonts.
 */
typedef struct conversion {
	double	c_fromsp;	/* multiplier to convert from scaled points */
	double	c_tosp;		/* multiplier to convert to scaled points:
				   could divide by c_fromsp, but this should
				   be faster and more accurate */
	double	c_mag;		/* the magnification this conversion
				   represents; mainly for GetFont() */
	double	c_dpi;		/* dpi (should be pixels per point?) */
} Conv;

extern Conv Conversion;		/* the global conversion factor */

/*
 * In order to do this, we need to round properly.  The compilers I
 * have tend to generate very poor code for this.  The following is
 * intended to help them out.  Smarter compilers can do better, but
 * if they are smart enough, they will realise that the variables
 * here are not used anywhere else, and discard them.  (For a compiler
 * to do this given separate compliation, `static' is a must.)
 */
#ifdef lint			/* or a smart compiler */

#define	ROUND(f) ((i32) ((f) < 0.0 ? (f) - 0.5 : (f) + 0.5))
#define	CEIL(f)	((double) (i32) (f) < (f) ? (i32) (f) + 1 : (i32) (f))

#else /* lint */

static double _half = 0.5;
static double _d;
#define	ROUND(f) ((i32) (_d = (f), _d < 0.0 ? _d - _half : _d + _half))

#ifdef NEGATIVE_FLOAT_ROUNDS_TO_NEGATIVE_INFINITY
#define	CEIL(f)  (-(i32) -(f))
#else /* we will assume that floating to integer truncates */
static i32 _i;
#define	CEIL(f)	 (_i = _d = (f), _i < _d ? _i + 1 : _i)
#endif /* round towards negative infinity */

#endif /* lint */

#define	SetConversion(dpi, usermag, num, denom, dvimag)	\
	CSetConversion(&Conversion, dpi, usermag, num, denom, dvimag)

#define	cfromSP(c, v)	ROUND((c)->c_fromsp * (v))
#define	ctoSP(c, v)	ROUND((c)->c_tosp * (v))

#define	fromSP(v)	cfromSP(&Conversion, v)
#define	toSP(v)		ctoSP(&Conversion, v)

/*
 * Conversions for rules are a bit different: we must round up, rather
 * than off.  ConvRule applies the global conversion value for a rule
 * value (height or width); CConvRule applies a specific conversion.
 */
#define	CConvRule(c, v)	CEIL((c)->c_fromsp * (v))
#define	ConvRule(v)	CConvRule(&Conversion, v)

void	CSetConversion();
