/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: HashMap.h
Responsibility: Steve McConnel
Last reviewed: Not yet.

Description:
	This provides a set of template collection classes to replace std::map.  Their primary
	reason to exist is to allow explicit checking for internal memory allocation failures.
----------------------------------------------------------------------------------------------*/
#pragma once
#ifndef HASHMAP_H_INCLUDED
#define HASHMAP_H_INCLUDED
//:End Ignore

/*----------------------------------------------------------------------------------------------
	Functor class for computing a hash value from an arbitrary object.

	Hungarian: hsho
----------------------------------------------------------------------------------------------*/
class HashObj
{
public:
	int operator () (void * pKey, int cbKey);
};

/*----------------------------------------------------------------------------------------------
	Functor class for comparing two arbitrary objects (of the same class) for equality.

	Hungarian: eqlo
----------------------------------------------------------------------------------------------*/
class EqlObj
{
public:
	bool operator () (void * pKey1, void * pKey2, int cbKey);
};

/*----------------------------------------------------------------------------------------------
	Hash map template collection class whose keys are objects of an arbitrary class.

	Hungarian: hm[K][T]
----------------------------------------------------------------------------------------------*/
template<class K, class T, class H = HashObj, class Eq = EqlObj> class HashMap
{
public:
	//:> Member classes

	/*------------------------------------------------------------------------------------------
		This is the basic data structure for storing one key-value pair in a hash map.  In
		order to handle hash collisions, this structure is a member of a linked list.
		Hungarian: hsnd
	------------------------------------------------------------------------------------------*/
	class HashNode
	{
	public:
		//:> Constructors/destructors/etc.

		HashNode(void)
			: m_key(K()), m_value(T()), m_nHash(0), m_ihsndNext(0)
		{
		}
		HashNode(K & key, T & value, int nHash, int ihsndNext = -1)
			: m_key(key), m_value(value), m_nHash(nHash), m_ihsndNext(ihsndNext)
		{
		}
		~HashNode()
		{
		}

		//:> Member variable access

		void PutKey(K & key)
		{
			m_key = key;
		}
		K & GetKey()
		{
			return m_key;
		}
		void PutValue(T & value)
		{
			m_value = value;
		}
		T & GetValue()
		{
			return m_value;
		}
		void PutHash(int nHash)
		{
			m_nHash = nHash;
		}
		int GetHash()
		{
			return m_nHash;
		}
		void PutNext(int ihsndNext)
		{
			m_ihsndNext = ihsndNext;
		}
		int GetNext()
		{
			return m_ihsndNext;
		}

		/*--------------------------------------------------------------------------------------
			Check whether the given HashNode is being used.
		--------------------------------------------------------------------------------------*/
		bool InUse()
		{
			return m_ihsndNext >= -1;
		}

	protected:
		//:> Member variables

		K m_key;
		T m_value;
		int	m_nHash;
		int	m_ihsndNext;	// -1 means end of list, -(ihsnd + 3) for free list members
	};

	/*------------------------------------------------------------------------------------------
		This provides an iterator for stepping through all HashNodes stored in the hash map.
		This is useful primarily for saving the contents of a hash map to a file.

		Hungarian: ithm[K][T]
	------------------------------------------------------------------------------------------*/
	class iterator
	{
	public:
		// Constructors/destructors/etc.

		iterator() : m_phmParent(NULL), m_ihsnd(0)
		{
		}
		iterator(HashMap<K,T,H,Eq> * phm, int ihsnd) : m_phmParent(phm), m_ihsnd(ihsnd)
		{
		}
		iterator(const iterator & v) : m_phmParent(v.m_phmParent), m_ihsnd(v.m_ihsnd)
		{
		}
		~iterator()
		{
		}

		// Other public methods

		iterator & operator = (const iterator & ithm)
		{
			m_phmParent = ithm.m_phmParent;
			m_ihsnd = ithm.m_ihsnd;
			return *this;
		}
		T & operator * (void)
		{
			Assert(m_phmParent);
			Assert(m_phmParent->m_prghsnd);
			Assert(m_ihsnd < m_phmParent->m_ihsndLim);
			return m_phmParent->m_prghsnd[m_ihsnd].GetValue();
		}
		HashNode * operator -> (void)
		{
			Assert(m_phmParent);
			Assert(m_phmParent->m_prghsnd);
			Assert(m_ihsnd < m_phmParent->m_ihsndLim);
			return &m_phmParent->m_prghsnd[m_ihsnd];
		}
		iterator & operator ++ (void)
		{
			Assert(m_phmParent);
			++m_ihsnd;
			// make sure that this new HashNode is actually in use
			while (m_ihsnd < m_phmParent->m_ihsndLim)
			{
				if (m_phmParent->m_prghsnd[m_ihsnd].InUse())
					return *this;
				// skip to the next one and check it
				++m_ihsnd;
			}
			if (m_ihsnd > m_phmParent->m_ihsndLim)
				m_ihsnd = m_phmParent->m_ihsndLim;
			return *this;
		}
		bool operator == (const iterator & ithm)
		{
			return (m_phmParent == ithm.m_phmParent) && (m_ihsnd == ithm.m_ihsnd);
		}
		bool operator != (const iterator & ithm)
		{
			return (m_phmParent != ithm.m_phmParent) || (m_ihsnd != ithm.m_ihsnd);
		}
		T & GetValue(void)
		{
			Assert(m_phmParent);
			Assert(m_phmParent->m_prghsnd);
			Assert(m_ihsnd < m_phmParent->m_ihsndLim);
			Assert(m_phmParent->m_prghsnd[m_ihsnd].InUse());
			return m_phmParent->m_prghsnd[m_ihsnd].GetValue();
		}
		K & GetKey(void)
		{
			Assert(m_phmParent);
			Assert(m_phmParent->m_prghsnd);
			Assert(m_ihsnd < m_phmParent->m_ihsndLim);
			Assert(m_phmParent->m_prghsnd[m_ihsnd].InUse());
			return m_phmParent->m_prghsnd[m_ihsnd].GetKey();
		}
		int GetHash()
		{
			Assert(m_phmParent);
			Assert(m_phmParent->m_prghsnd);
			Assert(m_ihsnd < m_phmParent->m_ihsndLim);
			Assert(m_phmParent->m_prghsnd[m_ihsnd].InUse());
			return m_phmParent->m_prghsnd[m_ihsnd].GetHash();
		}
		int GetIndex()
		{
			Assert(m_phmParent);
			Assert(m_phmParent->m_prghsnd);
			Assert(m_ihsnd < m_phmParent->m_ihsndLim);
			Assert(m_phmParent->m_prghsnd[m_ihsnd].InUse());
			return m_ihsnd;
		}

	protected:
		//:> Member variables

		HashMap<K,T,H,Eq> * m_phmParent;
		int m_ihsnd;
	};
	friend class iterator;

	//:> Constructors/destructors/etc.

	HashMap();
	~HashMap();
	HashMap(HashMap<K,T,H,Eq> & hm);

	//:> Other public methods

	iterator Begin();
	iterator End();
	void Insert(K & key, T & value, bool fOverwrite = false, int * pihsndOut = NULL);
	bool Retrieve(K & key, T * pvalueRet);
	bool Delete(K & key);
	void Clear();
	void CopyTo(HashMap<K,T,H,Eq> & hmKT);
	void CopyTo(HashMap<K,T,H,Eq> * phmKT);

	bool GetIndex(K & key, int * pihsndRet);
	bool IndexKey(int ihsnd, K * pkeyRet);
	bool IndexValue(int ihsnd, T * pvalueRet);

	int Size();

	/*------------------------------------------------------------------------------------------
		The assignment operator allows an entire hashmap to be assigned as the value of another
		hashmap.  It throws an error if it runs out of memory.

		@return a reference to this hashmap.  (That is how the assignment operator is defined!)

		@param hm is a reference to the other hashmap.
	------------------------------------------------------------------------------------------*/
	HashMap<K,T,H,Eq> & operator = (HashMap<K,T,H,Eq> & hm)
	{
		hm.CopyTo(this);
		return *this;
	}

	//:Ignore
#ifdef DEBUG
	int _BucketCount();
	int _EmptyBuckets();
	int _BucketsUsed();
	int _FullestBucket();
	bool AssertValid()
	{
		AssertPtrN(m_prgihsndBuckets);
		Assert(m_prgihsndBuckets || !m_cBuckets);
		Assert(!m_prgihsndBuckets || m_cBuckets);
		AssertArray(m_prgihsndBuckets, m_cBuckets);
		AssertPtrN(m_prghsnd);
		Assert(m_prghsnd || !m_ihsndMax);
		Assert(!m_prghsnd || m_ihsndMax);
		AssertArray(m_prghsnd, m_ihsndMax);
		Assert(0 <= m_ihsndLim && m_ihsndLim <= m_ihsndMax);
		Assert(-1 <= FreeListIdx(m_ihsndFirstFree));
		Assert(FreeListIdx(m_ihsndFirstFree) < m_ihsndLim);
		return true;
	}
#endif
	//:End Ignore

protected:
	//:> Member variables

	int * m_prgihsndBuckets;
	int m_cBuckets;
	HashNode * m_prghsnd;
	int m_ihsndLim;
	int m_ihsndMax;
	int m_ihsndFirstFree;		// stores -(ihsnd + 3)

	//:> Protected methods
	//:Ignore

	/*------------------------------------------------------------------------------------------
		Map between real index and "free list" index.  Note that this mapping is bidirectional.
	------------------------------------------------------------------------------------------*/
	int FreeListIdx(int ihsnd)
	{
		return -(ihsnd + 3);
	}
	//:End Ignore
};


// Local Variables:
// mode:C++
// c-file-style:"cellar"
// tab-width:4
// End:

#endif /*HASHMAP_H_INCLUDED*/
