// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Header for class specific to collision handling
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__RTPCOLLISIONMNG_H__)
#define __RTPCOLLISIONMNG_H__

#ifndef __RTPCORE_UNITTEST_CASES_
#include "rtpcontroller.h"
#include "rtpsourceentry.h"
#else
#include "ut_stubclasses.h"
#endif

const TUint32 KIsAlreadySending = 0x80000000;



/**
@internalComponent
*/
class TRtpConflictEntry
	{

public:
	static	TBool MatchAddress(const TRtpConflictEntry& aFirst,
									const TRtpConflictEntry& aSecond);
	TRtpConflictEntry(TUint32 aSSRC, TSockAddr& aAddr, CRtpHandlerBase* aHandler);
	void SetTime(TUint32 aSeconds,TUint32 aMicroSeconds);
	
private:
	TUint32 iSSRC;
	TSockAddr iConflictAddr;
	TNTPTime iTime;
	CRtpHandlerBase* iHandler;
	};
	
	

NONSHARABLE_CLASS(CRtpCollisionMng) : public CBase
/** 
A RTP collision manager. 
*/
	{
public:
	static CRtpCollisionMng* NewL(CRtpController* aRtpController,
									TUint32 aSSRC, const TDesC8& aCNAME);
	virtual ~CRtpCollisionMng();

	CRtpSourceEntry* CreateNewSourceL(TUint32 aSource,const TDesC8& aCNAME = KNullDesC8);
	
	CRtpSourceEntry* FindEntry(TUint32 aSRC);
	
	inline CRtpSourceEntry& GetEntry(TInt aIndex);
	inline TInt GetNumEntries();

	TUint8 GetValidEntries();

	TBool IsExistingSource(TUint32 aSRC);
	CRtpSourceEntry* GetLocalEntry();
	inline const TDesC8& LocalCname();
	inline void SetLocalCnameL(const TDesC8& aCNAME);
	TBool IsCollisionOrLoopback(CRtpSourceEntry* aEntry, TSockAddr& aSourceAddr,
								TRtpType aRtpType);
	inline TBool IsLocalCollisionOrLoop(TUint32 aSSRC);
	//TBool IsInConflictTable(TUint32 aSource,TSockAddr& aSourceAddr);
	TInt FindInConflictTable(TUint32 aSRC,TSockAddr& aSourceAddr);
	void UpdateConflictListL(TInt aIndex);
	void CreateConflictEntryL(TUint32 aSRC,TSockAddr& aSourceAddr);
	
	TBool ResolveRtpCollisionOrLoopL(TUint32 aSRC,TSockAddr& aSourceAddr,
									TUint aTypeAndSending);
									//TBool aAlreadySending,TRtpType aRtpType);
	
	TInt InsertEntryL(const CRtpSourceEntry* aEntry/*,TInt aPos = KErrNotFound*/);
	void DeleteEntry(TUint aSSRC);
	
	TInt UpdateLoopbackCount(TUint32 aSRC);
	TInt UpdateCollisionCount(TUint32 aSRC);

	void ChangeSSRC(TUint32 newSource);
	TBool IsRealConflict();


	inline void SetLocalSSRC(TUint32 aSource);
	inline TUint32 LocalSSRC();
private:
	CRtpCollisionMng(TUint32 aSSRC);
	
	void ConstructL(CRtpController* aRtpController, const TDesC8& aCNAME);
	TInt GetIndex(TUint32 aSSRC);
	TUint32 GenerateSourceIdL();
	
private:
	CRtpController* iRtpController;
	/**
	A single-entry cache is used for faster SSRC lookups;
	it is assumed that data will be typically received from one source at a time.
	*/
	CRtpSourceEntry* iSingleEntryCache;
	/**
	A table of all participants in the session indexed by the source identifier 
	and containing the source transport addresses from the first RTP packet and first RTCP packet received with that identifier,
	along with other state for that source.
	@see CRtpSSRCEntry
	*/
	RPointerArray<CRtpSourceEntry> iSourceTable;
	/** 
	A list of source transport addresses that have been found to be conflicting.
	*/
	RArray<TRtpConflictEntry> iConflictTable;
	//
	TUint32 iLocalSSRC;
	};

/**
Returns the source from the table if exists, otherwise it panics.

@internalComponent
@return Found Source entry
@param aIndex - must not be negative and must not be greater than the number of objects currently in the array,
				otherwise the operator raises a USER-130 panic. 
@pre 
@post 
*/

inline CRtpSourceEntry& CRtpCollisionMng::GetEntry(TInt aIndex)
	{
	return *iSourceTable[aIndex];
	}

inline TInt CRtpCollisionMng::GetNumEntries()
	{
	return iSourceTable.Count();
	}

inline const TDesC8& CRtpCollisionMng::LocalCname()
	{
	return GetLocalEntry()->Cname();
	}

inline void CRtpCollisionMng::SetLocalCnameL(const TDesC8& aCNAME)
	{
	GetLocalEntry()->SetCnameL(aCNAME);
	}

/**
Checks if collision or loopback with our own SSRC 

@internalComponent
@return TRUE if incoming SSRC matches our own
@param aSSRC An incoming source identifier
@pre 
@post 
*/
inline TBool CRtpCollisionMng::IsLocalCollisionOrLoop(TUint32 aSSRC)

	{
	return (aSSRC == iLocalSSRC);
	}

inline void CRtpCollisionMng::SetLocalSSRC(TUint32 aSource)
	{
	CRtpSourceEntry* entry = NULL;
	
	entry = FindEntry(aSource);
	if(!entry)
		{
		/* The SRC will be made local only if there is no Matching SRCs 
		   in the entry list */
		entry = GetLocalEntry();
		entry->SetSRC(aSource);
		iLocalSSRC = aSource;
		}
	}

inline TUint32 CRtpCollisionMng::LocalSSRC()
	{
	return iLocalSSRC;
	}


#endif

