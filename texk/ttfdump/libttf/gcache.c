/*
 * gcache.c -- Provide the cache mechanicsm for outlines
 * Copyright (C) 1996 Li-Da Lho, All right reserved
 *
 * This file is a little bit more complicated, the structure of the cache is 
 * described as fellow:
 * Basically the cache can be divided onto two parts, the first one is a 
 * linear part which is a least recently utilitied list (LRU), 
 * the second part is the nonlinear one which is a binary serach tree. 
 * Normally each cache element is on the LRU as well as on the BST.
 * Those glyph elements are allocated at once as an array of glyph cache 
 * elements at the init phase of glyph cache.
 *
 * font->gcache--
 *              |
 *      ________|
 *     |
 *     v 
 * ------------------------------------------------------------------
 * |element 0|   1   |   2   |  3   |   4   |   5   |   6   |   7   | ...
 * ------------------------------------------------------------------ 
 * The first element of the glyph cache array serves as a dummy header
 * for two purposes, one is for the head and tail of the LRU, the other is
 * to point to the root of the BST. As a pointer to the root of the
 * BST, the right subtree points to the root of the BST while the left one is 
 * not used and arbitrarly points to itself.
 *
 * After initialization, all the glyph cache element are on the LRU but
 * none of them is on the BST.
 *                                     
 *  |-----------------------------------------------------------|
 *  |   -------------         ------                  ------    |
 *  |-> |head & tail| <-----> |    |<-----> ...<----->|    | <--|
 *      -------------         ------                  ------ 
 *         /  \                /  \                    /  \
 *        /    \              /    \                  /    \
 * left=self right=root    left  right             left  right
 *
 * The cached data are pointed by the entries of the glyph cache element
 * in a strait forward fashion.
 *
 *  |-----------------------------------------------------------|
 *  |   -------------         ------                  ------    |
 *  |-> |head & tail| <-----> |    |<-----> ...<----->|    | <--|
 *      -------------         ------                  ------ 
 *       |  |     -------------|--|-------------------------------- 
 *       |  |---->|            |  |->  array for flag, instruction|
 *       |        -------------|----------------------------------- 
 *       |        -------------|-----------------------------------
 *       |------->|            |---->  xcoordinates,  ycoordinates|
 *                -------------------------------------------------
 * Implementation in this way is effecient for the reason that when we
 * need to free the cache, we only need to do the two things
 * 1. free the data pointed by the entries in the dummy header
 * 2. free the dummy header
 *
 * Every time a glyph outline is required and ttfLoadGlyphCached is called, 
 * the glyph is searched on the BST with its offset as a key.
 * If the glyph is on the BST then the address of the glyph cache is returned
 * and the glyph cache is moved to the tail of the LRU such that the order of 
 * LRU is mantained.
 * When the glyph required is not on the BST, something more complicated should
 * be done.
 *  1. Remove the first element on the LRU from the BST.
 *  2. Load the glyph data to the cache element.
 *  3. Insert the element back to the BST.
 *  4. Move the element to the tail of the LRU.
 *
 * The only problem remain is how to deal with composite components. 
 * The asnwer simply is not to support cache for composite components at the 
 * current stage for the two reasons:
 *  1. It will makes the code an order of magnitude more complicate to support
 *     composite glyf cache.
 *  2. The space occupied by composite glyf is not very much
 *
 * It is not clear right now if it is wise to add tree balancing code. 
 * The overhaed introduced by BST seems minimal for 256 elements cache.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: gcache.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfInitCache(TTFontPtr font);
static void ttfFreeCache(TTFontPtr font);
static void ttfRotateCache(TTFontPtr font);
static void ttfAllocCacheData(TTFontPtr font);
static void ttfInitCacheData(TTFontPtr font);
static void ttfFreeCacheData(TTFontPtr font);
static void ttfInsertBST(TTFontPtr font,GlyphCachePtr gcache);
static GlyphCachePtr ttfSearchBST(TTFontPtr font,ULONG offset);
static void ttfDeleteBST(TTFontPtr font,ULONG offset);
#ifdef MERGEDELETE
static void merge(GlyphCachePtr *root);
#endif
static void copy(GlyphCachePtr *root);

void ttfInitGlyphCache(TTFontPtr font)
{
    /* this should be made configurable */
    if (font->maxp->numGlyphs > 256)
	font->numCacheElements = 128;
    else
	font->numCacheElements = 64;

    font->gcache = XCALLOC (font->numCacheElements+1, GlyphCache);
    ttfInitCache(font);

    ttfAllocCacheData(font);
    ttfInitCacheData(font);
}

