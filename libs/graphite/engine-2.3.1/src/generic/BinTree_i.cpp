/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: BinTree_i.cpp
Responsibility: Darrell Zook
Last reviewed: 9/8/99

Description:
	This file provides implementations of binary tree class(es).
	It is used as an #include file in any file which derives from one of the classes.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef BINTREE_I_CPP_INCLUDED
#define BINTREE_I_CPP_INCLUDED

#include "BinTree.h"
////#include <algorithm>
////#include "GrPlatform.h"
#include "GrCommon.h"


//using namespace gr;

/***********************************************************************************************
	BalTreeBase<Obj, Key> methods
***********************************************************************************************/

/*----------------------------------------------------------------------------------------------
	Constructor.
	This calls Insert to add this node to the tree in the correct (sorted) position.

	Arguments:
		[in] ppobjRoot -> The address of the pointer to the root node of the tree.
		[in] key -> References the key of the node to insert.
----------------------------------------------------------------------------------------------*/
template<typename Obj, typename Key>
	BalTreeBase<Obj, Key>::BalTreeBase(Obj ** ppobjRoot, const Key & key)
{
	AssertPtr(ppobjRoot);
	AssertPtrN(*ppobjRoot);

	m_rgpobj[0] = NULL;
	m_rgpobj[1] = NULL;
	m_dht = 0;

	Insert(ppobjRoot, key);
}

/*----------------------------------------------------------------------------------------------
	This method finds a node with the given key within the tree. If the key is not found in
		the tree, NULL is returned.

	Arguments:
		[in] pobjRoot -> Points to the root node of the tree.
		[in] key -> References the key of the node to search for.
----------------------------------------------------------------------------------------------*/
template<typename Obj, typename Key>
	Obj * BalTreeBase<Obj, Key>::Find(Obj * pobjRoot, const Key & key)
{
	AssertPtrN(pobjRoot);

	for (Obj * pobj = pobjRoot; pobj; )
	{
		int nT = pobj->CompareKey(key);
		if (0 == nT)
			return pobj;
		pobj = pobj->m_rgpobj[nT > 0];
	}
	return NULL;
}

