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
// class for handling rtcp session information
// 
//

/**
 @file
 @internalComponent
*/

#include <e32base.h>
#include <random.h>
#include "rtcpsessiondata.h"
#include "rtpstatemachine.h"
#include "rtpcontroller.h"
#include "rtpcollisionmng.h"


LOCAL_D void ClearItems(TSdesItemArray& aArray);


CRtcpSessionData* CRtcpSessionData::NewL(TUint aRtcpBw,TBool aSendReports,
										 CRtpController* aRtpController,const TDesC8& aCname,TInt aPriority)
	{	
	CRtcpSessionData* self = new(ELeave) CRtcpSessionData(aRtcpBw,aSendReports,aPriority, aRtpController);
	CleanupStack::PushL(self);
	self->ConstructL(aCname);
	CleanupStack::Pop(self);
	return self;
	}

CRtcpSessionData::~CRtcpSessionData()
	{
	iSdesItems.ResetAndDestroy();
	delete iByeData;
	delete iRtcpSendData;
	CTimer::Cancel();
	}

void CRtcpSessionData::StopPeriodicReports()
	{	
	iSendPeriodicReports = EFalse;
	}

void CRtcpSessionData::StartPeriodicReports()
	{
	iSendPeriodicReports = ETrue;
	}

void CRtcpSessionData::CanSendReportL()
	{
	SendRtcpL();
	}

void CRtcpSessionData::UserJoinedSession(TBool aIsSender)
	{	
	iCurrentMembersEstimate++;
	if(aIsSender)
		iCurrentSendersEstimate++;
	DoTimerReconsideration();
	}

void CRtcpSessionData::UserLeftSession(TBool aWasSender)
	{	
	iCurrentMembersEstimate--;
	if(aWasSender)
		iCurrentSendersEstimate--;
	
	TInt err;
	TRAP(err,DoReverseTimerReconsiderationL());
	// ignore error - as this was not vital anyway
	(void)err;
	}
	
	
void CRtcpSessionData::DoTimerReconsideration()
	{
	// called after an user joined the session
	/*
	Since the calculated interval is dependent on the number of
      observed group members, there may be undesirable startup effects
      when a new user joins an existing session, or many users
      simultaneously join a new session.  These new users will initially
      have incorrect estimates of the group membership, and thus their
      RTCP transmission interval will be too short.  This problem can be
      significant if many users join the session simultaneously.  To
      deal with this, an algorithm called "timer reconsideration" is
      employed.  This algorithm implements a simple back-off mechanism
      which causes users to hold back RTCP packet transmission if the
      group sizes are increasing.
	*/
	__RTP_LOG(_L("DoTimerReconsideration()"));
	TTimeIntervalMicroSeconds32 interval(0);
	interval = 
		RtpUtils::RtcpIntervalMicroSeconds32(iCurrentMembersEstimate,iCurrentSendersEstimate,
											iRtcpBandwidth,iSentRecently,
											iAverageRtcpSize,iNotSendingYet,iRandSeed);
		iNextTxTime = iLastTxTime + interval;
		
		TTimeIntervalMicroSeconds32 inextinterval(iNextTxTime.GetCompactTime());
	
		if (IsActive())
			{
			Cancel();
			}
		After(inextinterval);
	}


void CRtcpSessionData::DoReverseTimerReconsiderationL()
	{
	//called after an user left the session
	/*
	When users leave a session, either with a BYE or by timeout, the
      group membership decreases, and thus the calculated interval
      should decrease.  A "reverse reconsideration" algorithm is used to
      allow members to more quickly reduce their intervals in response
      to group membership decreases.
	From RFC 3550
    	tn = tc + (members/pmembers)*(tn-tc)
    	tp = tc - (members/pmembers)*(tc-tp)
	*/
	__RTP_LOG(_L("DoReverseTimerReconsiderationL()"));
	if (iCurrentSendersEstimate > 0)
		{
		TNTPTime currTime;
		currTime = RtpUtils::GetNTPTimeL();
		
		currTime -= iInitCurrTime;

		TReal32 fraction = static_cast<TReal32>(iCurrentMembersEstimate)/(iCurrentMembersEstimate+1);
	
		iNextTxTime = currTime + ((iNextTxTime - currTime)*(fraction));
		iLastTxTime = currTime - ((currTime - iLastTxTime)*(fraction));

		TTimeIntervalMicroSeconds32 interval(iNextTxTime.GetCompactTime());
	
		if (IsActive())
			{
			Cancel();
			}
		After(interval);
		}
	}
	

