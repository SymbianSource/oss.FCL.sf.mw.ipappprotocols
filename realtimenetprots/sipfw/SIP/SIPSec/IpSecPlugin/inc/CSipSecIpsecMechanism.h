/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipSecIpsecMechanism.h
* Part of       : SIPSec
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSECIPSECMECHANISM_H
#define CSIPSECIPSECMECHANISM_H

#include <lib_pfkey.h>
#include <ipsecpolapi.h>

#ifdef CPPUNIT_TEST
#include "tsipsecmechanisminitparams.h"
#include "msipsecsecuritymechanism.h"
#else
#include "sipsecsecuritymechanism.h"
#endif

#include "SipSecIpsecParams.h"
#include "MSipSecSaDbSendReceiveObserver.h"
#include "MIpSecMechanismParams.h"
#include "MSIPTransportRemovalObserver.h"

class CSipSecSaDbMsgReceiver;
class CSipSecSaDbMsgSender;
class CSipSecAgreeContext;
class MTimerManager;
class MSIPTransportMgr;
class TSIPSecMechanismInitParams;
class CState;
class TSIPTransportParams;

#ifdef CPPUNIT_TEST
class CActiveObjController1;
#endif

/**
 * @brief IPSEC Mechanism implementation
 * 
 * 
 */