void ttfCleanUpGlyphCache(TTFontPtr font)
{
    ttfFreeCacheData(font);
    ttfFreeCache(font);    
}

/* provide cache mechanism for glyph */
GLYFPtr ttfLoadGlyphCached(TTFontPtr font,ULONG offset)
{
#ifdef OLDCODE
    GLYFPtr glyf;
    
    glyf = &(font->gcache->next->glyf);
    font->gcache->next->offset = offset;
    ttfRotateCache(font);
    ttfLoadGLYF(font->fp,glyf,offset);   
    return glyf;

#else
    GlyphCachePtr gcache;

    if ((gcache = ttfSearchBST(font,offset)) == NULL)
	{
	    /* if the glyph is not in the cache */
	    ttfDeleteBST(font,font->gcache->next->offset);
	    gcache = font->gcache->next;
	    font->gcache->next->offset = offset;
	    ttfRotateCache(font);
	    ttfLoadGLYF(font->fp,&gcache->glyf,offset);
	    ttfInsertBST(font,gcache);
	}
    return &gcache->glyf;
#endif
}

/* threading glyph cache headers */
static void ttfInitCache(TTFontPtr font)
{
    USHORT i,numCache = font->numCacheElements;
    GlyphCachePtr cur,tmp;

    cur = tmp = font->gcache;    
    tmp++;
    /* there are numGlyph+1 of cache header, the first one is for
     * mark as the head and tail of the queue */
    for (i=0;i<numCache;i++)
	{
	    cur->right = cur->left = NULL;
	    cur->next = tmp;
	    tmp->prev = cur;
	    cur = tmp;
	    tmp++;
	}

    /* make the LRU circular */
    cur->next = font->gcache;
    font->gcache->prev = cur;

    /* make the dummy header point to it self on the BST */
    font->gcache->left = font->gcache;
    font->gcache->offset = 0;
}
static void ttfFreeCache(TTFontPtr font)
{
    free(font->gcache);
}
/* move the first element on the LRU the the tail */
static void ttfRotateCache(TTFontPtr font)
{
    GlyphCachePtr gcache = font->gcache, tmp = font->gcache->next;

    /* remove the first element */
    gcache->next = tmp->next;
    tmp->next->prev = gcache;
    /* add the element to the tail */
    tmp->next = gcache;
    tmp->prev = gcache->prev;
    gcache->prev->next = tmp;
    gcache->prev = tmp;
}
static void ttfAllocCacheData(TTFontPtr font)
{
    USHORT numCache = font->numCacheElements+1;
    USHORT maxPoints = font->maxp->maxPoints;
    USHORT maxContours = font->maxp->maxContours;
    USHORT insLength = font->maxp->maxSizeOfInstructions;
      
    font->gcache->glyf.endPtsOfContours = XCALLOC (numCache*maxContours, USHORT);
    font->gcache->glyf.instructions = XCALLOC (numCache*insLength, BYTE);
    font->gcache->glyf.flags = XCALLOC (numCache*maxPoints, BYTE);
    font->gcache->glyf.xCoordinates = XCALLOC (numCache*maxPoints, SHORT);
    font->gcache->glyf.yCoordinates = XCALLOC (numCache*maxPoints, SHORT);
    font->gcache->glyf.comp = NULL;
}
static void ttfInitCacheData(TTFontPtr font)
{
    USHORT i;
    USHORT numCache = font->numCacheElements;
    USHORT maxPoints = font->maxp->maxPoints;
    USHORT maxContours = font->maxp->maxContours;
    USHORT insLength = font->maxp->maxSizeOfInstructions;
    GlyphCachePtr cur,tmp;
   
    tmp = font->gcache;
    cur = font->gcache->next;

    for (i=0;i<numCache;i++)
	{
	    /* makes the pointer in the cache element point to correct
	     * addresses in the data area */
	    cur->glyf.endPtsOfContours = tmp->glyf.endPtsOfContours + maxContours;
	    cur->glyf.instructions = tmp->glyf.instructions + insLength;
	    cur->glyf.flags = tmp->glyf.flags + maxPoints;
	    cur->glyf.xCoordinates = tmp->glyf.xCoordinates + maxPoints;
	    cur->glyf.yCoordinates = tmp->glyf.yCoordinates + maxPoints;
	    tmp = cur++;
	}
}
static void ttfFreeCacheData(TTFontPtr font)
{
    USHORT i,numCache = font->numCacheElements;
    GlyphCachePtr cur = font->gcache+1;

    /* free components of composite glyfs */
    for (i=0;i<numCache;i++)
	{
	    ttfFreeGLYF(&cur->glyf);
	    cur++;
	}
    free(font->gcache->glyf.endPtsOfContours);
    free(font->gcache->glyf.instructions);
    free(font->gcache->glyf.flags);
    free(font->gcache->glyf.xCoordinates);
    free(font->gcache->glyf.yCoordinates);
}

