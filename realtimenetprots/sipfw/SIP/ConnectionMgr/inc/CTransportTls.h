/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CTransportTls.h
* Part of       : ConnectionMgr
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/
#ifndef CTRANSPORTTLS_H
#define CTRANSPORTTLS_H

#include <in_sock.h>
#include "MContext.h"
#include "MTlsTransStateOwner.h"
#include "CTransport.h"
#include "Lwtimer.h"

class CSIPConnection;
class CSIPMessage;
class MTransactionFinder;
class MServerTaFactory;
class MTransportOwner;
class CTransportErrorHandler;
class CReceiverTcp;
class CSenderTcp;
class CSender;
class CTcpTransportState;
class CNetworkInfo;
class CSecureSocket;
class CSecureSocketContainer;
class CSIPNATTraversalController;
class MTimerManager;


class CTransportTls : public CTransport,
					  public MContext,
					  public MTlsTransStateOwner,
					  public MExpirationHandler
	{
	
    public:

    	static CTransportTls* NewL( RSocketServ& aServer,
                    				MTransactionFinder& aFinder, 
                    				MServerTaFactory& aTaFactory,
                    				MTransportOwner& aTransportOwner,
                    				MTimerManager& aTimer,
                    				const TSIPTransportParams& aTransportParams,
                    				const TInetAddr& aRemoteAddr,
                    				TUint aTOne,
                    				CSIPServerResolver& aServerResolver,
                    				RConnection& aConnection,
                    				MSigCompController& aSigCompHandler,
                    				CNetworkInfo& aNetworkInfo,
                    				COwnerSettingsList& aSettingsList,
                    				CSIPNATTraversalController& aNATTraversal );

    	static CTransportTls* NewLC( RSocketServ& aServer,
                        			 MTransactionFinder& aFinder, 
                        			 MServerTaFactory& aTaFactory,
                                     MTransportOwner& aTransportOwner,
                                     MTimerManager& aTimer,
                                     const TSIPTransportParams& aTransportParams,
                                     const TInetAddr& aRemoteAddr,
                                     TUint aTOne,
                                     CSIPServerResolver& aServerResolver,
                                     RConnection& aConnection,
                                     MSigCompController& aSigCompHandler,
                                     CNetworkInfo& aNetworkInfo,
                                     COwnerSettingsList& aSettingsList,
                                     CSIPNATTraversalController& aNATTraversal );

        ~CTransportTls();

		//This function sets the domain name for CTransportTls object,
		//which will be used in matching the domain name which is retrived
		//from the Server Certificate durin TLS negotiation
        void SetDomainNameL(const TDesC8& domainName);
        
        //This function determines whether the Secure socket has been created or not
        TBool HasSecureSocket();
		
		//This function does the certificate matching as per draft-ietf-sip-domain-certs-04
		//and the output is ETrue if certificate matches otherwise EFalse
        TBool MatchCertificateL();
        
    public:	// From CTransport

    	TBool HandleMessage( const TSIPTransportParams& aParams,
    	                     RStringF aProtocol, 
    	                     const TInetAddr& aRemoteAddr,
    	                     TUint aLocalPort,
    	                     TBool aIsStrict );
    	                     
    	TUint SourcePort();
    	
    	void SendToNetL( const TSIPTransportParams& aParams,
    	                 const TInetAddr& aAddress,
    					 CSIPMessage& aMessage, 
    					 TBool aForceUDP,
    					 TUint aOrigTransport, 
    					 TRequestStatus &aStatus );
    					 
    	TBool CancelSend( TRequestStatus& aStatus );
    	
    	CTransportErrorHandler& ErrorHandler();
    	
    	void WaitL();
    	
    	TBool IsWaiting( TUint aProtocol, TUint aPort );
    	
    	void CancelAllRequests( CSIPConnection::TState aReason );
    	
    	TUint Protocol();
    	
    	CSocketContainer& GetSocketContainerL( const TInetAddr& aRemoteAddr );
    	
    	void InformSendingStatus( const TInetAddr& aRemoteAddr );
    	
    	CSender* Sender( const TSIPTransportParams& aParams,
    	                 TUint aProtocol, 
    	                 const TInetAddr& aRemoteAddr );
    	                 
    	void Disconnect( const TInetAddr& aRemoteHost );
    	              
    	TInetAddr* PersistentRemoteAddr( MSIPNATBindingObserver* aBindingObserver );
    	
    	             
    public:	// From MContext

    	CSocketContainer& SocketContainer();
    	
    	CSocketContainer* SocketContainer( TUint aIPAddrFamily );
    	void ReceivedDataL( HBufC8* aData,
    					    const TInetAddr& aRemoteAddr,
    					    TBool aCompressed );
    					    
    	void ReceivedDataL( CSIPMessage* aMessage,
    					    const TInetAddr& aRemoteAddr,
    					    TInt aError,
    					    TBool aCompressed );
    	
    	MSigCompController* SigCompHandler();
    					    
    	void ResetSocketL(); 
    	
    	void ContinueL();
    	
    	TBool StopL();
    	
    	TInt Remove();
    	
    	TBool ConnectionOpenL();
    	
    	TBool DisconnectedL();
    	
    	void ReRouteL( TUint aProtocol, COutgoingData* aData );
    	
    	TUint32 IapId();
    	
    	void Sending( TBool aIsSending );

    public: // From MTlsTransStateOwner

    	void ChangeStateL( TTlsTransStateBase::TState aState );
    	
    	void ConnectTcpL();
    	
    	void ConnectTlsL();
    	
    	TBool Retry() const;
    	
    	void ResetL();
    	
    	void Destroy();

        CSender& GetSender();
        
        void StartTimerL( TInt aTimeout );
        
         void StopTimer();
    
    public: // From MExpirationHandler
    
        void TimerExpiredL( TTimerId aTimerId, TAny* aTimerParam );
    	
    private:

    	CTransportTls( RSocketServ& aServer,
                       MTransactionFinder& aFinder, 
                       MServerTaFactory& aTaFactory,
                       MTransportOwner& aTransportOwner,
                       MTimerManager& aTimer,
                       const TSIPTransportParams& aTransportParams,
                       const TInetAddr& aRemoteAddr,
                       TUint aTOne,
                       CSIPServerResolver& aServerResolver,
                       RConnection& aConnection,
                       MSigCompController& aSigCompHandler,
                       CNetworkInfo& aNetworkInfo,
                       COwnerSettingsList& aSettingsList,
                       CSIPNATTraversalController& aNATTraversal );

        void ConstructL();
        
        void InitializeStatesL();
        
        TTlsTransStateBase& CurrentState();

        void CheckCipherSuiteAvailabilityL();

    private: // Data
    	
    	RSocketServ& iSocketServ;

	    MTimerManager& iTimer;
	    
	    TUint iTimerValue;
    	
        RConnection& iConnection;
        
        TInetAddr iRemoteAddr;
        			
    	RSocket iSocket;
    	
    	CSecureSocket* iSecureSocket;
    	
    	CSecureSocketContainer* iSocketContainer;
    	
    	CTransportErrorHandler* iErrorHandler;
    	
    	CReceiverTcp* iReceiver;
    	
    	CSenderTcp* iSender;
    	
    	CArrayVarFlat< TTlsTransStateBase > iStates;
        TTlsTransStateBase::TState iCurrentState;
        
        TInt iConnectAttemptCount;
        
        TBool iIsSending;
        
        TTimerId iMyTimerId;
        
		//Domain name is stored in this variable
        HBufC8* iDomainName;

	private: //For testing purposes
#ifdef CPPUNIT_TEST	
    friend class CTransportTlsTest;
#endif
    
	};

#endif // end of CTRANSPORTTLS_H

// End of File