/*----------------------------------------------------------------------------------------------
	This method inserts a node within the tree, sorting it based on key. It does not allocate
		any memory.

	Arguments:
		[in] ppobjRoot -> The address of the pointer to the root node of the tree.
		[in] key -> References the key of the node to insert into the tree.
		Note: "this" is the node that is being inserted into the tree.
----------------------------------------------------------------------------------------------*/
template<typename Obj, typename Key>
	void BalTreeBase<Obj, Key>::Insert(Obj ** ppobjRoot, const Key & key)
{
	AssertPtr(ppobjRoot);
	AssertPtrN(*ppobjRoot);

	Assert(m_dht == 0);
	Assert(!m_rgpobj[0]);
	Assert(!m_rgpobj[1]);

	Obj * pobjThis = static_cast<Obj *>(this);

	// If the tree is empty, set the root of the tree to this and return.
	if (!*ppobjRoot)
	{
		*ppobjRoot = pobjThis;
		return;
	}

	Obj ** ppobjRebal = ppobjRoot;
	Obj * pobjCur;

	// Place this in the tree. Mark each node indicating which branch we took.
	// Remember the lowest node with non-zero m_dht in ppobjRebal. If we have to do
	// rebalancing, this is where we'll do it.
	for (pobjCur = *ppobjRoot; ; )
	{
		AssertPtr(pobjCur);

		int nT = pobjCur->CompareKey(key);
		Assert(nT != 0);

		// if nT is positive go right, otherwise go left.
		pobjCur->m_ipobj = nT > 0;

		Obj ** ppobjNext = &pobjCur->m_rgpobj[pobjCur->m_ipobj];
		if (!*ppobjNext)
		{
			// Place the node and break.
			*ppobjNext = pobjThis;
			break;
		}

		// Remember this location if m_dht is non-zero.
		if ((*ppobjNext)->m_dht != 0)
			ppobjRebal = ppobjNext;

		// Move on down.
		pobjCur = *ppobjNext;
	}

	// Adjust m_dht from *ppobjRebal to pobjThis.
	for (pobjCur = *ppobjRebal; pobjCur != pobjThis; )
	{
		AssertPtr(pobjCur);
		Assert((unsigned int)pobjCur->m_ipobj < (unsigned int)2);

		// Recall that ppobjRebal is the lowest node on the path which has non-zero m_dht.
		Assert(pobjCur->m_dht == 0 || pobjCur == *ppobjRebal);

		// If we went left (m_ipobj == 0) we need to subtract one. If we went right
		// (m_ipobj == 1) we need to add one.
		pobjCur->m_dht = (short)(pobjCur->m_dht + (pobjCur->m_ipobj << 1) - 1);
		pobjCur = pobjCur->m_rgpobj[pobjCur->m_ipobj];
	}

	if ((unsigned int)((*ppobjRebal)->m_dht + 1) <= (unsigned int)2)
	{
		// No need to rebalance since the new m_dht for *ppobjRebal is within -1 to 1.
		return;
	}

	// We need to rebalance.
	Obj * pobjRebal = *ppobjRebal;
	Assert(pobjRebal->m_dht == -2 || pobjRebal->m_dht == 2);

	// Get the direction from pobjRebal and the next lower node.
	int ipobj = pobjRebal->m_ipobj;
	Obj * pobjChd = pobjRebal->m_rgpobj[ipobj];

	if (pobjChd->m_ipobj == ipobj)
	{
		// We branched the same way at pobjRebal and pobjChd.
		// Do a single rotation: make pobjChd the parent of pobjRebal.
		Assert(pobjChd->m_dht << 1 == pobjRebal->m_dht);
		pobjRebal->m_rgpobj[ipobj] = pobjChd->m_rgpobj[1 - ipobj];
		pobjChd->m_rgpobj[1 - ipobj] = pobjRebal;
		*ppobjRebal = pobjChd;

		// Fix the m_dht values.
		pobjChd->m_dht = 0;
		pobjRebal->m_dht = 0;
		return;
	}

	// We branched opposite ways at pobjRebal and pobjChd.
	// Do a double rotation.
	Assert(pobjChd->m_ipobj == 1 - ipobj);
	pobjCur = pobjChd->m_rgpobj[1 - ipobj];
	AssertPtr(pobjCur);

	// Let (A own[ipobj] B) mean that A->m_rgpobj[ipobj] == B.
	// Currently, (pobjRebal own[ipobj] pobjChd) and (pobjChd own[1 - ipobj] pobjCur).
	// Make (pobjCur own[ipobj] pobjChd) and (pobjCur own[1 - ipobj] pobjRebal).

	// Rotate pobjChd and pobjCur.
	Assert(pobjCur == pobjChd->m_rgpobj[1 - ipobj]);
	pobjChd->m_rgpobj[1 - ipobj] = pobjCur->m_rgpobj[ipobj];
	pobjCur->m_rgpobj[ipobj] = pobjChd;

	// Rotate pobjRebal and pobjCur.
	Assert(pobjRebal->m_rgpobj[ipobj] == pobjChd);
	pobjRebal->m_rgpobj[ipobj] = pobjCur->m_rgpobj[1 - ipobj];
	pobjCur->m_rgpobj[1 - ipobj] = pobjRebal;

	// Fix *ppobjRebal.
	*ppobjRebal = pobjCur;

	// Fix the m_dht values. Note that we don't compare with pobjRebal->m_dht since it
	// currently has absolute value 2.
	Assert(pobjChd->m_dht);
	if (pobjCur->m_dht == pobjChd->m_dht)
	{
		pobjRebal->m_dht = 0;
		pobjChd->m_dht = (short)-pobjChd->m_dht;
	}
	else if (pobjCur->m_dht == -pobjChd->m_dht)
	{
		pobjRebal->m_dht = pobjChd->m_dht;
		pobjChd->m_dht = 0;
	}
	else
	{
		pobjRebal->m_dht = 0;
		pobjChd->m_dht = 0;
	}
	pobjCur->m_dht = 0;
}

#endif // !BINTREE_I_CPP_INCLUDED