#ifdef CPPUNIT_TEST
class CSipSecIpsecMechanism : public CBase, public MSIPSecSecurityMechanism,
#else
class CSipSecIpsecMechanism : public CSIPSecSecurityMechanism,
#endif
	public MSipSecSaDbSendReceiveObserver,
	public MIpSecMechanismParams,
	public MSIPTransportRemovalObserver
	{
	
public: // Constructors and destructor

    /**
    * Constructs the object. Leaves on Failure
    *
    * @return Return the CSIPSecAgreeRecordContainer object
    * 
    */
	static CSipSecIpsecMechanism* NewL( TAny* aInitParam );

    /**
    * Constructs the object and adds pointer to cleanup stack.
    *
    * @return Return the CSIPSecAgreeRecordContainer object
    */
	static CSipSecIpsecMechanism* NewLC( TAny* aInitParam );
    
    /**
    * Destructor
    */
    ~CSipSecIpsecMechanism();
    
public: // Functions from MSIPSecSecurityMechanism

	const TDesC8& Name() const;
	
	void InitializeSecurityClientL( CSIPSecurityClientHeader& aSecurityClient );

  	void ProcessSecurityVerifyL(
  						TSIPTransportParams& aTransportParams,
                  	    CSIPRequest& aRequest,
                        TInetAddr& aNextHop,
                        const CUri8& aRemoteTarget,
                        const TDesC8& aOutboundProxy,
                        MSIPSecUser* aUser,
                        TRegistrationId aRegistrationId,
                        RPointerArray<CSIPSecurityServerHeader>& aSecurityServer,
                		RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify );

	void AddSecurityParamsL( TSIPTransportParams& aTransportParams,
	                         CSIPRequest& aRequest,
	                         TRegistrationId aRegistrationId,
	                         TTransactionId aTransactionId,
			                 TInetAddr& aNextHop,
			                 const CUri8& aRemoteTarget,
			                 const TDesC8& aOutboundProxy,
							 MSIPSecUser* aUser );
						    
	TBool ResponseReceivedL( TSIPTransportParams& aTransportParams,
	                         CSIPResponse& aResponse,
	                         CSIPRequest& aRequest,
	                         TRegistrationId aRegistrationId,
	                         TTransactionId aTransactionId,
			                 TInetAddr& aNextHop,
			                 const CUri8& aRemoteTarget,
			                 const TDesC8& aOutboundProxy,
							 MSIPSecUser* aUser,
							 MSIPSecSecurityMechanismObserver& aObserver );
	
	TBool IsServerInitiatedSecAgreeAllowed() const;
	
	TBool ParametersUpdatedL( MSIPSecUser* aUser );
	    
	void CancelPendingOperations( MSIPSecSecurityMechanismObserver* aObserver );

	void ClearCache( MSIPSecUser* aUser );

	void SetCredentialsL( TTransactionId aTransactionId,
	                      const TDesC8& aRealm,
			              const TDesC8& aOutboundProxy, 
						  const TDesC8& aUserName,
						  const TDesC8& aPassword );

	void SetCredentialsL( const MSIPSecUser& aUser,
	                      const TDesC8& aRealm,
			              const TDesC8& aOutboundProxy, 
						  const TDesC8& aUserName,
						  const TDesC8& aPassword );
	
	TInt IgnoreChallenge( TTransactionId aTransactionId,
	                      const TDesC8& aRealm,
	                      const MSIPSecUser* aTrustedUser );

	TInt RemoveCredentials( const TDesC8& aRealm );
	
	
public: // Functions from MSipSecSaDbSendReceiveObserver

	void MessageSentL( TInt aStatus, 
					   TInt aMsgType );

	void MessageReceived( TInt aStatus );

public: // Functions from MIpSecMechanismParams

	TUint T1();
	RSADB& Sadb();
	RIpsecPolicyServ& PolicyServer();
	TUint SeqNumber();
	MTimerManager& TimerMan();
	MSIPTransportMgr& TransportMan();
	void UpdateSecCliL( RPointerArray<CSIPSecurityHeaderBase>& aSecCliHeaders );
	void ContextCleared( CSipSecAgreeContext* aContext );
	MSIPTransportRemovalObserver* TransportRemovalObserver();
	void ProcessInitialReqisterL( TSIPTransportParams& aTransportParams,
                                  CSIPRequest& aRequest );

public: // Function from MSIPTransportRemovalObserver

    void RemovalCompleted( TUint32 aTransportId );
    

private: // Functions
					
	TBool Capable();
								  
	void ConsumeSadbMessage();
	
	void SadbRegister();
	
	void SadbAdd();
	
	void InitSecCliHeaderL( CSIPSecurityClientHeader* aCliHeader );
	
	void CreateNewContextL( TSIPTransportParams& aTransportParams,
	                        CSIPResponse& aResponse, 
	                        CSIPRequest& aRequest,
	                        TInetAddr& aNextHop, 
	                        MSIPSecUser* aUser,
	                        MSIPSecSecurityMechanismObserver& aObserver );
	                        
	CSipSecAgreeContext* FindContext( const MSIPSecUser* aSipUser );
	
	CSipSecAgreeContext* FindContext( const TInetAddr& aNextHop );

	void DeleteContext( CSipSecAgreeContext* aContext );
	
	void InitializeStatesL();
							  
private: // Private constructor

    void ConstructL();

    /*
     * Constructor
     */
	CSipSecIpsecMechanism( TSIPSecMechanismInitParams* aInitParams );
        
private: // Data

	// Seed used for generating ports and spi's randomly    
    TInt64 iSeed;
    
    // Handle to socket server
    RSocketServ iSockServ;
    
    // Handle to IPSEC SADB
    RSADB iSaDb;
    
    // Handle to IPSEC policy server
    RIpsecPolicyServ iPolicyServ;
    
    // Sequence number for SADB messages
    TUint iSeqNum;
    
    // Buffer for received messages
    TPfkeyRecvMsg iReceiveBuffer;
        
    // Contains protocol, mode, algorithm capabilities
    TSipSecIpsecCaps iCaps;
    
    // IPSEC Sec Agree contexts for each negotiated SAs
    RPointerArray<CSipSecAgreeContext> iSecAgreeContexts;
    
    // Timer services
    MTimerManager& iTimerMgr;
    
    // Plugin context (Framework)
    MSIPSecEngineContext& iEngineContext;
    
    // Transport management (ConnectionMgr)
    MSIPTransportMgr& iTransportMgr;
    
   	// SIP T1 timer
	TUint iT1;
    
    // All states of the record
	RPointerArray<CState> iStates;
	
	// Receiver for SADB messages
	CSipSecSaDbMsgReceiver* iSaDbMsgReceiver;
	
	// Sender for sending initial SADB_REGISTER message
	CSipSecSaDbMsgSender* iSaDbMsgSender;


#ifdef CPPUNIT_TEST
	friend class CActiveObjController1;
	friend class CSipIpSecTest;
public:
	CActiveObjController1* iObjCtr;   
#endif
	};

#endif // CSIPSECIPSECMECHANISM_H


// End of File
