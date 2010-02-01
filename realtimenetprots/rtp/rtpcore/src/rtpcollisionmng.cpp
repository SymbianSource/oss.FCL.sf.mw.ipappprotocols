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
// rtpcollisionmg.cpp
// class for handling collision scenarios
// 
//

/**
 @file
 @internalComponent
*/

#include <e32base.h> 

#ifndef __RTPCORE_UNITTEST_CASES_
#include "rtpcollisionmng.h"
#include "rtpstatemachine.h"
#include "rtputils.h"
#include "rtcppacket.h"
#include "rtcpsender.h"
#else
#include "ut_stubclasses.h"
#include "flogger.h"
#define Panic(x) PanicL(x) 
#endif
#include "rtpcollisionmng.h"
#include "in_sock.h"


class TRtpConflictEntry;

/**
Create a new Collision Manager

@internalComponent
@return - new instance of the Collision manager
@param aRtpController - owning RTP controller
@param aSSRC - local source identifier
@param aCNAME - CNAME in case RTCP is used as well
*/

CRtpCollisionMng* CRtpCollisionMng::NewL(CRtpController* aRtpController,
										 TUint32 aSSRC,const TDesC8& aCNAME)
	{
	__RTP_LOG1(_L("CRtpCollisionMng::NewL() ... LocalSSRC -> %u"), aSSRC);
	CRtpCollisionMng* self = new(ELeave) CRtpCollisionMng(aSSRC);
	CleanupStack::PushL(self);
	self->ConstructL(aRtpController,aCNAME);
	CleanupStack::Pop(self);
	return self;
	}

CRtpCollisionMng::~CRtpCollisionMng()
	{
	iSourceTable.ResetAndDestroy();
	// is conflict table only close??
	iConflictTable.Close();
	}

/**
Creates a new source and inserts it into the table of known sources.

@internalComponent
@return 
@param aSource A source identifier of newly idetified source
@param aCNAME A CNAME of that source if known otherwise KNullDesC8
@leave KErrAlreadyExists if an attempt is being made to insert a duplicate entry
@leave KErrNoMemory
@pre 
@post 
*/

CRtpSourceEntry* CRtpCollisionMng::CreateNewSourceL(TUint32 aSSRC, const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("CRtpCollisionMng::CreateNewSourceL() ... "));
	__RTP_LOG1(_L("\t[Source] == %u"), aSSRC);
	CRtpSourceEntry* newEntry = CRtpSourceEntry::NewLC(aSSRC,aCNAME,iRtpController);
	newEntry->SetState(ERtpProbation);
	InsertEntryL(newEntry);
	CleanupStack::Pop(newEntry);
	return newEntry;
	}



/**
Returns the source entry if the source is already in the table 

@internalComponent
@return The entry if found NULL otherwise
@param aSRC An identifier of a source we are looking for
@pre 
@post 
*/

CRtpSourceEntry* CRtpCollisionMng::FindEntry(TUint32 aSRC)
	{
	__RTP_LOG(_L("CRtpCollisionMng::FindEntry() ... "));
	__RTP_LOG1(_L("\t[SSRC] == %u"), aSRC);	
	__ASSERT_DEBUG(iSingleEntryCache,Panic(ERtpSingleEntryCacheNull));
	// we use a single 'cache' as most of the time we might be dealing with 1 source only
	if(iSingleEntryCache->SRC() == aSRC)
		{
		// we are looking for the same entry as the last time
		return iSingleEntryCache;
		}
	
	const TInt entryCount = GetNumEntries();
	for (TInt count = 0; count < entryCount; count++)
		{
		iSingleEntryCache = iSourceTable[count];
		if(iSingleEntryCache->SRC() == aSRC)
			{
			return iSingleEntryCache;
			}
		}
	return NULL;
	} 

TBool CRtpCollisionMng::IsExistingSource(TUint32 aSRC)
	{	
	return KErrNotFound == GetIndex(aSRC) ? EFalse : ETrue;
	}

/**
Returns the local source entry if exists, otherwise it panics.

@internalComponent
@return Pointer to the local entry in the source table 
@panic ERtpCollisionMgrNoLocalSSRC if the local source entry does not exists
@pre 
@post 
*/
CRtpSourceEntry* CRtpCollisionMng::GetLocalEntry()
	{
	__RTP_LOG(_L("CRtpCollisionMng::GetLocalEntry() ... "));	
	CRtpSourceEntry* entry = NULL;
	if(iSingleEntryCache->SRC() == iLocalSSRC)
		{
		entry = iSingleEntryCache;
		}
	else 
		{
		entry = FindEntry(iLocalSSRC);
		}
	
	__ASSERT_ALWAYS(NULL != entry,Panic(ERtpCollisionMgrNoLocalSSRC));
	
	return entry;
	}


