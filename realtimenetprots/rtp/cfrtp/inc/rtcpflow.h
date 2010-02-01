// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RTCP Flow Definitions.
// 
//

/**
 @file
 @internalComponent
*/


#ifndef CRTCPFLOW_H
#define CRTCPFLOW_H

#include "rtpprovisioninfo.h"
#include "rtpbaseflow.h"
#include "rtpcontroller.h"


const TInt KRtcpDefBufSz = 1500;

NONSHARABLE_CLASS(CRtcpFlow) : public CRtpBaseFlow
	{
	/** CRtcpFlow implements the RTCP Flow.

	@internalComponent 
	
	**/				    

	friend class CRtpFlowFactory;	
	
	public:
	static CRtcpFlow* NewL(CSubConnectionFlowFactoryBase& aFactory, 
	                       const Messages::TNodeId& aSubConn,  
	                       CProtocolIntfBase* aProtocolIntf);

	public:
	
	virtual TInt  Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr = NULL );
	virtual TInt GetData(RMBufChain& aData, TUint aLength,TUint aOptions,TSockAddr* anAddr=NULL);
	
	/* From MRtpDataSender */
	virtual void SendData(TDes8& aBuffer,TSockAddr& aRemoteAddr, TInt aFlags, TRequestStatus& aStatus);
	
	void SetRtpSession(CRtpController *apRtpSession);
	
	public:
	
	//CSubConnectionFlowBase	
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	virtual void NewData(TUint aCount);
	virtual void CanSend();
	
	virtual void DoUpdateLocalAddress(const TSockAddr& anAddr);
	
	virtual ~CRtcpFlow();
	
	protected:
	CRtcpFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);
	void ConstructL();
	/* Worker for Provision */
	void DoProvisionL(const CRtpProvisionConfig *apRtpProv);
	
	void DoCanClose(MSessionControlNotify::TDelete aDelete);
	
	void HandleWriteBlocked(TSockAddr& aAddr,TDes8& aBuffer);

	virtual TInt DoSetOption(TUint level, TUint name, const TDesC8 &anOption);
	virtual TInt DoGetOption(TUint level, TUint name, TDes8 &anOption) const;
	
	void SetNonRtcpSendData(TBool aNonRtcpData);
 	TBool NonRtcpSendData();
	/* RTP Session Pointer */
	CRtpController *iRtpSession;
	
	/* This buffer is used to pass data to rtp Stack.*/
	RBuf8 iRtcpBuffer;
	
	/* If the UDP flows us off we will buffer the current packet. If another
	   report comes in during this interval the buffered data will be updated
	   with the new report. This will be send out in CanSend(). */
	TBool iBlockedWrite;
	
	/* Temp buffers */
	RMBufChain iRtcpDataChain;
	RBuf8  iRtcpTempBuffer;
	// Non Rtcp Data sending flag
	TBool iNonRtcpSendData;
	
	/* Handler For RTCP Flow */
	CCFRtpHandler *iRtcpHandler;	
};



#endif // CRTCPFLOW_H

