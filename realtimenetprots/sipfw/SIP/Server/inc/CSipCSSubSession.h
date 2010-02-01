/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipCSSubSession.h
* Part of       : SIPServerCore
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCSSUBSESSION_H
#define CSIPCSSUBSESSION_H

#include <e32base.h>
#include <stringpool.h>
#include <in_sock.h>
#include "SipStackServerDefs.h"
#include "sipconnection.h"
#include "TSIPTransportParams.h"
#include "MSIPCSReceiverOwner.h"
#include "_sipcodecdefs.h"

class CSipCSSession;
class MSIPSecUser;
class MTransactionOwner;
class CSipServerCore;
class CSipCSServerITC;
class CSipCSReceiverBase;
class CSipCSSubSessionReceiver;
class CSIPFromHeader;
class CURIContainer;
class CSIPMessage;
class CSIPRequest;
class TSIPRemoteTargetAndProxy;


class CSipCSSubSession : public CObject,
                         public MSIPCSReceiverOwner
	{
public: // Constructors and desctructor
    
    static CSipCSSubSession* NewLC (CSipCSSession& aSession,
                                    MSIPSecUser& aSIPSecUser,
		                            TUint32 aIapId);
    ~CSipCSSubSession();

public: // From MSIPCSReceiverOwner

    const TSIPTransportParams& TransportParams() const;

    TSIPTransportParams TransportParams(
        const TRegistrationId& aRegistrationId);
        
    const MSIPSecUser* SIPSecUser(const TRegistrationId& aRegistrationId);

public: // New functions

    TUint32 IapId () const;
    MTransactionOwner* TransactionOwner();
    CSipCSReceiverBase& Receiver();
    void OpenIapL(const RMessage2& aMessage);
    void SetStateL(CSIPConnection::TState aState);
    void ServiceL(TSIPIds& aIds, TInt aFunction, const RMessage2& aMessage);

private: // New functions

    CSipCSSubSession (CSipCSSession& aSession,
                      MSIPSecUser& aSIPSecUser,
		              TUint32 aIapId);

    void ConstructL ();

    // Services
    void GetLocalAddrL(const RMessage2& aMessage);
    
    void SetSIPSockOptL(const RMessage2& aMessage, TBool aDesOpt=EFalse);
    
	void SetOutboundProxyL (TSIPIds& aIds, const RMessage2& aMessage);    
    
	void OutboundProxyL (TSIPIds& aIds, const RMessage2& aMessage);

	void RemoveOutboundProxyL (TSIPIds& aIds);
	
    void RegisterL (TSIPIds& aIds, const RMessage2& aMessage);
	
    void RegisterAndSetOutboundProxyL(TSIPIds& aIds, const RMessage2& aMessage);
	
    void UpdateRegistrationL (TSIPIds& aIds, const RMessage2& aMessage);
	
    void UnRegisterL (TSIPIds& aIds, const RMessage2& aMessage);
	
    void FetchRegistrationsL (const RMessage2& aMessage);
    
    void SendRequestL (TSIPIds& aIds, const RMessage2& aMessage);
	
    void SendRequestAndUpdateRefreshL (TSIPIds& aIds,
                                       const RMessage2& aMessage);

    void SendRequestAndTerminateRefreshL (TSIPIds& aIds,
                                          const RMessage2& aMessage);
    
    void SendResponseL (TSIPIds& aIds, const RMessage2& aMessage);

    void SendResponseAndCreateDialogL (TSIPIds& aIds,
                                       const RMessage2& aMessage);

    void SendResponseInDialogL (TSIPIds& aIds, const RMessage2& aMessage);

    void SendRequestAndCreateDialogL (TSIPIds& aIds, const RMessage2& aMessage);

    void SendRequestInDialogL (TSIPIds& aIds, const RMessage2& aMessage);

    void SendRequestInDialogAndUpdateRefreshL (TSIPIds& aIds,
                                               const RMessage2& aMessage);
                                               
    void SendCancelL (TSIPIds& aIds,
                      const RMessage2& aMessage);

	void TerminateDialogL (const TDialogId& aId);

	void TerminateRefreshL (const TRefreshId& aId);

	void TerminateDialogRefreshL (const TDialogId& aDialogId,
                                  const TRefreshId& aRefreshId);

	void TerminateRegistrationL (const TRegistrationId& aId);
    
    void ReceiveSipMessageL (const RMessage2& aMessage);

    void ClientReadyToReceiveL (const RMessage2& aMessage);
    
    void CancelClientReceiveL ();

    void GetFromHeaderL (const TRegistrationId& aId,
                         const RMessage2& aMessage);
    
    void SetRefreshIntervalL (const TRefreshId& aRefreshId, 
                              const RMessage2& aMessage);

    void GetRefreshIntervalL (const TRefreshId& aRefreshId,
                              const RMessage2& aMessage);
                              
    void GetCallIDHeaderL (const TDialogId& aId,
                           const RMessage2& aMessage);
                           
    void GetLocalTagL (const TDialogId& aId,
                       const RMessage2& aMessage);                  

    void GetRegisteredContactL (const TRegistrationId& aId,
                                const RMessage2& aMessage);

    void ResetDialogStateL(const TDialogId& aId);

    // Helper functions:
    CSipCSServerITC& ITC ();
    
    CSipServerCore& Server ();
    
    CURIContainer* FillRequestLC (const RMessage2& aMessage,
                                  CSIPRequest& aRequest,
                                  TBool aMethodExpected=EFalse);

    void FillRequestL (const RMessage2& aMessage,
                       CSIPRequest& aRequest,
                       TSIPRemoteTargetAndProxy& aRemoteTargetAndProxy,
                       TBool aMethodExpected=EFalse,
                       TBool aProxyExpected=EFalse);

	TBool OverlappingInDialogAllowed (RStringF aMethod);

    void AddFromL (const TRegistrationId& aRegistrationId,
                   CSIPRequest& aRequest);

    void AddContactL (const TRegistrationId& aRegistrationId,
                      CSIPMessage& aMessage);

    void AddRouteL (const TRegistrationId& aRegistrationId,
                    CSIPRequest& aRequest);
                    
    void CheckSigCompInNextHopL (TRegistrationId aRegistrationId,
                                 CURIContainer& aRemoteTarget,
                                 CSIPRequest& aRequest); 
                    
    void FillToAndRemoteTargetL (CSIPRequest& aRequest,
                                 TSIPRemoteTargetAndProxy& aRemoteTargetAndProxy);

    void CheckSigCompInContactsL (CSIPMessage& aMessage);

    TBool HasSigCompParameter (CURIContainer& aURI) const;
    
    void RefreshConnectionL(TUint32 aIapId);
    
    void GetConnectionErrorL(TSIPIds& aIds, const RMessage2& aMessage);

    
private: // Data    
    
    CSipCSSession& iSession;
    MSIPSecUser& iSIPSecUser;
    TSIPTransportParams iTransportParams;
    CSIPConnection::TState iState;
    TInt iHandle;
    CSipCSSubSessionReceiver* iReceiver;    
    TBool iIapOpened;
    TInetAddr iLocalAddr;

private: // For testing purposes

	UNIT_TEST(CSipCSSubSessionTest)
	};

#endif // CSIPCSSUBSESSION_H

// End of File