/**
Checks if there is a collision or s loopback
If the SSRC from incoming packet is found in table (represented by source entry here)
then the source address of the incoming packet is checked against stored address

@internalComponent
@return True if there is a collision or loopback identified
@param aEntry A source entry associated with the incoming packet
@param aSourceAddr A source address from the incoming packet
@param aRtpType RTP or RTCP packet 
@panic ERtpCollisionMgrWrongType - if neither Rtp nor Rtcp - we may define more types in future
@pre 
@post 
*/

TBool CRtpCollisionMng::IsCollisionOrLoopback(CRtpSourceEntry* aEntry, TSockAddr& aSourceAddr, 
											  TRtpType aRtpType)
	{
	__RTP_LOG(_L("CRtpCollisionMng::IsCollisionOrLoopback() ... "));
	__RTP_LOG1(_L("\t[SourceAddrPort] == %u"), aSourceAddr.Port());
	__RTP_LOG1(_L("\t[SourceAddrFamily] == %u"), aSourceAddr.Family());
	__RTP_LOG1(_L("\t[RtpType] == %d"), static_cast<TInt>(aRtpType));	
	// source address according to type as stored in the source entry table
	const TSockAddr* entryAddr = 0;
	CRtpHandlerBase* handler = NULL;
	if(ERtp == aRtpType)
		{
		entryAddr = &(aEntry->RtpSourceAddr());
		handler = iRtpController->RtpHandler();
		}
	else if(ERtcp == aRtpType)
		{
		entryAddr = &(aEntry->RtcpSourceAddr());
		handler = iRtpController->RtcpHandler();
		}
	else
		{
		Panic(ERtpCollisionMgrWrongType);
		}
	__ASSERT_ALWAYS(entryAddr,Panic(ERtpCollisionMgrNoAddr));
	__ASSERT_ALWAYS(handler,Panic(ERtpHandlerIsNotCreated));
	TBool collision = ETrue;
	collision = !(handler->CompareAddresses(aSourceAddr,*entryAddr));
	
	if(collision && KAFUnspec == entryAddr->Family())
		{
		// doesn't match because not set yet!
		// This would happen when a first packet received from a particular source is RTCP
		// and the some time later a first RTP packet is received. And vice versa...
		if(ERtp == aRtpType)
		    {
		    TSockAddr entryRtcpAddr = (aEntry->RtcpSourceAddr());
		    entryRtcpAddr.SetPort(aSourceAddr.Port());
		    if(handler->CompareAddresses(aSourceAddr,entryRtcpAddr))
		        {
		        aEntry->SetRtpSourceAddr(aSourceAddr);
		        collision = EFalse;
		        }
		    }
		else if(ERtcp == aRtpType)
		    {
		    TSockAddr entryRtpAddr = (aEntry->RtpSourceAddr());
		    entryRtpAddr.SetPort(aSourceAddr.Port());
		    if(handler->CompareAddresses(aSourceAddr,entryRtpAddr))
		        {
		        aEntry->SetRtcpSourceAddr(aSourceAddr);
		        collision = EFalse;
		        }
		    }
		}
	__RTP_LOG1(_L("\t==> %d"), collision);
	return collision;
	}


/**
Checks if presented source ID from this particular source address
has beeen detected as conflicting before

@internalComponent
@return 
@param aSRC
@param aSourceAddr
@pre 
@post 
*/
TInt CRtpCollisionMng::FindInConflictTable(TUint32 aSRC,TSockAddr& aSourceAddr)
	{	
	__RTP_LOG(_L("CRtpCollisionMng::FindInConflictTable() ... "));
	__RTP_LOG1(_L("\t[SSRC] == %u"), aSRC);
	__RTP_LOG1(_L("\t[SourceAddrPort] == %u"), aSourceAddr.Port());
	__RTP_LOG1(_L("\t[SourceAddrFamily] ==%u"), aSourceAddr.Family());
	TInt index = iConflictTable.Find(TRtpConflictEntry(aSRC,aSourceAddr,iRtpController->RtpHandler()),
									TIdentityRelation<TRtpConflictEntry>(TRtpConflictEntry::MatchAddress));
	
	return index;
	}

