/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2003 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrJustifier.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	A default justification agent for Graphite.
-------------------------------------------------------------------------------*//*:End Ignore*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
//#include "main.h" // This is used by clients, so main.h is not available

#pragma hdrstop
// any other headers (not precompiled)
#include "GrClient.h"
#include "ITextSource.h"
#include "IGrJustifier.h"
#include "GraphiteProcess.h"
#include "GrDebug.h"

#include "GrJustifier.h"

#ifdef WIN32
#include <string>
#endif

//:>********************************************************************************************
//:>	Global constants
//:>********************************************************************************************

const int g_cnPrimes = 7;
static const int g_rgnPrimes[] =
{
	2, 3, 5, 7, 11, 13, 17, // these primes will allow a range of weights up to 255
	// 19, 23, 31, 37, 41, 43, 47, 53, 59, 61
};

//:>********************************************************************************************
//:>	Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	Methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Constructors.
----------------------------------------------------------------------------------------------*/
GrJustifier::GrJustifier()
{
	m_cref = 1; // COM-like behavior
}


/*----------------------------------------------------------------------------------------------
	Destructor.
----------------------------------------------------------------------------------------------*/
GrJustifier::~GrJustifier()
{
}

/*----------------------------------------------------------------------------------------------
	Determine how to adjust the widths of the glyphs to get a justified effect.
	Return kresFalse if we can't achieve the desired width.
----------------------------------------------------------------------------------------------*/
gr::GrResult GrJustifier::adjustGlyphWidths(gr::GraphiteProcess * pgje,
	int iGlyphMin, int iGlyphLim,
	float dxCurrentWidthArg, float dxDesiredWidthArg)
{
	if (dxCurrentWidthArg == dxDesiredWidthArg)
		return gr::kresOk; // no stretch needed

	int dxCurrentWidth = (int)dxCurrentWidthArg;
	int dxDesiredWidth = (int)dxDesiredWidthArg;

	bool fShrinking = (dxDesiredWidth < dxCurrentWidth);

	// First, get the relevant values for each glyph out of the Graphite engine.

	int dxsStretchAvail = 0;
	std::vector<int> viGlyphs;
	std::vector<int> vdxStretchLeft;
	std::vector<int> vdxStep;
	std::vector<int> vnWeight;
	std::vector<int> vdxWidth;
	std::vector<int> vdxStretchOrig;
	bool fStep = false;
	int nMaxWt = 1;
	int cUnits = 0; // glyph-weight units
	int cStretchable = 0;
	for (int iGlyph = iGlyphMin; iGlyph < iGlyphLim; iGlyph++)
	{
		int dx;
		pgje->getGlyphAttribute(iGlyph,
			(fShrinking ? gr::kjgatShrink : gr::kjgatStretch), 1, &dx);
		if (dx > 0)
		{
			int dxStep = 0;
			pgje->getGlyphAttribute(iGlyph, gr::kjgatStep, 1, &dxStep);
			if (fShrinking)
				dxStep = (dxStep > 0) ? 0 : dxStep; // step is applicable if it is negative
			else // stretching
				dxStep = (dxStep < 0) ? 0 : dxStep; // step is applicable if it is positive
			if (dxStep != 0)
			{
				// Get the actual number of steps allowed. This is more accurate than
				// trying to calculate it, due to rounding when converting between
				// font units and pixels.
				int cSteps;
				pgje->getGlyphAttribute(iGlyph, gr::kjgatStretchInSteps, 1, &cSteps);
				dx = abs(dxStep * cSteps);
				fStep = true;
			}
			dxStep = abs(dxStep);

			int nWt;
			pgje->getGlyphAttribute(iGlyph, gr::kjgatWeight, 1, &nWt);
			nWt = max(nWt, 0);
			nMaxWt = max(nMaxWt, nWt);

			viGlyphs.push_back(iGlyph);
			vdxStretchLeft.push_back(dx);
			vdxStep.push_back(dxStep);
			vnWeight.push_back(nWt);
			vdxWidth.push_back(0);
			vdxStretchOrig.push_back(dx);

			dxsStretchAvail += dx;
			cUnits += nWt;
			cStretchable++;
		}
	}

	int dxStretchNeeded = dxDesiredWidth - dxCurrentWidth;
	if (fShrinking)
		dxStretchNeeded *= -1; // always a positive number
 	int dxStretchAchieved = 0;
	bool fIgnoreStepGlyphs = false;
	int iiGlyph;
	std::vector<int> vnMFactor;

	if (viGlyphs.size() > 0)
	{
	
		// The way weights are handled is the following: we calculate the least common multiple
		// of all the weights, and then scale each stretch value accordingly before distributing
		// widths. In other words, we put the stretch values into an alternate "common" scaled
		// system based on the LCM. "cUnits" represents the total number of stretch-units
		// available, where each glyph contributes a number of units equal to its weight.
		// To get into this scaled system, small-weight stretches are scaled by a large amount
		// and large-weight stretches are scaled by a small amount. After assigning the width,
		// we do the reverse scaling on that width. Since large-weight stretches are scaled
		// back by less, this results in more width being assigned to glyphs with a large weight.  
	
		int nLcm = 1;
		if (nMaxWt > 1)
			nLcm = Lcm(vnWeight, vnMFactor);
		else
		{
			vnMFactor.push_back(1); // weight 0 - bogus
			vnMFactor.push_back(1); // weight 1
		}
	
		// Loop over the glyphs until we have assigned all the available space. (If a small amount
		// is left over it will be distributed using a special method.)
	
LMainLoop:
	
		int dxStretchStillNeeded = dxStretchNeeded - dxStretchAchieved;
		int dxNonStepMore = 0;
		int dxNonStepLess = 0;
		while (cUnits > 0 && dxStretchStillNeeded >= cStretchable) 
										// && dxStretchStillNeeded * nLcm >= cUnits)
		{
			// This is the scaled stretch per glyph, that is, in the scaled system of the LCM.
			int dxwStretchPerGlyph = dxStretchStillNeeded * nLcm / cUnits;
	
			// Recalculate these for the next round:
			cUnits = 0;
			cStretchable = 0;
	
			for (iiGlyph = 0; iiGlyph < (signed)viGlyphs.size(); iiGlyph++)
			{
				if (vdxStep[iiGlyph] > 0 && fIgnoreStepGlyphs)
					continue; // leave step-glyphs as they are
	
				int nWt = vnWeight[iiGlyph];
				int dxwThis = vdxStretchLeft[iiGlyph] * vnMFactor[nWt]; // weighted stretch
				dxwThis = min(dxwStretchPerGlyph, dxwThis);
				int dxThis = dxwThis / vnMFactor[nWt];	// scale back to unweighted stretch
				vdxWidth[iiGlyph] += dxThis;
				dxStretchAchieved += dxThis;
				vdxStretchLeft[iiGlyph] -= dxThis;
				if (vdxStretchLeft[iiGlyph] > 0)
				{
					cUnits += nWt; // can do some more on the next round
					cStretchable++;
				}
	
				// Keep track of how much we could adjust in either direction
				// on the second round to handle steps.
				if (vdxStep[iiGlyph] == 0)
				{
					dxNonStepMore += vdxStretchOrig[iiGlyph] - vdxWidth[iiGlyph];
					dxNonStepLess += vdxWidth[iiGlyph];
				}
			}
			dxStretchStillNeeded = dxStretchNeeded - dxStretchAchieved;
		}
	
		Assert(dxStretchAchieved <= dxStretchNeeded);
	
		// Make adjustments so that the step values are honored.
	
		if (fStep // there are some step-glyphs
			&& !fIgnoreStepGlyphs) // and we didn't already do this
		{
			// First make some basic adjustments, alternating making more and fewer steps
			// and see how much that buys us.
			int dxAdjusted = 0;
			int cNonStepUnits = 0;
			int cStretchableNonStep = 0;
			bool fReloop = false;
			for (iiGlyph = 0; iiGlyph < (signed)viGlyphs.size(); iiGlyph++)
			{
				if (vdxStep[iiGlyph] > 1)
				{
					int dxRem = vdxWidth[iiGlyph] % vdxStep[iiGlyph];
					int dxFewer = vdxWidth[iiGlyph] - dxRem;	// round down
					int dxMore = dxFewer + vdxStep[iiGlyph];	// round up
					int dxAdd = dxMore - vdxWidth[iiGlyph];
					if (dxRem == 0) 
					{	// Step is okay; no adjustment needed.
					}
					else if (
						// this glyph has stretch available to make more steps:
						(dxMore <= vdxStretchOrig[iiGlyph])
						// and we need at least this much extra:
						&& (dxAdd + dxStretchAchieved <= dxStretchNeeded)
						// and we still have enough slack in the non-step-glyphs:
						&& (dxNonStepLess - dxAdjusted - dxAdd > 0)
						// and we don't have to adjust much to get to the next step
						// (we're 75% of the way there):
						&& ((dxRem > ((vdxStep[iiGlyph] * 3) << 2))
							// or this glyph has a high weight:
							|| (vnWeight[iiGlyph] > (nMaxWt >> 2))
							// or we've removed a fair amount already:
							|| (dxAdjusted < (dxAdd * -2))
							// or we don't have enough slack to remove more:
							|| (dxNonStepMore + dxAdjusted - dxRem < 0)))
					{
						// Use the next larger number of steps.
						vdxWidth[iiGlyph] += dxAdd;
						dxStretchAchieved += dxAdd;
						dxAdjusted += dxAdd;
						fReloop = true;
					}
					else
					{
						// Use the next smaller number of steps.
						vdxWidth[iiGlyph] -= dxRem;
						dxStretchAchieved -= dxRem;
						dxAdjusted -= dxRem;
						fReloop = true;
					}
				}
				else if (vdxStep[iiGlyph] == 0)
				{
					cNonStepUnits += vnWeight[iiGlyph];
					cStretchableNonStep++;
				}
			}
	
			if (cNonStepUnits < cUnits || cStretchableNonStep < cStretchable)
			{
				// Even if no step-glyphs need to be adjusted, there is a different number
				// of (non-step) glyphs to divide the space over. (The first time through
				// there may not have been enough space per glyph to run the main loop,
				// but now there may be.)
				fReloop = true;
			}
	
			// Any left over adjustments need to be made by adjusting the items with
			// step = 1 (ie, the glyphs that allow fine-grained adjustments).
			// Do the main loop again, but only adjust the non-step glyphs.
			if (fReloop)
			{
				cUnits = cNonStepUnits;
				cStretchable = cStretchableNonStep;
				fIgnoreStepGlyphs = true;
				goto LMainLoop;
			}
		}
	
		// Divide up any remainder that is due to rounding errors.
	
		int dxRemainder = dxStretchNeeded - dxStretchAchieved;
		if (0 < dxRemainder && dxRemainder < cStretchable)
		{
			if (cStretchable < (signed)viGlyphs.size() || fStep)
			{
				//	Make sub-lists using the glyphs that are still stretchable.
				std::vector<int> vdxStretchRem;
				std::vector<int> vdxWidthRem;
				std::vector<int> viiGlyphsRem;
				for (iiGlyph = 0; iiGlyph < (signed)viGlyphs.size(); iiGlyph++)
				{
					if (vdxStretchLeft[iiGlyph] > 0 && vdxStep[iiGlyph] == 0)
					{
						viiGlyphsRem.push_back(iiGlyph);
						vdxStretchRem.push_back(vdxStretchLeft[iiGlyph]);
						vdxWidthRem.push_back(vdxWidth[iiGlyph]);
					}
				}
				Assert(viiGlyphsRem.size() == size_t(cStretchable));
				DistributeRemainder(vdxWidthRem, vdxStretchRem, dxRemainder, 0, vdxWidthRem.size(),
					&dxStretchAchieved);
				for (int iiiGlyph = 0; iiiGlyph < cStretchable; iiiGlyph++)
				{
					int ivGlyph = viiGlyphsRem[iiiGlyph];
					vdxStretchLeft[ivGlyph] = vdxStretchRem[iiiGlyph];
					vdxWidth[ivGlyph] = vdxWidthRem[iiiGlyph];
				}
			}
			else
			{
				//	All glyphs are still stretchable.
				DistributeRemainder(vdxWidth, vdxStretchLeft, dxRemainder, 0, vdxWidth.size(),
					&dxStretchAchieved);
			}
		}
		// otherwise we assume left-over is cannot be handled
	
		// Assign the widths to the glyphs.
	
		for (iiGlyph = 0; iiGlyph < (signed)viGlyphs.size(); iiGlyph++)
		{
			int dxThis = vdxWidth[iiGlyph] * ((fShrinking) ? -1 : 1);
			if (vdxStep[iiGlyph] == 0)
				pgje->setGlyphAttribute(viGlyphs[iiGlyph], gr::kjgatWidth, 1, dxThis);
			else
			{
				// Set the actual number of steps allowed. This is more accurate than
				// setting the pixels and then converting to font em-units.
				Assert(int(dxThis) % vdxStep[iiGlyph] == 0); // width divides evenly into steps
				int cSteps = dxThis/vdxStep[iiGlyph];
				pgje->setGlyphAttribute(viGlyphs[iiGlyph], gr::kjgatWidthInSteps, 1, cSteps);
			}
		}
	}
//LLeave:

	if (dxStretchAchieved == dxStretchNeeded)
		return gr::kresOk;
	else
	{
#ifdef WIN32
		wchar_t rgchw[20];
		std::fill_n(rgchw, 20, L'\0');
		_itow(dxStretchNeeded - dxStretchAchieved, rgchw, 10);
		std::wstring strTmp(L"justification failed by ");
		strTmp += rgchw;
		strTmp += L" units (width needed = ";
		std::fill_n(rgchw, 10, L'\0');
		_itow(dxDesiredWidth, rgchw, 10);
		strTmp += rgchw;
		strTmp += L")\n";
		OutputDebugString(strTmp.c_str());
#else
		Assert(fprintf(stderr,
			   "justification failed by %d units (width needed = %d)\n",
			   dxStretchNeeded - dxStretchAchieved, dxDesiredWidth));
#endif
		return gr::kresFalse;
	}
}

