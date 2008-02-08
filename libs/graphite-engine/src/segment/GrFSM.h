/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrFSM.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    The GrFSM class, which is the mechanism that examines input in a glyph stream and
	determines which rule matches and should be run.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef FSM_INCLUDED
#define FSM_INCLUDED

//:End Ignore

namespace gr
{

class GrPass;

/*----------------------------------------------------------------------------------------------
	A machine class range consists of a range of contiguous glyph IDs that map to
	a single column in the FSM (a machine class). (Note that there might be more than one
	range mapping to a given column.)

	Keep in mind that these "classes" are completely different from the classes that
	are used for substitution. These are used only for matching by the FSM.

	Hungarian: mcr
----------------------------------------------------------------------------------------------*/
class GrFSMClassRange
{
	friend class GrFSM;

protected:
	gid16	m_chwFirst;	// first glyph ID in range
	gid16	m_chwLast;	// last glyph ID in range
	data16	m_col;		// column (machine class) to which this range maps
};

/*----------------------------------------------------------------------------------------------
	The finite state machine that is used to match sequences of glyphs and
	determine which rule to apply. There is one FSM per pass.

	The states in the FSM are grouped and ordered in the following way:
		- non-accepting states (no rule completely matched)
		- accepting non-final states (a rule matched, but a longer rule is possible)
		- final states (a rule matched and no longer rule is possible)
	In other words, we have:
		- transition,	  non-accepting
		- transition,	  accepting
		- non-transition, accepting
	The transition states have information about the next state to transition to;
	the accepting states have information about the rule(s) that matched.

	There are three main data structures that are part of the finite state machine:

	(1) the transition matrix: m_prgprgrowXitions. It contains a row for each transition
		state (non-accepting states plus accepting non-final states), and a column for each
		machine class. The cell values indicate the next state to transition to for the
		matched input. A positive number indicates that the next state is a non-accepting state;
		a negative value indicates an accepting state. Zero means there is no next state;
		no more matches are possible; machine has "jammed."

	(2) the matched-rule list: m_prgrulnMatched. This is a list of rule numbers,
		which are indices into the pass's action- and constraint-code arrays.
		It contains the numbers of the rules matched by the first accepting state,
		followed by those matched by the second accepting state, etc. For each state,
		the rules must be ordered using the same order of the rules in the RDL file.

	(3) the matched-rule-offsets list: m_prgirulnMin. This gives the starting index into
		the matched-rule list for each accepting state. (Non-accepting states are not included,
		so the first item is for state m_crowNonAcpt.)

	In addition, the FSM interacts with the action- and constraint-code lists in the pass
	itself. These lists are indexed by rule number (the values of m_prgrulnMatched).
	When some input matches the rule, the constraint-code is run; if it succeeds,
	the rule number is returned to the caller and that rule is applied (ie, the action-code
	is run).

	Hungarian: fsm

	Other hungarian:
		row - row (state)
		col	- column (machine class)
		ruln - rule number

	REVIEW: This is quite a time-critical class, and there are two potential optimizations
	that have been proposed for the value of a cell:

	(1) use a negative cell number to indicate an accepting state and a positive number
		to indicate a non-accepting state. So this gives a comparison with zero rather than
		some arbitrary number

	(2)	have the positive numbers--the most common case--be the actual byte offset into the
		table rather than the row number. This saves matrix multiplication at each step,
		ie, "m_prgsnTransitions[(row * m_ccol) + col]".

	The two versions are implemented in RunTransitionTable and RunTransitionTableOptimized.
	Do these seem to be worth the inconvenience of added complexity	in understanding
	and debugging?
----------------------------------------------------------------------------------------------*/
class GrFSM
{
	friend class FontMemoryUsage;

public:
	GrFSM() :
		m_prgirulnMin(NULL),
		m_prgrulnMatched(NULL),
		m_prgrowTransitions(NULL),
		m_prgibStateDebug(NULL),
		m_prgmcr(NULL),
		m_prgrowStartStates(NULL)
	{
	}

