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
// Header for active object class specific to send source
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef SENDSTREAM_H
#define SENDSTREAM_H

#include "events.h"
class TRtpSendPacket;
const TInt KRTPDefaultAlignment = 1;

NONSHARABLE_CLASS(CRtpSendSource) : public CActive, public MEventSource
	{
 public:
	inline CRtpSendSource(TInt aPriority, CRtpSession* aSession);
	void Close();
	virtual ~CRtpSendSource();
	
	void Send(TRtpSendPacket* aPacket);
	void Send(TRtpSendPacket* aPacket, TRequestStatus &aStatus);
	TBool ShouldPacketBeClosed(TAny *aPtr, TUint aLength);

	// from MEventSource
	virtual void HandleErrorL(TInt aError);
	virtual void CloseNow();
	virtual void CloseSession();
	virtual TInt WhatShouldClose();

 protected:
	// From CActive
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();

 private:
	friend class RRtpSendSource;

	REventModel iEventModel;
	CRtpSession* iSession;
	TUint iPayloadType;
	TUint iPayloadSize;
	TInt iAlignment;
	TRtpSendPacket* iLastSentPacket;
	TUint16 iSeq;
	TAny *iBufferedPkt;
	TUint iBuffPktLen;
	};

inline CRtpSendSource::CRtpSendSource(TInt aPriority, CRtpSession* aSession)
		: CActive(aPriority), iSession(aSession), iAlignment(KRTPDefaultAlignment)
	{
	CActiveScheduler::Add(this);
	}

#endif // SENDSTREAM_H