/**
@internalComponent
@leave
*/
void CRtpCollisionMng::UpdateConflictListL(TInt aIndex)
	{	
	__RTP_LOG(_L("CRtpCollisionMng::UpdateConflictList() ... "));
	__RTP_LOG1(_L("\t[Index] == %d"), aIndex);	
	__ASSERT_DEBUG(aIndex>=0,Panic(ERtpParameterNegative));
	__ASSERT_DEBUG(aIndex<iConflictTable.Count(),Panic(ERtpParameterOutOfRange));
	TNTPTime time = RtpUtils::GetNTPTimeL();
	iConflictTable[aIndex].SetTime(time.iSeconds, time.iMicroSeconds);
	}

/**
@internalComponent
@leave
*/
void CRtpCollisionMng::CreateConflictEntryL(TUint32 aSRC,TSockAddr& aSourceAddr)
	{
    __RTP_LOG(_L("CRtpCollisionMng::NewEntryInConflictList() ... "));
	__RTP_LOG1(_L("\t[Source] == %u"), aSRC);
	__RTP_LOG1(_L("\t[SourceAddrPort] = %u"), aSourceAddr.Port());
	__RTP_LOG1(_L("\t[SourceAddrFamily] = %u"), aSourceAddr.Family());
	TRtpConflictEntry newEntry(aSRC, aSourceAddr,iRtpController->RtpHandler());
	TNTPTime time = RtpUtils::GetNTPTimeL();
	newEntry.SetTime(time.iSeconds, time.iMicroSeconds);
	User::LeaveIfError(iConflictTable.Append(newEntry));
	}

/**
Resolves collision or loop if possible

@internalComponent
@return TRUE if resolved thus we can continue with processing this packet
@param aSRC - A source identifier
@param aSourceAddr
@param aTypeAndSending - bitwise OR of TRtpType and KIsAlreadySending
@pre 
@post 
*/
TBool CRtpCollisionMng::ResolveRtpCollisionOrLoopL(TUint32 aSRC,TSockAddr& aSourceAddr,
													TUint aTypeAndSending)
	{
	__RTP_LOG(_L("CRtpCollisionMng::ResolveRtpCollisionOrLoop"));
	__RTP_LOG1(_L("\t[SSRC] == %u"), aSRC);
	__RTP_LOG1(_L("\t[SourceAddrPort] == %u"), aSourceAddr.Port());
	__RTP_LOG1(_L("\t[SourceAddrFamily] == %u"), aSourceAddr.Family());
	TBool continueWithProcesing = EFalse;
	if(IsLocalCollisionOrLoop(aSRC))
		{
		// local one
		// get index in the conflict table
		TInt indexInConflictTable = FindInConflictTable(aSRC,aSourceAddr);
		if(KErrNotFound != indexInConflictTable)
			{
			// own loop! Update in the table of source identifiers for our own SSRC
			UpdateLoopbackCount(aSRC);
			UpdateConflictListL(indexInConflictTable);
			}
		else
			{
			// new source ID
			TUint32 newSSRC = GenerateSourceIdL();
			__ASSERT_DEBUG(GetLocalEntry()->SRC() == aSRC,Panic(ERtpSourceMismatch)); //we must be dealing with local SSRC here
			// change the local SSRC - updates controller, entry and handlers
			ChangeSSRC(newSSRC);
			//if(aAlreadySending)
			if(KIsAlreadySending & aTypeAndSending)	
				{
				// there would be no point doing all of this if we were not sending yet
				// update count in source table
				UpdateCollisionCount(newSSRC);
				// create entry in conflict list
				CreateConflictEntryL(aSRC,aSourceAddr);
				iRtpController->SendBYEPacketL(aSRC,KRtcpByeReasonCollision);
				}
			
			// and create new entry in the source table for this newly 
			// identified source with the old local SSRC
			CRtpSourceEntry* entry = CreateNewSourceL(aSRC);
			if(ERtp == ((~KIsAlreadySending)&aTypeAndSending))
				entry->SetRtpSourceAddr(aSourceAddr);
			else if(ERtcp == ((~KIsAlreadySending)&aTypeAndSending))
				entry->SetRtcpSourceAddr(aSourceAddr);
			//else??
			continueWithProcesing = ETrue;
			}
		}
	else
		{
		// external conflict
		if(!IsRealConflict())
			{
			// could be just mobile changing address etc.
			continueWithProcesing = ETrue;
			}
		}
	__RTP_LOG1(_L("\t==> %d"), continueWithProcesing);
	return continueWithProcesing;
	}