/*----------------------------------------------------------------------------------------------
	Distribute the remainder of the width evenly over the stretchable glyphs.
----------------------------------------------------------------------------------------------*/
void GrJustifier::DistributeRemainder(std::vector<int> & vdxWidths, std::vector<int> & vdxStretch,
	int dx, int iiMin, int iiLim,
	int * pdxStretchAchieved)
{
	if (dx == 0)
		return;

	Assert(dx <= iiLim - iiMin);
	if (iiMin + 1 == iiLim)
	{
		int dxThis = min(dx, vdxStretch[iiMin]);
		Assert(dxThis == 1); // we're never adjusting by more than 1, and the glyph should be
							 // adjustable by that much
		vdxWidths[iiMin] += dxThis;
		vdxStretch[iiMin] -= dxThis;
		*pdxStretchAchieved += dxThis;
	}
	else
	{
		int iiMid = (iiLim + iiMin) / 2;
		int dxHalf1 = dx / 2;
		int dxHalf2 = dx - dxHalf1;
		DistributeRemainder(vdxWidths, vdxStretch, dxHalf1, iiMin, iiMid, pdxStretchAchieved);
		DistributeRemainder(vdxWidths, vdxStretch, dxHalf2, iiMid, iiLim, pdxStretchAchieved);
	}
}

