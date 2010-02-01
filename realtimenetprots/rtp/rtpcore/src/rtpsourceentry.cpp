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
// class for handling rtp source tables
// 
//

/**
 @file
 @internalComponent
*/



#include "rtpsourceentry.h"
#include "rtpstatemachine.h"
#include "rtputils.h"
#include "rtpcontroller.h"
#include "rtpcollisionmng.h"

/** Creates an entry for a specific RTP source

@internalTechnology
@param aSource The source identifier, e.g. SSRC or CSRC 
@param aCNAME CNAME, a canonical end-point identifier,
@param aController Link to the owning RTP controller
@return 
@leave KErrNoMemory
*/

CRtpSourceEntry* CRtpSourceEntry::NewLC(TUint32 aSSRC, const TDesC8& aCNAME,
								   CRtpController* aController)
	{	
	__RTP_LOG(_L("CRtpSourceEntry::NewL() ... "));
	__RTP_LOG1(_L("\t[SSRC] == %u"), aSSRC);
	
	CRtpSourceEntry* self = new(ELeave) CRtpSourceEntry(aSSRC);
	CleanupStack::PushL(self);
	self->iController = aController;
	self->ConstructL(aCNAME);
	return self;
	}

CRtpSourceEntry::~CRtpSourceEntry()
	{
	delete iCNAME;
	}

/** Constructs the source entry, allocates CNAME.

@internalTechnology
@param aCNAME CNAME, a canonical end-point identifier.
*/
void CRtpSourceEntry::ConstructL(const TDesC8& aCNAME)
	{	
	__RTP_LOG(_L("CRtpSourceEntry::ConstructL() ... "));
	if(0 != aCNAME.Compare(KNullDesC8))
		{
		iCNAME = aCNAME.AllocL();
		}
	Reset(ERtpInactive);
	}

/** Allocates the CNAME if doesn't exists yet.

@internalTechnology
@param aCNAME The CNAME, a canonical end-point identifier.
@leave KErrAlreadyExists if trying to re-create CNAME
*/
void CRtpSourceEntry::SetCnameL(const TDesC8& aCNAME)
	{	
	__RTP_LOG(_L("CRtpSourceEntry::SetCnameL() ... "));
	if(iCNAME)
		{
		User::Leave(KErrAlreadyExists);
		}
	else
		{
		iCNAME = aCNAME.AllocL();
		}
	}


const TDesC8& CRtpSourceEntry::Cname()
	{
	__RTP_LOG(_L("CRtpSourceEntry::Cname() ... "));
	if(iCNAME)
		return *iCNAME;
	else
		return KNullDesC8;
	}

CRtpSourceEntry::CRtpSourceEntry(TUint32 aSRC) : iSRC(aSRC)
	{
	__RTP_LOG(_L("CRtpSourceEntry::CRtpSourceEntry() ... "));
	}

/** Checks if the sequence number is valid
@internalTechnology
*/
TBool CRtpSourceEntry::IsValidSequence(TUint16 aSequenceNo,TUint aMaxDropout,TUint aMaxMisorder)
	{	
	TBool result = EFalse;
	TUint16 delta = static_cast<TUint16>(aSequenceNo - MaxSequenceNo());
	__RTP_LOG(_L("CRtpSourceEntry::IsValidSequence() ... "));
	if (delta < aMaxDropout) 
		{
		// in order, with permissible gap
		if (aSequenceNo < MaxSequenceNo()) 
			{
			// Sequence number wrapped - count another 64K cycle.
			IncShiftedCycles(KRtpSequenceMod);
			}
		SetMaxSequenceNo(aSequenceNo);
		result = ETrue;
		} 
	else if (delta <= KRtpSequenceMod - aMaxMisorder) 
		{
		// the sequence number made a very large jump
		SetBadSequenceNo((aSequenceNo + 1) & (KRtpSequenceMod - 1));
		result = EFalse;
		} 
	else 
		{
		// duplicate or re-ordered packet
		result = ETrue;
		}
	__RTP_LOG1(_L("\t==> %d"), result);
	return result;
	}

TInt CRtpSourceEntry::CompareSSRC(const CRtpSourceEntry& aFirst,const CRtpSourceEntry& aSecond)
	{	
	TInt result =0;
	if (aFirst.iSRC < aSecond.iSRC)
		result = -1;
	else if (aFirst.iSRC > aSecond.iSRC)
		result = 1;
	
	return result;
	}


/** Reset ALL attributes for this source apart from C/SSRC and CNAME
@internalTechnology
*/
void CRtpSourceEntry::Reset(TRtpSourceState aState,TUint aProbationCount)
	{	
	iIsInConflictList = EFalse;
	iCollisionCount = 0;
	iLoopbackCount = 0;
	iState = aState;
	if(ERtpProbation == aState)
		iProbationCount = aProbationCount;
	ClearDeltaTimerEntry();
	InitialiseSequenceNo(0);
	//
	ResetRtcpStatistics();
	}

void CRtpSourceEntry::InitialiseSequenceNo(TUint16 aSequenceNo)
	{	
	// The highest seq. number seen
	iMaxSequenceNo = aSequenceNo;
	// shifted count of seq. number cycles
    iShiftedCycles = 0;
	// base seq number
    iBaseSequenceNo = aSequenceNo;
	// last 'bad' seq number
    iBadSequenceNo = KRtpSequenceMod + 1; // so sequnce number  == iBadSequenceNo is false
	// packets received
    iPacketsReceived = 0;
	// packet expected at last interval
    iExpectedPrior = 0;
	// packet received at last interval
    iReceivedPrior = 0;

	// jitter related attributes are reset in ResetRtcpStatistics()
	}