TUint32 CRtpCollisionMng::GenerateSourceIdL()
	{
	__RTP_LOG(_L("CRtpCollisionMng::GenerateSourceIdL() ... "));
	TUint32 ssrc = RtpUtils::GenerateRandomNumber32L();
	/* oops this would have been deadlock :-(
	TInt index = iCollisionManager->FindEntry(ssrc);
	*/
	while (IsExistingSource(ssrc))
		{
		/* source identifier already in table generate another one...
		How likely this is? Is generate going to generate a different number
		without re-seeding? */
		ssrc = RtpUtils::GenerateRandomNumber32L();
		}
	__RTP_LOG1(_L("\t==> %u"), ssrc);
	return ssrc;
	}


//TInt CRtpCollisionMng::UpdateLoopFromSource(TUint32 aSSRC)
/**
Update count of loops from this particullar source

@internalComponent
@return Current loopback count
@param Source identifier
@pre 
@post 
*/
TInt CRtpCollisionMng::UpdateLoopbackCount(TUint32 aSSRC)
	{
	__RTP_LOG(_L("CRtpCollisionMng::UpdateLoopFromSource() ... "));
	__RTP_LOG1(_L("\t[SSRC] == %u"), aSSRC);
	CRtpSourceEntry* entry = FindEntry(aSSRC);
	return NULL == entry ? 0 : ++entry->iLoopbackCount;
	}

//TInt CRtpCollisionMng::UpdateCollisionCount(TUint32 aSRC)
/**
Update count of loops from this particullar source

@internalComponent
@return 
@param 
@param 
@pre 
@post 
*/
TInt CRtpCollisionMng::UpdateCollisionCount(TUint32 aSRC)
	{
	__RTP_LOG(_L("CRtpCollisionMng::UpdateCollisionFromSource() ... "));
	__RTP_LOG1(_L("\t[SSRC] == %u"), aSRC);
	CRtpSourceEntry* entry = FindEntry(aSRC);
	return NULL == entry ? 0 : ++entry->iCollisionCount;
	}

//void CRtpCollisionMng::ChangeSSRC(TUint32 aNewSSRC)
/** Replaces old SSRC with the new one - the new source is updated
	in the controller, in the table of sources and in the RTP sockets as well.

@internalComponent
@param 
@param 
@pre 
@post 
*/

void CRtpCollisionMng::ChangeSSRC(TUint32 aNewSSRC)
	{
	__RTP_LOG(_L("CRtpCollisionMng::ChangeSSRC() ... "));
	__RTP_LOG1(_L("\t[NewSSRC] == %u"), aNewSSRC);
	CRtpSourceEntry* entry = GetLocalEntry();
	if (NULL != entry)
		{
		// source identifier found as expected
		entry->SetSRC(aNewSSRC);
		iLocalSSRC = aNewSSRC;
		}

	iRtpController->HandleCollision();
	}

TBool CRtpCollisionMng::IsRealConflict()
/**
Some sources (mobiles) may change addresses during the course of an RTP session, 
thus collision detection algorithm SHOULD accept packets from the new source transport address.
To guard against flip-flopping between addresses if a genuine collision does occur, 
the algorithm SHOULD include some means to detect this case and avoid switching.

@internalComponent
@return ETrue indicates genuine conflict 
		where EFalse changed address for mobile device
@param 
@param 
@pre 
@post 
*/
	{
	// Treat as conflict for now
	return ETrue;
	}

/**
Insert a new entry in the source table.
Inserted in order not allowing any duplicates.

@internalComponent
@return Index of the newly inserted entry in the source table
@param aEntry - new entry to be inserted in the source table
@leave	KErrNone, if the insertion is successful; 
		KErrAlreadyExists, if an attempt is being made to insert a duplicate entry; 
		otherwise one of the system wide error codes.
@pre 
@post 
*/
TInt CRtpCollisionMng::InsertEntryL(const CRtpSourceEntry* aEntry)
	{	
	__RTP_LOG2(_L("CRtpCollisionMng[0x%x]::InsertEntryL() ... %u"), this, aEntry->SRC());
	// insert in order 
	TLinearOrder<CRtpSourceEntry> order(CRtpSourceEntry::CompareSSRC);
	User::LeaveIfError(iSourceTable.InsertInOrder(aEntry,order));

	return iSourceTable.FindInOrder(aEntry, order);
	}