	~GrFSM()
	{
		delete[] m_prgirulnMin;
		delete[] m_prgrulnMatched;

		delete[] m_prgrowTransitions;

		delete[] m_prgibStateDebug;

		delete[] m_prgmcr;

		delete[] m_prgrowStartStates;
	}

	bool ReadFromFont(GrIStream & grstrm, int fxdVersion);
	bool ReadStateTableFromFont(GrIStream & grstrm, int fxdVersion);

	int GetRuleToApply(GrTableManager *, GrPass * ppass,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);

	int RunTransitionTable(GrPass * ppass, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
		int * prgrowAccepting, int * prgcslotMatched);
	int RunTransitionTableOptimized(GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
		int * prgrowAccepting, int * prgcslotMatched);
	bool RunConstraintAndRecordLog(GrTableManager *, GrPass * ppass, int ruln,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
		int cslotPreModContext, int cslotMatched);

	int RunConstraints_Obsolete(GrTableManager *, GrPass * ppass, int row, GrSlotStream * psstrmIn,
		int cslotMatched);

	//	For sorting matched rules
	struct MatchedRule // mr
	{
		int	ruln;
		int nSortKey;
		int cslot;	// number of slots matched AFTER the current stream position
	};

	int MaxRulePreContext()
	{
		return m_critMaxRulePreContext;
	}

protected:
	int FindColumn(gid16 chwGlyphID);
protected:
	//	Instance variables:
	int				m_crow;			// number of rows (states)
	int				m_crowFinal;	// number of final states; no transitions for these
	int				m_rowFinalMin;	// index of first final row
	int				m_crowNonAcpt;	// number of non-accepting states; no rule indices for these

	int				m_ccol;			// number of columns (machine classes)

	data16 *		m_prgirulnMin;		// m_crow-m_crowNonAcpt+1 of these;
										// index within m_prgrulnMatched, start of matched
										// rules for each accepting state

	data16 *		m_prgrulnMatched;	// long ordered list of rule indices matched by
										// subsequent states; total length is sum of number
										// of rules matched for each accepting state
	int m_crulnMatched; // needed only for memory instrumentation

	//	Transition matrix--for optimized version:
//	short **		m_prgprgrowXitions;	// ((m_crow-m_crowFinal) * m_ccol) of these;
										// positive number indicates
										// next state is non-accepting; negative number is
										// negative of accepting state.

	//	Transition matrix--for current version:
	short *			m_prgrowTransitions; // ((m_crow-m_crowFinal) * m_ccol) of these

	//	debugger string offsets
	data16 *		m_prgibStateDebug;	// for transition states; (m_crow-m_crul+1) of these

	//	constants for fast binary search; these are generated by the compiler so that the
	//	engine doesn't have to take time to do it
	data16			m_dimcrInit;		// (max power of 2 <= m_cmcr);
										//		size of initial range to consider
	data16			m_cLoop;			// log2(max power of 2 <= m_cmcr);
										//		indicates how many iterations are necessary
	data16			m_imcrStart;		// m_cmcr - m_dimcrInit;
										//		where to start search

	int				m_cmcr;				// number of machine-class-ranges
	GrFSMClassRange * m_prgmcr;			// array of class ranges; we search these to find the
										// mapping to the machine-class-column

	//	minimum and maximum number of items in the rule contexts before the first modified
	//	item.
	int				m_critMinRulePreContext;
	int				m_critMaxRulePreContext;

	//	start states--row in the FSM to start on depending on how many bogus slots we
	//	are skipping; (max rule-precontext - min rule-precontext + 1) of these;
	//	first always = zero
	short *			m_prgrowStartStates;

//:Ignore
#if OLD_TEST_STUFF
public:
	//	For test procedures:
	void SetUpSimpleFSMTest();
	void SetUpRuleActionTest();
	void SetUpRuleAction2Test(int);
	void SetUpAssocTest(int);
	void SetUpAssoc2Test(int);
	void SetUpDefaultAssocTest();
	void SetUpFeatureTest();
	void SetUpLigatureTest(int);
	void SetUpLigature2Test(int);
#endif // OLD_TEST_STUFF
//:End Ignore

};

} // namespace gr


#endif // !FSM_INCLUDED