/*----------------------------------------------------------------------------------------------
	Return the least common multiple of the given weights. Also return
	a std::vector of multiplicative factors for each weight.
----------------------------------------------------------------------------------------------*/
int GrJustifier::Lcm(std::vector<int> & vnWeights, std::vector<int> & vnMFactors)
{
	//	The basic algorithm is to factor each weight into primes, counting how many times
	//	each prime occurs in the factorization. The LCM is the multiple of the primes
	//	with each prime raised to maximum power encountered within the factorizations.
	//	Example: weights = [2, 4, 5, 10]
	//			 2 = 2^1
	//			 4 = 2^2
	//			 5 =       5^1
	//			10 = 2^1 * 5^1
	//	So the LCM = 2^2 * 5^1 = 20.

	std::vector<int> vnPowersForLcm;
	int inPrime;
	for (inPrime = 0; inPrime < g_cnPrimes; inPrime++)
		vnPowersForLcm.push_back(0);

	std::vector<int> vnPowersPerPrime;
	vnPowersPerPrime.resize(g_cnPrimes);
	int nWtMax = 1;
	for (int inWt = 0; inWt < (signed)vnWeights.size(); inWt++)
	{
		int inMax = PrimeFactors(vnWeights[inWt], vnPowersPerPrime);
		for (inPrime = 0; inPrime <= inMax; inPrime++)
			vnPowersForLcm[inPrime] = max(vnPowersForLcm[inPrime], vnPowersPerPrime[inPrime]);
		nWtMax = max(nWtMax, vnWeights[inWt]);
	}
	
	int nLcm = 1;
	for (inPrime = 0; inPrime < g_cnPrimes; inPrime++)
		nLcm = nLcm * NthPower(g_rgnPrimes[inPrime], vnPowersForLcm[inPrime]);

	//	For each weight, calculate the multiplicative factor. This is the value by which
	//	to multiply stretch values of this weight in order to get them properly proportioned.
	//	Note that weights that are not used will have bogus factors.
	vnMFactors.push_back(nLcm); // bogus, for weight 0
	for (int nWt = 1; nWt <= nWtMax; nWt++)
	{
		vnMFactors.push_back(nLcm / nWt);
	}
	return nLcm;
}