void CRtcpSessionData::RunL()
	{
	__RTP_LOG1(_L("Timer expired from [SSRC] %u"), iRtpController->GetLocalSSRC());

	if(iSendPeriodicReports)
		{
		OnExpireL();
		}
	// we want the timer to run always 
	// in order to update RTCP interval for the state machine timeouts
	Deque();
	CActiveScheduler::Add(this);
	StartTimer();
	}

void CRtcpSessionData::DoCancel()
	{
	CTimer::DoCancel();
	}

TInt CRtcpSessionData::RunError(TInt )
	{	
	return KErrNone;
	}


static const TInt KSdesArrayGranularity = 4;

CRtcpSessionData::CRtcpSessionData(TUint aRtcpBw,TBool aSendReports,TInt aPriority, CRtpController* aRtpController) 
	: CTimer(aPriority), iLastTxTime(0,0), iNextTxTime(0,0), 
	iRtpController(aRtpController), iByePtr(NULL, 0), iSdesItems(KSdesArrayGranularity)
	{	
	// As recommended in RFC:
	// Upon joining the session the stack initializes
	// iCurrentSendersEstimate (senders) to 0, 
	// iLastMembersEstimate (pmembers) to 1, 
	// iCurrentMembersEstimate (members) to 1, iSentRecently (we_sent) to EFalse,
	// iRtcpBandwidth to the specified fraction of the session bandwidth, 
	// iNotSendingYet (initial) to true, and iAverageRtcpSize to the probable size of the first RTCP
	// packet that the application will later construct.  
	
	iLastMembersEstimate = 1;
	iCurrentMembersEstimate = 1;
	iCurrentSendersEstimate = 0;
	iRtcpBandwidth = aRtcpBw;
	iSentRecently = EFalse;
	iAverageRtcpSize = KRtcpDefaultAverageSize;
	iNotSendingYet = ETrue;
	//RTP time stamp at session construction
	//iInitCurrTime = iRtpController->CurrentRtpTimeStampL();
	RtpUtils::GetNTPTime(iInitCurrTime);
	//
	iSendPeriodicReports = aSendReports;
	
	iSDESFlags = 0;
	}
	
void CRtcpSessionData::ConstructL(const TDesC8& aCname)
	{
	iRtcpSendData = HBufC8::NewMaxL(KRtcpDataDefaultSize);
	iByeData = HBufC8::NewL(KRtcpDataDefaultSize);
	iByePtr.Set(iByeData->Des());

	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	
	// when RTCP interval is randomly offset we need a random seed
	TBuf8<8> random(8);
	TRandom::RandomL(random);
	iRandSeed = *(reinterpret_cast<const TInt64*>(random.Ptr()));

	UpdateSdesItemsL(aCname);
	
	/* If Timer is Started here. The Reports go based on the Variable
	   iSendPeriodicReports */
	StartTimer();
	}

// Used above, and used in case of collision
void CRtcpSessionData::UpdateSdesItemsL(const TDesC8& aCname)
	{
	__RTP_LOG(_L("CRtcpSessionData::UpdateSdesItems"));
	// Clear the list
	iSdesItems.ResetAndDestroy();
	CSdesItem* item = CSdesItem::NewL(iRtpController->GetLocalSSRC(), EsdesCNAME, aCname);
	CleanupStack::PushL(item);
	User::LeaveIfError(iSdesItems.InsertInOrder(item, TSdesItemOrder()));
	CleanupStack::Pop();
	item = CSdesItem::NewL(iRtpController->GetLocalSSRC(), EsdesNONE, KNullDesC8);
	CleanupStack::PushL(item);
	User::LeaveIfError(iSdesItems.InsertInOrder(item, TSdesItemOrder()));
	CleanupStack::Pop();
	}
	
void CRtcpSessionData::StartTimer()
	{
	__ASSERT_DEBUG(iRtpController,Panic(ERtpControllerIsNotCreated));
	__ASSERT_DEBUG(iRtpController->SourceStateMachine(),Panic(ERtpStateMachineNotCreated));
	
	__RTP_LOG2(_L("CRtcpSessionData[0x%x] Timer started for [SSRC] %u"), this, iRtpController->GetLocalSSRC());
	
	TTimeIntervalMicroSeconds32 interval(0);
	if (!iRtpController->UserSetInterval(interval))
		{
		interval = 
		RtpUtils::RtcpIntervalMicroSeconds32(iCurrentMembersEstimate,iCurrentSendersEstimate,
											iRtcpBandwidth,iSentRecently,
											iAverageRtcpSize,iNotSendingYet,iRandSeed);
		}
		
	iRtpController->SourceStateMachine()->SetRtcpTransmissionInterval(interval);
	iNextTxTime = iLastTxTime + interval;
	After(interval);
	}
	