TBool CRtpSourceEntry::IsSequenceInitialised()
	{
	if(0 == iMaxSequenceNo && 0 == iShiftedCycles &&  0 == iBaseSequenceNo)
		return EFalse;
	return ETrue;
	}

void CRtpSourceEntry::ResetRtcpStatistics()
	{
	// relative trans time for prev pkt
    iRelTransitTime = 0;
	// estimated jitter
    iJitter = 0;
	// time attributes for RTCP SR packets
	iLastSRTimestamp = 0;
	iWhenLastSRReceived = 0;
	// and send RTCP ones...
	// sender's packet count
    iPacketsSent = 0;
	// sender's octet count
    iOctetsSent = 0;
  
	}

void CRtpSourceEntry::SetSRC(TUint32 aSRC)
	{	
	/* Donot Reset Probation count */
	Reset(ERtpProbation,iProbationCount);
	iSRC = aSRC;
	}

/** Calculates jitter and updates relevant RTCP atributes
@internalTechnology
@param aRtpTimeFromPacket - the timestamp from the incoming packet in RTP time units
@param aRtpArrivalTime - the current time in the same units
*/
void CRtpSourceEntry::UpdateRtcpStatisticsL(TUint aRtpTimeFromPacket,TUint aPayloadType)
	{	
	__RTP_LOG(_L("CRtpSourceEntry::UpdateRtcpStatistics ... "));
	__RTP_LOG1(_L("\t[SSRC] == %u"), iSRC);
	
	/* See if RTP Time is initialised */
	if(!iRtpTime.IsValid())
		{
		TUint32 samplerate = iController->GetSamplingRate(aPayloadType);
		if(!samplerate)
			{
			//So no updation possible. 
			return;
			}
		TUint32 tickPerMusec = 1000*1000/samplerate; 
		iRtpTime.SetTimeConversion(aRtpTimeFromPacket, tickPerMusec);
		}
	
	TUint32 rttpArrivalTime = iRtpTime.GetTimeStampL();
    // sequence related attributes (iMaxSequenceNo, iShiftedCycles, iPacketsReceived) etc.
	// updated in the state-machine within the valid state (CRtpValidState::ProcessL)
	

	// A.8 Estimating the Interarrival Jitter (RFC 3550)
	// iRelTransitTime(transit) holds the relative transit time for the previous packet, 
	// iJitter holds the estimated jitter.  The jitter field of the reception report is
	// measured in timestamp units and expressed as an unsigned integer, but
	// the jitter estimate is kept in a floating point.  As each data packet
	// arrives, the jitter estimate is updated:
	TInt transit = rttpArrivalTime - aRtpTimeFromPacket;
    TInt d = transit - iRelTransitTime;
    iRelTransitTime = transit;
	if (d < 0) d = -d;
	
	// we don't want floating point arithmetics
	//		iJitter += (1./16.) * ((double)d - s->jitter);
	// so alternatively, the jitter estimate can be kept as an integer, but
	// scaled to reduce round-off error.
	iJitter += d - ((iJitter + 8) >> 4);
	// end of jitter section //
	
	// When a reception report block (to which rr points) is generated for
	// this member, the current jitter estimate is returned for floating point:
	//		rr->jitter = (u_int32) s->jitter;
	// where for int arithmetics, the estimate is sampled for the reception report as:
	//		rr->jitter = s->jitter >> 4;
	// should we do here or when assembling RR packet??
	}

void CRtpSourceEntry::SetDeltaTimerEntry(TCallBack& aCallBack, TRtpSourceEvent aEvent)
	{	
	iDeltaTimerEntry.Set(aCallBack);
	iTimerEvent = aEvent;
	iIsTimerUsed = ETrue;
	}

void CRtpSourceEntry::ClearDeltaTimerEntry()
	{
	TCallBack callback(NULL); 
	iDeltaTimerEntry.Set(callback); //NULL!! is it OK?!?
	iTimerEvent = ERtpNone;
	iIsTimerUsed = EFalse;
	}

TInt CRtpSourceEntry::HandleTimeoutL(TAny* aSourceEntry)
	{	
	__RTP_LOG(_L("CRtpSourceEntry::HandleTimeout() ... "));
	__ASSERT_DEBUG(aSourceEntry,Panic(ERtpTimeoutNoEntry));
	return (static_cast<CRtpSourceEntry*>(aSourceEntry))->DoHandleTimeoutL();
	}

TInt CRtpSourceEntry::DoHandleTimeoutL()
	{
	//
	__RTP_LOG(_L("CRtpSourceEntry::DoHandleTimeout() ... "));
	__RTP_LOG1(_L("\t[SSRC] == %u"), iSRC);
	__ASSERT_DEBUG(iController,Panic(ERtpSourceControllerNull));
	__ASSERT_DEBUG(iController->SourceStateMachine(),Panic(ERtpStateMachineNull));
	iIsTimerUsed = EFalse; // so state machine would try to remove timer from queue
	TRtpSourceState state = iController->SourceStateMachine()->NextL(*this,iTimerEvent,0);
	__RTP_LOG1(_L("\t[State] == %d"), static_cast<TInt>(state));
	if(ERtpInactive == state)
		{
		// To indicate Inactive/Delete states we'd get the RTP handler and complete client
		//		iController->RtpHandler()->CompleteClient(KErrTimedOut);
		// But we can't really do that since we might have two incoming streams (distinctive SSRC)
		// and we'd get complet's messed up!
		// So how do we indicate Inactive/Delete states? Do we have to?? 
		// We should send an event to client dll here ...
		
		// Do we need to update entry here?
		}
	else if (ERtpDelete == state)
		{
		iController->CollisionManager()->DeleteEntry(SRC());
		}
	ClearDeltaTimerEntry(); // and clear completely now...
	return 0;
	}

