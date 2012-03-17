/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: BinTree.h
Responsibility: Darrell Zook
Last reviewed: 9/8/99

Description:
	This file provides declarations of binary tree class(es).
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef BINTREE_H_INCLUDED
#define BINTREE_H_INCLUDED

/*----------------------------------------------------------------------------------------------
	This template class implements a balanced tree, which is a binary tree where the height of
		the two children of any node in the tree can only be different by +/- 1 or 0.
		
	The class of objects to be stored in the tree should be derived from BalTreeBase, because
		the constructor of BalTreeBase adds an object to the tree in sorted order without
		allocating any memory.

	Classes that derive from BalTreeBase must have a CompareKey member function that compares
		a given key with this->key to determine which should be first when sorting the tree.
		The declaration of CompareKey should be as follows:
			int CompareKey(const Key & key);
		and it should return:
			<0 if key is less than the key in the current node (this->key)
			 0 if key is equal to the key in the current node (this->key)
			>0 if key is greater than the key in the current node (this->key)

	CAUTION: Classes that use this template should never be dynamically instantiated (by using
		local variables or new/NewObj). This class is meant to be used only for global
		instances of a class. The reason for this restriction is that the tree is never
		destroyed, so using dynamic instantiations of objects would result in stale pointers
		in the tree.

	Hungarian: btb
----------------------------------------------------------------------------------------------*/

template<typename Obj, typename Key> class BalTreeBase
{
protected:
	// Pointers to the left and right subtrees.
	Obj * m_rgpobj[2];

	// The difference between the height of the right sub tree and the height of the left
	// sub tree. This should be -1, 0, or +1. Insert may temporarily set it to -2 or +2.
	short m_dht;

	// Which way we went from this node when inserting the new node.
	short m_ipobj;

	// Inserts this in the balanced tree.
	BalTreeBase(Obj ** ppobjRoot, const Key & key);

	// Returns a node with the given key from the tree rooted at pobjRoot.
	static Obj * Find(Obj * pobjRoot, const Key & key);

private:
	// Inserts a node in the tree rooted at *ppobjRoot, sorting it based on key.
	void Insert(Obj ** ppobjRoot, const Key & key);
};

#endif // !BINTREE_H_INCLUDED

