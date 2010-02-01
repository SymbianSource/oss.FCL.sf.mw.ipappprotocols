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
// Header for class specific to RTCP handling
// 
//

/**
 @file
 @internalComponent
*/
 
#if !defined(RTCP_SENDER_H__)
#define RTCP_SENDER_H__

#include <e32base.h>
#include <badesca.h>
#include "rtpsock.h"
#include "rtputils.h"
//
class CRtpCollisionMng;

NONSHARABLE_CLASS( CRtcpSender) : public CActive
	{
	friend class CRtpController;
public:
	static CRtcpSender* NewL(CRtpHandlerBase* aHandler, CRtpCollisionMng* aCollisionMgr, 
		TInt aPriority = EPriorityNormal);
	virtual ~CRtcpSender();

	void QueueAndSendMessageL(HBufC8* aPacket);

private:
	CRtcpSender(TInt aPriority, CRtpCollisionMng* aCollisionMgr);
	// from CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	TInt GetNextRemoteAddress(TSockAddr& aAddr);
	void SendToNextAddr();
private:
	//RTCP handler
	TPtr8 iDataPtr;
	CRtpHandlerBase* iHandler;
	CRtpCollisionMng* iCollisionManager;
	TRTPTime iRtpTime;
	TInt iEntryIndex;
	TInt iLoop;
	};

#endif

