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
// Header for class specifying inteface for rtp handling
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__RTP_BASE_HANDLER__)
#define __RTP_BASE_HANDLER__

#include <es_sock.h>
#include "rtpsock.h"
#include "rtpcontrollerbase.h"

class MRtpDataObserver
    {
public:
    /* Notifies that some data is available for reading.
     * The aErrorCode is the result of the parsing done internally for the
     * data. Please check the error code to ensure that the data is properly
     * constructed before handling it.
     * The data would have been copied to the buffer passed to the handler when
     * Read() was called.
     */
    virtual void DataAvailable(TInt aErrorCode)=0;
    };

/** Base class for RTP handlers.
This is an accessor for the RTP socket services.
It provides a direct access to the RtpSocket and offers additional facilities 
such as socket activation, a common interface for both connection-less and connection-mode service etc.

The handler (through RtpSocket) deals with generating SSRC, sequence numbers, 
generating RTCP packets (for RTCP connection).

The class always represents a single socket.

The Send and Read methods assume the handler is 'active', 
and will panic in debug if it isn't.

@internalTechnology
*/
class CRtpHandlerBase : public CActive, public MSockAddrHelper
	{
	friend class CRtpController;
	friend class CRtpControllerBase;
	
	friend class CRtpCollisionMng;
public:
	static CRtpHandlerBase* NewBearerL(CRtpControllerBase* aController,
										RSocket& aSocket, TInt aPriority = EPriorityNormal);
	static CRtpHandlerBase* NewBearerL(CRtpControllerBase* aController,
										TSockAddr aLocalAddr, TInt aPriority = EPriorityNormal);

	IMPORT_C virtual ~CRtpHandlerBase();
	/** Send a RT(C)P packet. The packet must be already allocated, including the header.
	Some of the header fields will be updated by RTP socket, @see RRtpSocket
	This is a common interface for both connection-less and connection mode services.
	For CL services previously set remote address is used for this operation
	
	The returned packet is not owned by the handler.

	@param aBuffer The RT(C)P header including pre-allocated header
	@param aStatus The completion status of this request.
	@pre For connection-less mode the remote address must be set.
	@post On completion aStatus will contain an error code.
	*/
	virtual void Send(TDes8& aBuffer, TRequestStatus& aStatus) =0;

	/** Receive a RT(C)P data from a remote host. 
	The buffer must be sufficiently big for the packet, including the header.
	The function completes when one datagram arrives - even if it is not sufficient to fill the buffer. 
	If the datagram does not fit in the buffer supplied then the remaining data will be lost.
	
	Only valid packets are received, @see RRtpSocket
	
	This is a common interface for both connection-less and connection mode services.
	For CL services previously set remote address is used for this operation.
		
	The packet is not owned by the handler.
	
	@param aBuffer A descriptor where data received will be placed.
	@param aStatus The completion status of this request.
	@pre For connection-less mode the remote address must be set.
	@post On completion aStatus will contain an error code.
	*/
	virtual void Read(TDes8& aBuffer) =0;

	/** Activate  the handler according to type of the connection. 
	E.g for CL service it will bind to a local address, 
	it will connect  to remote host for outgoing CO service,
	and it will listen for incoming connection for incoming CO service.
	
	This is a common interface for both connection-less and connection mode services.
	
	@param aConnectionType The type of connection required, @see TRtpConnectionType
	@leave 
	@pre For connection-less mode the remote address must be set.
	@post On completion aStatus will contain an error code.
	*/
	virtual void ActivateL(RSocketServ& aServer, TRtpConnectionType aConnectionType) = 0;
	virtual void ActivateL(RSocketServ& aServer, TRtpConnectionType aConnectionType,RConnection& aConnection) = 0;
	virtual void ActivateL(RSocketServ& aServer, TRtpConnectionType aConnectionType,RSubConnection& aSubConnection) = 0;

	virtual void CancelSend();
	virtual void CancelRecv();

	virtual void SetLocalAddress(const TSockAddr& aAddr) = 0;
	virtual void SetRemoteAddress(const TSockAddr& aAddr) = 0;
	virtual TSockAddr& RemoteAddress() = 0;
	virtual TSockAddr& LocalAddress() = 0;
	virtual TSockAddr& RecvAddress() = 0;
	virtual void SetDefaultRemoteAddress(const TSockAddr& aAddr)=0;
	virtual TSockAddr& DefaultRemoteAddress()=0;
    
	//From MSockAddrHelper
	virtual TBool CompareAddresses(const TSockAddr& aAddr1,const TSockAddr& aAddr2) = 0;
	
	IMPORT_C virtual void SetDataObserver(MRtpDataObserver *aDataObserver);

	inline RRtpSocket& RtpSocket();
	
	inline void SetType(TRtpType aType);
	inline TRtpType Type();

	void DoReceiveTag(TBool aTag);
	
	virtual void ReadMore() =0;
	IMPORT_C virtual void SaveReadStatus(TDes8& aData);
	IMPORT_C virtual void CompleteRead(const TInt aError);
protected:
	/**
	Types of parity codes for forward error correction (FEC); defined in RFC2733 
	A parity code is a particular combination of data packets used to generate FEC packets.
	*/
	enum TRtpFecParityCodes
		{
		/** A basic scheme as defined in RFC2733, Section 4 */
		EFecScheme0,
		/** The Scheme 1 as defined in RFC2733, Section 4 */
		EFecScheme1,
		/** The Scheme 2 as defined in RFC2733, Section 4 */
		EFecScheme2,
		/** The Scheme 3 as defined in RFC2733, Section 4 */
		EFecScheme3,
		/** The Scheme 4 as defined in RFC2733, Section 4 */
		EFecScheme4
		};

	IMPORT_C CRtpHandlerBase(TInt aPriority = EPriorityNormal);
	IMPORT_C CRtpHandlerBase(RSocket& aSocket,  TInt aPriority = EPriorityNormal);
	
	// from CActive
	 IMPORT_C virtual void RunL();
	 IMPORT_C virtual void DoCancel();
	 IMPORT_C virtual TInt RunError(TInt aError);
	//

	/*inline*/ void PacketBeingSentL();

protected:
	// These options are used to check if we want to receive these packets
	TBool iRecvOn;

public:
	/** A generic RT(C)P socket, could represent either */
	RRtpSocket iRtpSocket;
	
	/** Reference to Clients data buffer, contains data after the runL completes
	The client is always the user in case of rtp, but it can be sessiondata incase of rtcp
	*/
	TDes8* iClientDataPtr;

	/** Link back to owning controller */
	CRtpControllerBase* iRtpController;
	/**
	Client status - saved and updated when appropriate operations 
	in RTP socket is finished
	*/
	TRequestStatus* iReadStatus;
	MRtpDataObserver* iDataObserver;
	};

inline RRtpSocket& CRtpHandlerBase::RtpSocket()
	{
	//__ASSERT_ALWAYS(((!iRtpSocket.iOwnSocket)||(iRtpSocket.iOpenSocket)), Panic(ERtpSocketNotReady)); No... some methods accesble anyway, e.g. need to be able do Open!!
	return iRtpSocket;
	}

inline void CRtpHandlerBase::SetType(TRtpType aType)
	{
	iRtpSocket.iType = aType;
	}

inline TRtpType CRtpHandlerBase::Type()
	{
	return iRtpSocket.iType;
	}

#endif