void CRtcpSessionData::OnExpireL()
	{
	/* This function is responsible for deciding whether to send an
	 * RTCP report or BYE packet now, or to reschedule transmission.
	 * It is also responsible for updating the pmembers, initial, tp,
	 * and avg_rtcp_size state variables.  This function should be
	 * called upon expiration of the event timer used by Schedule().
	 */
	__RTP_LOG(_L("CRtcpSessionData::OnExpireL"));	
	TTimeIntervalMicroSeconds32 interval(0);
	if (!iRtpController->UserSetInterval(interval))
		{
		interval = 
		RtpUtils::RtcpIntervalMicroSeconds32(iCurrentMembersEstimate,iCurrentSendersEstimate,
											iRtcpBandwidth,iSentRecently,
											iAverageRtcpSize,iNotSendingYet,iRandSeed);
		}

	TNTPTime nextTxTime = iLastTxTime + interval;
	if (nextTxTime.GetCompactTime() <= iNextTxTime.GetCompactTime())
		{
		__RTP_LOG(_L("....CRtcpSessionData::OnExpireL -- Sending RTCP"));    
		SendRtcpL();
		iLastTxTime = nextTxTime;
		}
	}

void CRtcpSessionData::SendRtcpL()
	{
	TInt size = 0;
	size = ComposeRTCPPacketL();
	iAverageRtcpSize = TUint(((TReal32)1/16)*size + ((TReal32)15/16)*(iAverageRtcpSize));
	iRtpController->SendRTCPPacketL();
	iNotSendingYet = EFalse;
	}


void CRtcpSessionData::WriteAppPacket(const TDesC8& aName, const TDesC8& aAppData, TUint8 aSubType)
	{	
	TPtr8 buf(iRtcpSendData->Des());
	buf.Zero();
	TRtcpAppPacket packet(buf);
	packet.WriteToPacket(iRtpController->GetLocalSSRC(), aName, aAppData, aSubType);
	}
/**
Send the pending RTCP packet to this peer when asked to do so

@internalTechnology
@return - number of octets send 
@param 
@param 
@pre 
@post 
*/
TInt CRtcpSessionData::ComposeRTCPPacketL()
	{
	const TUint32 localSsrc = iRtpController->GetLocalSSRC();
	
	__RTP_LOG1(_L("ComposeRTCPPacket from [SSRC] %u"), localSsrc);
		
	TPtr8 buf(iRtcpSendData->Des());
	buf.Zero();
	// Write the report
	WriteReportL(buf, localSsrc);

	__RTP_LOG1(_L("\tComposeRTCPPacket - New Length %d"), buf.Length());

	// Create a new descriptor of remaining space and len 0
	TUint oldLen = buf.Length();
	TPtr8 newPtr(const_cast<TUint8*>(buf.Ptr() + oldLen), buf.MaxLength() - oldLen);

	__RTP_LOG(_L("\tComposeRTCPPacket - SDes"));

	TRtcpSDesPacket packet(newPtr);
	// Update SDESFlags to SDESPackets...
	packet.SetSDESFlags(iSDESFlags);
	packet.WriteToPacket(iSdesItems);
	ClearItems(iSdesItems);
	iSDESFlags =0;
	// add the length of the new descriptor to the old one
	buf.SetLength(oldLen + newPtr.Length());	
	__RTP_LOG1(_L("\tComposeRTCPPacket - New Length %d"), buf.Length());
	
	if (iByeSrc != 0)
		{
		__RTP_LOG2(_L("\t\t- Bye[ByeReason] == %S. [SSRC] == %u"), &iByePtr, iByeSrc);

		oldLen = buf.Length();
		newPtr.Set(const_cast<TUint8*>(buf.Ptr() + oldLen), KRtcpRRBlockSize, buf.MaxLength() - oldLen);
	
		TRtcpByePacket packet(newPtr);
		packet.WriteHdrToPacket(iByeSrc);
		packet.WriteReasonToPacket(iByePtr);
		buf.SetLength(oldLen + newPtr.Length());
		__RTP_LOG1(_L("\tComposeRTCPPacket - New Length %d"), buf.Length());
		iByeSrc = 0;
		}

	return buf.Length();
	}

