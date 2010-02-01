/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : MTransport.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CTRANSPORTBASE_H__
#define __CTRANSPORTBASE_H__

#include <e32base.h>
#include <e32std.h>
#include <e32def.h>
#include <in_sock.h>
#include "Lwtimer.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipmessage.h"
#include "sipclient.h"
#include "sipconnection.h"
#include "CSipConnectionMgr.h"
#include "SipStackServerDefs.h"
#include "CSender.h"

// FORWARD DECLARATIONS
class MSocketUsagePermissionObserver;
class MSIPNATBindingObserver;
class MSIPTransportRemovalObserver;

// CLASS DEFINITION
//
class CTransportBase :  public CBase
	{
public:
	virtual ~CTransportBase() {}
	virtual CSender* Sender(const TSIPTransportParams& aParams,
	                        TUint aProtocol,
	                        const TInetAddr& aRemoteAddr) = 0;
	virtual TUint ProtocolType() = 0;
	virtual TBool HandleL(const TSIPTransportParams& aParams,
	                      CSIPMessage* aMessage, 
	                      TTransactionId aId,
	                      TBool aIsStrict) = 0;
	virtual TBool HandleL(const TSIPTransportParams& aParams,
	                      CSIPMessage* aMessage, 
	                      const TInetAddr& aAddr,
	                      TUint aLocalPort,
	                      TBool aIsStrict) = 0;
	virtual TBool HandleMessage(const TSIPTransportParams& aParams,
	                            RStringF aProtocol,
							 	const TInetAddr& aRemoteAddr,
							 	TUint aLocalPort,
							 	TBool aIsStrict) = 0;
	virtual TBool Handle(TUint32 aTransportId) = 0;
	virtual TBool Match(const TSIPTransportParams& aParams) = 0;
	virtual void WaitL() = 0;
	virtual TBool IsWaiting(TUint aProtocol, TUint aPort) = 0;
	virtual void Send(const TSIPTransportParams& aParams,
	                  CSIPResponse& aResponse,
					  TTransactionId aId,
					  TRequestStatus &aStatus) = 0;
	virtual void Send(const TSIPTransportParams& aParams,
	                  CSIPResponse& aResponse,
					  const TInetAddr& aAddr,
					  TRequestStatus &aStatus) = 0;
	virtual void Send(const TSIPTransportParams& aParams,
	                  CSIPRequest& aRequest,
	                  TBool aForceUDP,
					  const TInetAddr& aAddr,
					  TUint aOrigTransport,
					  TRequestStatus &aStatus,
					  TBool aPublicAddrResolved) = 0;
	virtual TBool CancelSend(TRequestStatus& aStatus) = 0;
	virtual TBool CancelResponseSend(TTransactionId aId, 
	                                 TBool aCancelAlso2xxResponses) = 0;
	virtual void CancelAllRequests(CSIPConnection::TState aReason) = 0;
	virtual void GetAddrFromResponseL(const TSIPTransportParams& aParams,
	                                  CSIPMessage& aMessage,
									  TInetAddr& aAddr) = 0;
	virtual void ICMPErrorL(CSipConnectionMgr::TICMPError aICMPError,
							const TInetAddr& aSourceAddr) = 0;
	virtual TBool HasRemoteHost(const TSIPTransportParams& aParams,
	                            const TInetAddr& aRemoteHost,
	                            RStringF aProtocol) = 0;
	virtual void Disconnect(const TInetAddr& aRemoteHost) = 0;	

    virtual TBool Remove(const TSIPTransportParams& aParams) = 0;
    virtual TBool Remove(TUint32 aTransportId) = 0;
    virtual TBool Remove(MSIPNATBindingObserver& aSIPNATBindingObserver) = 0;
    
    virtual TBool Shutdown(TUint32 aTransportId,
                           MSIPTransportRemovalObserver* aRemovalObserver) = 0;
    
    virtual TBool AddUserL(const TSIPTransportParams& aParams, 
                           TUint aReceivePort) = 0;
                           
    virtual TBool IsReservedTransport() const = 0;
    
    virtual RSocket* AcquireUdpSocket( MSocketUsagePermissionObserver* aObserver,
                                       const TInetAddr& aRemoteAddr,
                                       TBool& aPermissionToUse ) = 0;
                                        
    virtual void UdpSocketReleased( MSocketUsagePermissionObserver* aObserver,
                                    RSocket& aSocket ) = 0; 
    
    virtual TInetAddr* PersistentRemoteAddr( 
                            MSIPNATBindingObserver* aBindingObserver ) = 0;
    
	static const TInt iOffset;
	TSglQueLink iLink;
	};

#endif // end of __CTRANSPORTBASE_H__

// End of File
