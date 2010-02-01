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
// Header for class handling RTP incoming streams
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef RECEIVESTREAM_H
#define RECEIVESTREAM_H

class TRtpReceivePacket;

#include "rtcp.h"
#include "rtcppacket.h"

GLREF_C TInt GetSDESPart(const TInt aType, TDes8& aValue, TRtcpSdesPart& aLastSdesPart);

NONSHARABLE_CLASS(CRtpReceiveSource) : public CBase, public MEventSource
	{
public:
	inline CRtpReceiveSource(TRtpReceivePacket* aPacket,CRtpSession* aSession);
	virtual ~CRtpReceiveSource();

	// This always takes ownership of the packet, even when it leaves.
	void NewPacketL(TRtpReceivePacket* aPacket, 
					MEventSource*& aErrorCallback);
	void ActivateL(MEventSource*& aErrorCallback);
	void Close();
	void NotifyRtcpReceivedL(TRtpEventType aEvent, TInt aError);
	
	virtual void HandleErrorL(TInt aError);
	virtual void CloseNow();
	virtual void CloseSession();
	virtual TInt WhatShouldClose();
	
private:

	friend class RRtpReceiveSource;
	friend class CRtpSession;

	void NotifyPacketReceivedL();
	void GetLastApp(TPtrC8& aName, TPtrC8& aAppData, TUint& aSubType);

	void SetLastAppL(TDesC8& aName, TDesC8& aAppData, TUint& aSubType);
	void SetLastByeL(TDesC8& aBye);
	void SetLastSR(TDesC8& aReport, const TUint aSsrc);
	
	REventModel iEventModel;
	TRtpReceivePacket* iNewPacket;
	CRtpSession* iSession;
	TUint iPrevPayloadType;
	TUint iSSRC;				// The ssrc of the remote end

	
	// A Buffer each for all the RTCP packet types
	TRtcpSdesPart		iLastSdesPart;
	TRtcpAppPart		iAppPart;
	TRtcpSRPart			iLastSRPart;
	HBufC8*				iLastByeReason;
	};

inline CRtpReceiveSource::CRtpReceiveSource(TRtpReceivePacket* aPacket,
											CRtpSession* aSession)
		: iNewPacket(aPacket), iSession(aSession), 
		iSSRC(aPacket->SSRC())
	{
	};

#endif // __RECEIVESTREAM_H__

