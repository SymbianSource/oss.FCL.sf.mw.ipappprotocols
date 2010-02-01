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
// Header for class specific to RTP session handling
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef SESSION_H
#define SESSION_H

#include <es_sock.h>
#include <in_sock.h>
#include "events.h"
//#include "rtpcontroller.h"
#include "rtpcontrollerbase.h"
#include "rtcp.h"
#include "rtcppacket.h"

class TRtpReceivePacket;
class TReceiveSourceRegistration;
class TRtpSendPacket;
class CRtpController;
class CRtpControllerV2;

class CRtcpListener;

NONSHARABLE_CLASS(CRtpSession) : public CBase, public MEventSource, MRtpDataObserver
	{
	friend class RRtpSession;
	friend class RRtpSession_Internal;
	friend class RRtpSendSource;
	friend class CRtcpListener;
 public:
 	static CRtpSession* NewL(RSocketServ& aServer, TSockAddr& aLocalAddr, 
							 TSockAddr& aRemoteAddr, RConnection& aConnection,
							 TInt aPriority, TUint aRxSize, const TDesC8& aCNAME);
	static CRtpSession* NewL(RSocketServ& aServer, TSockAddr& aLocalAddr, 
							 TSockAddr& aRemoteAddr, TInt aPriority, 
							 TUint aRxSize, const TDesC8& aCNAME);
	static CRtpSession* NewL(RSocket& aSocket, TUint aMaxRXSize,
							   RSocket& aRtcpSocket,
							   TInt aPriority,
							   const TDesC8& aCNAME);
	static CRtpSession* NewL(RSocket& aSocket, TUint aMaxRXSize, 
							 TInt aPriority);
							 
	static CRtpSession* NewL(RSocketServ& aServer, TSockAddr& aLocalAddr, 
							 TSockAddr& aRemoteAddr, RSubConnection& asubConnection,
							 TInt aPriority, TUint aRxSize, const TDesC8& aCNAME);
						 

	virtual ~CRtpSession();
	void Close();

	void WaitForPacketL(TBool aDoAllocate = ETrue);
	void RemoveRXSource(const CRtpReceiveSource* aSource);
	inline void SendSourceClosing();
	void Send(TRtpSendPacket* aPacket, TRequestStatus& aStatus);
	void CancelSend();
	inline void SetErrorHandler(MEventSource* aHandler);
	TInt GetSDES(const TInt aType, TDes8& aValue);

	// From MEventSource
	virtual void HandleErrorL(TInt aError);
	virtual void CloseNow();
	virtual void CloseSession();
	virtual TInt WhatShouldClose();

	virtual TInt HandleError(TInt aError);
	virtual void CancelAllRequests();
	
	const TDesC8& NonRtpDataL();
	TPtr8 NonRtcpDataL();
	void SetRemoteRtcpPort(TUint aPort);
	TInt RtcpStatistics(TUint32 aSsrc, TRtcpStatistics &aRtcpStatistics);
	RSocket* RtpSocket();
    RSocket* RtcpSocket();
    CRtpControllerBase* RtpController();
    void SetRemoteAddress(const TSockAddr& aRemoteAddr);
	void SendDataL(TBool aSocketType, const TDesC8& aData, TRequestStatus&  aStatus);
	TInt SetSamplingRate(TInt aPayloadType, TUint aSamplingRate);
	void DataAvailable(TInt aErrorCode);

 private:
	inline CRtpSession(TInt aPriority, TUint aRxSize);
	inline void ConstructL(RSocketServ& aServer, TSockAddr& aLocalAddr,
						   TSockAddr& aRemotelAddr, RConnection& aConnection, const TDesC8& aCNAME);
	inline void ConstructL(RSocketServ& aServer, TSockAddr& aLocalAddr,
						   TSockAddr& aRemotelAddr, const TDesC8& aCNAME);
	inline void ConstructL(RSocket& aSocket, 
							RSocket& aRtcpSocket, 
							const TDesC8& aCNAME);
	inline void ConstructL(RSocket& aSocket);
	inline TInt Priority();
	
	
	
	// Version 2
	void ConstructL(RSubConnection& aSubConn,RSocket& aSocket);
	void ConstructL(RSubConnection& aSubConn,RSocket& aSocket, 
							RSocket& aRtcpSocket, 
							const TDesC8& aCNAME);
	void ConstructL(RSocketServ& aServer, TSockAddr& aLocalAddr,
						   TSockAddr& aRemotelAddr, RSubConnection& aConnection, const TDesC8& aCNAME);
	
						
	TBool CheckForClose();
	
	CRtpReceiveSource* FindSource(const TUint aSsrc) const;
	
	void GotRtcpReportPacketL(const TUint aPacketType, const TUint aSenderSsrc, TDesC8& aPtr, const TUint aCount);
	void GotRtcpAppPacketL(const TUint aSenderSsrc, TDesC8& aPtr, TUint aSubType);
	void GotRtcpByePacketL(const TUint aSenderSsrc, TDesC8& aPtr);
	void GotRtcpSdesPacketL(const TUint aSenderSsrc, TDesC8& aPtr, const TUint aChunkCount);
	
	void NotifyRtcpReceivedL(TRtpEventType aEvent, TInt aError);
	
	void DoProcessRtpDataL(TInt aErrorCode);

 private:
	// RTP Controller - the main access main for RTP handler/socket
	//CRtpController* iRtpSocketController;
	CRtpControllerBase* iRtpSocketController;
	HBufC8* iCNAME; // THe CNAME of this client
	TInt iRxSize; // The maximum size of a received buffer
	TRtpReceivePacket* iWaitingPacket; // The packet that is currently being received
	TRtpReceivePacket* iNewSourcePacket; // A packet that needs a stream to be created for it
	REventModel iEventModel; 
	RArray<TReceiveSourceRegistration> iReceiveSources;
	CRtpReceiveSource* iNewSource; // A receive stream that has just been created
	MEventSource* iErrorHandler;
	CRtpSendSource* iSendSource;
	TSockAddr iRemoteAddr;
	CRtcpListener* iRtcpListener;
	//non RTP data
	TRtpReceivePacket* iNonRtpData;
	TRtcpRRPart iLastRRPart;
	TRtcpSdesPart iLastSdesPart;
	TInt iPriority;
	};

class TReceiveSourceRegistration
	{
 public:
	TUint iSSRC;
	CRtpReceiveSource* iSource;
	};

inline CRtpSession::CRtpSession(TInt aPriority, TUint aRxSize)
		: iRxSize(aRxSize), iPriority(aPriority)
	{
	iErrorHandler = this;
	}

inline void CRtpSession::SendSourceClosing()
	{
	iSendSource = NULL;
	}

inline void CRtpSession::SetErrorHandler(MEventSource* aHandler) 
	{ 
	iErrorHandler = aHandler;
	};

inline TInt CRtpSession::Priority()
    {
    return iPriority;
    }

#endif // SESSION_H

