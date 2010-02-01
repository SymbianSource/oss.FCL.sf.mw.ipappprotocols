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
// class for handling rtcp sending sequence
// 
//

/**
 @file
 @internalComponent
*/

#include "rtcpsender.h"
#include "rtpcontroller.h"
#include "rtputils.h"
#include "rtpcollisionmng.h"

/** 
@param aServer An opened socket server session
*/	
CRtcpSender* CRtcpSender::NewL(CRtpHandlerBase* aHandler, 
							   CRtpCollisionMng* aCollisionMgr, TInt aPriority)
	{
	__RTP_LOG(_L("CRtcpSender::NewL() ... "));
	CRtcpSender* self = new(ELeave) CRtcpSender(aPriority, aCollisionMgr);
	__ASSERT_ALWAYS(aHandler,Panic(ERtpHandlerIsNotCreated));
	self->iHandler = aHandler;
	return self;
	}

CRtcpSender::~CRtcpSender()
	{
	__RTP_LOG(_L("CRtcpSender::~CRtcpSender()"));
	Cancel();
	}

void CRtcpSender::QueueAndSendMessageL(HBufC8* aPacket)
	{	
	__RTP_LOG(_L("CRtcpSender::QueueAndSendMessageL() ... "));

	// If the last send hasn't completed yet, cancel it and send the next one
	if(IsActive())
		{
		__RTP_LOG(_L("\tCanceling old send "));
		Cancel();
		}
	// we're ready to send straight away...
	// Reset the index
	iEntryIndex = 0;
	iLoop =0;
	iDataPtr.Set(aPacket->Des());
	SendToNextAddr();
	}

void CRtcpSender::SendToNextAddr()
	{	
	TSockAddr remote;
	TInt err = GetNextRemoteAddress(remote);
	if (err != KErrNotFound)
		{
		iHandler->SetRemoteAddress(remote);
		iHandler->Send(iDataPtr,iStatus);
		SetActive();
		}
		iLoop++;
	if (err == KErrNotFound && iLoop == 1)
		{
		remote = iHandler->DefaultRemoteAddress();
		iHandler->SetRemoteAddress(remote);
		iHandler->Send(iDataPtr,iStatus);
		SetActive();
		}
			
	}
		
TInt CRtcpSender::GetNextRemoteAddress(TSockAddr& aAddr)
	{	
	TUint localSsrc = iCollisionManager->LocalSSRC();
	const TInt count = iCollisionManager->GetNumEntries();
	// get the source at index
	for (; iEntryIndex < count; iEntryIndex++)
		{
		CRtpSourceEntry& entry = iCollisionManager->GetEntry(iEntryIndex);
		if ((entry.SRC() != localSsrc) && (entry.State() == ERtpValid) && (KAFUnspec != entry.RtcpSourceAddr().Family()))
			{
			// If we got here, we have found something
			aAddr = entry.RtcpSourceAddr();
			while(++iEntryIndex < count)
				{
				if (iCollisionManager->GetEntry(iEntryIndex).RtcpSourceAddr() != aAddr)
					{
					break;
					}
				}
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

CRtcpSender::CRtcpSender(TInt aPriority, CRtpCollisionMng* aCollisionMgr) : 
	CActive(aPriority),iDataPtr(NULL,0,0), iCollisionManager(aCollisionMgr)
	{
	CActiveScheduler::Add(this);
	iLoop =0;
	}

void CRtcpSender::DoCancel()
	{
	__RTP_LOG(_L("CRtcpSender::DoCancel() ... "));
	iHandler->CancelSend();
	}

void CRtcpSender::RunL()
	{
	__RTP_LOG(_L("CRtcpSender::RunL() ... "));
	__RTP_LOG1(_L("\tiStatus == %d"), iStatus.Int());
	SendToNextAddr();
	}

TInt CRtcpSender::RunError(TInt aError)
	{	
	/* so we failed when sending a packet
	and the RunL leave when trying to resend... 
	thus reset the queue! A bit brutal? 
	RTP is unreliable protocol after all so shoul be fine besides the queue is unlikely to hold 
	more then a few RR/SR and may be some BYE's...
	What about lost RTCP packets? Should we indicate to the client code above?? */
	__RTP_LOG(_L("CRtcpSender::RunError() ... "));
	__RTP_LOG1(_L("\t[Error] == %d"), aError);	
	(void)aError;
	return KErrNone;
	}
	