/**
Deletes the source from the table if exists, otherwise it panics.

@internalComponent
@return 
@param aSRC - suorce identifier for entry to be deleted 
@panic ERtpSourceEntryNull if the source entry does not exists
@pre 
@post 
*/
void CRtpCollisionMng::DeleteEntry(TUint aSRC)
	{	
	__RTP_LOG2(_L("CRtpCollisionMng[0x%x]::DeleteEntry() ... %u"), this, aSRC);
	TInt index = GetIndex(aSRC);
	__ASSERT_ALWAYS(KErrNotFound != index,Panic(ERtpSourceEntryNull));
	//delete the entry
	iSingleEntryCache = iSourceTable[index];
	iSourceTable.Remove(index);
	delete iSingleEntryCache;
	iSourceTable.Compress();
	// lets pick another (safe) one
	iSingleEntryCache = iSourceTable[0];
	}

CRtpCollisionMng::CRtpCollisionMng(TUint32 aSSRC)
	{	
	__RTP_LOG(_L("CRtpCollisionMng::CRtpCollisionMng() ..."));
	__RTP_LOG1(_L("\t[SSRC] == %u"), aSSRC);
	iLocalSSRC = aSSRC;
	}
	
void CRtpCollisionMng::ConstructL(CRtpController* aRtpController,const TDesC8& aCNAME)
	{	
	__RTP_LOG(_L("CRtpCollisionMng::ConstructL() ... "));
	__ASSERT_DEBUG(aRtpController, Panic(ERtpControllerIsNotCreated));
	iRtpController = aRtpController;
	// iSourceTable should be empty
	__ASSERT_DEBUG(0 == iSourceTable.Count(), Panic(ERtpCollisionMgr));
	CRtpSourceEntry* newEntry = CRtpSourceEntry::NewLC(iLocalSSRC,aCNAME,iRtpController);
	newEntry->SetState(ERtpProbation);
	User::LeaveIfError(iSourceTable.Append(newEntry));
	// A single-entry cache is used for faster SRC lookups;
	// it points to the local entry for now
	iSingleEntryCache = newEntry;
	CleanupStack::Pop(newEntry);
	// conflict table... lets compress assuming conflicts do not happend to often,
	//downside adding a new conflict entry will result in a re-allocation of memory.
	iConflictTable.Compress();
	}

/**
Looks up an index for a particular source

@internalComponent
@return Index of the entry if found KErrNotFound otherwise
@param aSRC An identifier of a source we are looking for
@pre 
@post 
*/
	
TInt CRtpCollisionMng::GetIndex(TUint32 aSRC)
	{
	const TInt entryCount = GetNumEntries();
	
	for (TInt count = 0; count < entryCount; count++)
		{
		CRtpSourceEntry& entry = GetEntry(count);
		if (entry.SRC() == aSRC)
			{			
			return count;
			}
		}
	return KErrNotFound;
	}

TUint8 CRtpCollisionMng::GetValidEntries()
	{
	const TInt entryCount = GetNumEntries();

	TUint8 validCount = 0;
	for (TInt count = 0; count < entryCount; count++)
		{
		CRtpSourceEntry& entry = GetEntry(count);
		if (entry.State() == ERtpValid)
			{
			validCount++;
			}
		}
	return validCount;
	}

// TRtpConflictEntry //
	
TRtpConflictEntry::TRtpConflictEntry(TUint32 aSSRC, TSockAddr& aAddr, CRtpHandlerBase* aHandler):iTime(0,0)
	{
	iSSRC = aSSRC;
	iConflictAddr = aAddr;
	iHandler = aHandler;
	}

TBool TRtpConflictEntry::MatchAddress(const TRtpConflictEntry& aFirst,
											const TRtpConflictEntry& aSecond)
	{
	__RTP_LOG(_L("TRtpConflictEntry::MatchAddress(aFirst,aSecond )"));
	return aFirst.iHandler->CompareAddresses(aFirst.iConflictAddr, aSecond.iConflictAddr);
			
	}
 
		
void TRtpConflictEntry::SetTime(TUint32 aSeconds,TUint32 aMicroSeconds)
	{
	iTime.iSeconds = aSeconds;
	iTime.iMicroSeconds = aMicroSeconds;
	}