/*----------------------------------------------------------------------------------------------
	Return a std::vector indicating the prime factors of n. The values of the std::vector correspond
	to the primes in g_rgnPrimes: [2, 3, 5, 7, ...]; they are the powers to which each
	prime should be raised. For instance, 20 = 2^2 * 5^1, so the result would contain
	[2, 0, 1, 0, 0, ...].
	The returned int is index of the highest prime in the list that we found.
----------------------------------------------------------------------------------------------*/
int GrJustifier::PrimeFactors(int n, std::vector<int> & vnPowersPerPrime)
{
	// Short-cut for common cases:
	switch (n)
	{
	case 0:
	case 1:
		vnPowersPerPrime[0] = 0;
		return 0;
	case 2:
		vnPowersPerPrime[0] = 1;	// 2^1
		return 0;
	case 3:
		vnPowersPerPrime[0] = 0;
		vnPowersPerPrime[1] = 1;	// 3^1
		return 1;
	case 4:
		vnPowersPerPrime[0] = 2;	// 2^2
		return 0;
	case 5:
		vnPowersPerPrime[0] = 0;
		vnPowersPerPrime[1] = 0;
		vnPowersPerPrime[2] = 1;	// 5^1
		return 2;
	case 6:
		vnPowersPerPrime[0] = 1;	// 2^1
		vnPowersPerPrime[1] = 1;	// 3^1
		return 1;
	case 7:
		vnPowersPerPrime[0] = 0;
		vnPowersPerPrime[1] = 0;
		vnPowersPerPrime[2] = 0;
		vnPowersPerPrime[3] = 1;	// 7^1
		return 3;
	case 8:
		vnPowersPerPrime[0] = 3;	// 2^3
		return 0;
	case 9:
		vnPowersPerPrime[0] = 0;
		vnPowersPerPrime[1] = 2;	// 3^2
		return 1;
	case 10:
		vnPowersPerPrime[0] = 1;	// 2^1
		vnPowersPerPrime[1] = 0;
		vnPowersPerPrime[2] = 1;	// 5^1
		return 2;
	default:
		break;
	}

	// Otherwise use the general algorithm: suck out prime numbers one by one,
	// keeping track of how many we have of each.

	int inPrime;
	for (inPrime = 0; inPrime < g_cnPrimes; inPrime++)
		vnPowersPerPrime[inPrime] = 0;

	int nRem = n;
	for (inPrime = 0; inPrime < g_cnPrimes; inPrime++)
	{
		while (nRem % g_rgnPrimes[inPrime] == 0)
		{
			vnPowersPerPrime[inPrime] += 1;
			nRem = nRem / g_rgnPrimes[inPrime];
		}
		if (nRem == 1)
			break;
	}
	Assert(n > 255 || nRem == 1);
	return inPrime;
}

/*----------------------------------------------------------------------------------------------
	Return nX raised to the nY power.
----------------------------------------------------------------------------------------------*/
int GrJustifier::NthPower(int nX, int nY)
{
	int nRet = 1;
	for (int i = 0; i < nY; i++)
		nRet *= nX;
	return nRet;
}

/*----------------------------------------------------------------------------------------------
	Determine how much shrinking is possible for low-end justification.
----------------------------------------------------------------------------------------------*/
//GrResult GrJustifier::suggestShrinkAndBreak(GraphiteProcess * pgje,
//	int iGlyphMin, int iGlyphLim, int dxsWidth, LgLineBreak lbPref, LgLineBreak lbMax,
//	int * pdxShrink, LgLineBreak * plbToTry)
//{
//	*pdxShrink = 0;
//	*plbToTry = lbPref;
//	return kresOk;
//}

