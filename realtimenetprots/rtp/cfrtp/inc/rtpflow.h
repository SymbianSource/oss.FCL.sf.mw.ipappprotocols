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
// RTP Flow definitions.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef CRTPFLOW_H
#define CRTPFLOW_H

#include "rtpprovisioninfo.h"
#include "rtpbaseflow.h"
#include "rtpcontroller.h"


const TInt KRtpDefBufSz = 1500;

NONSHARABLE_CLASS(CRtpFlow) : public CRtpBaseFlow 
	{
	/** CRtpFlow implements the RTP Flow.

	@internalComponent 
	
	**/				    

	friend class CRtpFlowFactory;	
	
	public:
	static CRtpFlow* NewL(CSubConnectionFlowFactoryBase& aFactory, 
	                       const Messages::TNodeId& aSubConn,  
	                       CProtocolIntfBase* aProtocolIntf);

	public:
	//CSubConnectionFlowBase
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	
	virtual TInt  Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr = NULL );
	virtual TInt GetData(RMBufChain& aData, TUint aLength,TUint aOptions,TSockAddr* anAddr=NULL);
	
	/*MRtpDataSender*/
	void SendData(TDes8& aBuffer, TSockAddr &aAddr, TInt aFlags, TRequestStatus  &aStatus);
	
	public:
	virtual void NewData(TUint aCount);
	virtual void CanSend();
	
	virtual void DoCanClose(MSessionControlNotify::TDelete aDelete);
	
	virtual void DoUpdateLocalAddress(const TSockAddr& anAddr);

	virtual TInt DoSetOption(TUint level, TUint name, const TDesC8 &anOption);
	virtual TInt DoGetOption(TUint level, TUint name, TDes8 &anOption) const;
	
	/* When an RTCP flow is created call this function to enable RTCP in
	   RtpSession. The function will return a Pointer to RTPSession which
	   can be passed on to RTCP Flow */
	CRtpController* GetRtpSession() const;
	
	void SetNonRtpSendData(TInt aNonRtpData);
 	TInt NonRtpSendData();
 		
	virtual ~CRtpFlow(); 
	
	protected:
	CRtpFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);
	/* RTP Session Pointer */
	CRtpController *iRtpSession;
	void ConstructL();
	/* Worker for Provision */
	void DoProvisionL(const CRtpProvisionConfig *apRtpProv);
	/* This buffer is used to pass data to rtp Stack.*/
	RBuf8 iRtpBuffer;
	// Non Rtp Data sending flag
	TInt iNonRtpSendData;
	
	/* Handlet For RTP Flow */
	CCFRtpHandler *iRtpHandler;	
};




#endif // CRTPFLOW_H