/* code to deal with nonlinear part of glyph cache */
static void ttfInsertBST(TTFontPtr font,GlyphCachePtr gcache)
{
    GlyphCachePtr root,parent;
    
    parent = font->gcache;
    root = parent->right;

    while (root) 
	{
	    parent = root;
	    if (root->offset > gcache->offset)
		root = root->left;
	    else
		root = root->right;
	}

    if (parent->offset > gcache->offset)
	parent->left = gcache;
    else
	parent->right = gcache;
}
static GlyphCachePtr ttfSearchBST(TTFontPtr font,ULONG offset)
{
    GlyphCachePtr root = font->gcache->right;

    while (root)
	{
	    if (root->offset == offset)
		return root;
	    if (root->offset > offset)
		root = root->left;
	    else
		root = root->right;
	}
    return NULL;    
}
/* Deleting a node form a BST is a little bit tricky.
 * Basically, there are three different situations for deleteing a node
 *   1. The node to delete is a leaf, then just delete the node and make the
 *      node's parent point to null.
 *   2. The node has one subtree, the let the node's parent adopt it.
 *   3. The node has both subtrees, in this situation, there are two 
 *      alternatives
 *     a. let the parent adopt one of the subtree and the subtree adopt the 
 *        other. (delete by merge)
 *     b. replace the node with its precedeer, and leave the topology almost
 *        unchanged.
 */
static void ttfDeleteBST(TTFontPtr font,ULONG offset)
{
    GlyphCachePtr root,parent;

    parent = font->gcache;
    root = parent->right;

    /* find the node to delete */
    while (root) 
	{
	    if (root->offset == offset)
		break;
	    parent = root;
	    if (root->offset > offset)
		root = root->left;
	    else
		root = root->right;
	}
    if (root != NULL)
	{
	    /* the node to be deleted is on the tree */
	    if (root->glyf.numberOfContours < 0)
		/* a composite glyf with components */
		ttfFreeGLYF(&root->glyf);

#ifdef MERGEDELETE
	    if (parent == font->gcache)
		/* delete root */
		merge(&parent->right);
	    else if (parent->left == root)
		merge(&parent->left);
	    else 
		merge(&parent->right);
#else
	    if (parent == font->gcache)
		/* delete root */
		copy(&parent->right);
	    else if (parent->left == root)
		copy(&parent->left);
	    else 
		copy(&parent->right);
#endif

	    root->left = root->right = NULL;
	}
    else
	; /* the tree is empty or the node is not on the tree */
    
}

#ifdef MERGEDELETE
static void merge(GlyphCachePtr *root)
{
    GlyphCachePtr tmp;
    
    if ((*root)->right == NULL)
	*root = (*root)->left;
    else if ((*root)->left == NULL)
	*root = (*root)->right;
    else
	{
	    /* the node to be attached has both left and right
	     * subtrees */
	    tmp = (*root)->left;
	    while (tmp->right)
		{
		    tmp = tmp->right;
		}
	    tmp->right = (*root)->right;
	    *root = (*root)->left;
	}
}
#endif
static void copy(GlyphCachePtr *root)
{
    GlyphCachePtr tmp, parent;
    
    if ((*root)->right == NULL)
	*root = (*root)->left;
    else if ((*root)->left == NULL)
	*root = (*root)->right;
    else
	{
	    tmp = (*root)->left;
	    parent = *root;
	    while (tmp->right)
		{
		    parent = tmp;
		    tmp = tmp->right;
		}
	    tmp->right = (*root)->right;
	    if (parent != *root)
		{
		    parent->right = tmp->left;
		    tmp->left = parent;
		}
	    *root = tmp;
	}
}