void CRtcpSessionData::SetByePendingL(TUint32 aLocalSRC, const TDesC8& aReason)
	{	
	iByePtr.Zero();
	iByePtr.Copy(aReason);
	iByeSrc = aLocalSRC;
	CanSendReportL();
	}

void CRtcpSessionData::SetReportPendingL(TInt aReportType)
	{	
	// overloading the iSentRecently variable here
	aReportType== ERtcpSR ?(iSentRecently = ETrue) :(iSentRecently = EFalse);
	CanSendReportL();
	}

void CRtcpSessionData::SetSDesPendingL(TUint32 aSSRCOrCSRC, const TUint8 aItemType, const TDesC8& aItemValue)
	{	
	CSdesItem* item = CSdesItem::NewL(aSSRCOrCSRC, aItemType, aItemValue);
	CleanupStack::PushL(item);
	User::LeaveIfError(iSdesItems.InsertInOrder(item, TSdesItemOrder()));
	CleanupStack::Pop();
	}

void CRtcpSessionData::SetPRIVL(TUint32 aSSRCOrCSRC, const TDesC8& aPrefixString, const TDesC8& aValueString)
	{	
	CSdesItem* item = CSdesItem::NewL(aSSRCOrCSRC, aPrefixString, aValueString);
	CleanupStack::PushL(item);
	User::LeaveIfError(iSdesItems.InsertInOrder(item, TSdesItemOrder()));
	CleanupStack::Pop();
	}

void CRtcpSessionData::WriteReportL(TPtr8& aBuf, TUint aLocalSsrc)
	{	
	__RTP_LOG(_L("CRtcpSessionData::WriteReport"));
	
	const TUint8 entryCount = iRtpController->CollisionManager()->GetValidEntries();
	//__ASSERT_DEBUG(entryCount >= 1, Panic(ERtpSourceEntryCount)); // Not Valid - as we can send SR if we only send

	// Create a new descriptor of remaining space and len 0
	TUint oldLen = aBuf.Length();
	TPtr8 rPtr(const_cast<TUint8*>(aBuf.Ptr() + oldLen), aBuf.MaxLength() - oldLen);
		
	if(iSentRecently)
		{
		__RTP_LOG(_L("\tWriteReport - SR"));
		iSentRecently = EFalse;
		TRtcpSRPacket packet(rPtr);
		CRtpSourceEntry* localEntry = iRtpController->CollisionManager()->FindEntry(aLocalSsrc);
		__ASSERT_DEBUG(localEntry, Panic(ERtpSourceEntryNull ));
		packet.WriteToPacket(*localEntry, iRtpController->CurrentRtpTimeStampL(), aLocalSsrc, entryCount);
		}
	else
		{
		__RTP_LOG(_L("\tWriteReport - RR Header"));
		TRtcpRRPacket packet(rPtr);
		// write the first RR block (the only one in this case)
		packet.WriteToPacket(aLocalSsrc, entryCount);
		}
	// add the length of the new descriptor to the old one
	aBuf.SetLength(oldLen + rPtr.Length());
	__RTP_LOG1(_L("\tWriteReport - New Length %d"), aBuf.Length());
	
	// Add the receiver reports for all participants, except ours
	for (TInt count = 0; count < iRtpController->CollisionManager()->GetNumEntries(); count++)
		{
		CRtpSourceEntry& entry = iRtpController->CollisionManager()->GetEntry(count);
		if ((entry.SRC() != aLocalSsrc) && (entry.State() == ERtpValid))
			{
			__RTP_LOG1(_L("\tWriteReport - RR for SSRC %u"), entry.SRC());
			oldLen = aBuf.Length();
			rPtr.Set(const_cast<TUint8*>(aBuf.Ptr() + oldLen), KRtcpRRBlockSize, aBuf.MaxLength() - oldLen);
			TRtcpPacket::WriteReportBlock(rPtr, entry);
			aBuf.SetLength(oldLen + rPtr.Length());
			}
		}
	__RTP_LOG1(_L("\tWriteReport - New Length %d"), aBuf.Length());
	}


void CRtcpSessionData::SetSDESFlags(TUint8 aFlags)
	{
	iSDESFlags= aFlags;
	}
	
/*
 *	Clear the array of the items that are not needed anymore
 */
void ClearItems(TSdesItemArray& aArray)
	{	
	CSdesItem* item = NULL;
	for(TInt count = 0; count < aArray.Count(); count++)
		{
		// Extract the first item in the array
		item = aArray[count];
		if((item->iItemType != EsdesCNAME) && (item->iItemType != EsdesNONE))
			{
			aArray.Remove(count);
			delete item;
			}
		}		
	}
	
