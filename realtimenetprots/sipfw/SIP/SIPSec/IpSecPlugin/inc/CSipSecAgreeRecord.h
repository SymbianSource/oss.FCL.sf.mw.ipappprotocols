/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPSecAgreeRecord.h
* Part of       : SIPSec
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPSECAGREERECORD_H
#define CSIPSECAGREERECORD_H

#include <e32base.h>

#include "Lwtimer.h"
#include "MSipSecSadbObserver.h"
#include "MSipSecPolicyObserver.h"
#include "SipSecIpsecParams.h"
#include "TSIPTransportParams.h"
#include "CState.h"

class CSIPSecurityVerifyHeader;
class CSIPSecurityHeaderBase;
class CSIPMessage;
class CSIPRequest;
class CSIPResponse;
class CURIContainer;
class MIpSecAgreeContextParams;
class CIpSecSadbHandler;

#ifdef CPPUNIT_TEST
class CActiveObjController1;
#endif

/**
 * @brief Contains Security-Client and Security-Verify headers
 */
class CSipSecAgreeRecord :
	public CBase,
	public MSipSecSadbObserver,
	public MSipSecPolicyObserver,
	public MExpirationHandler
	{
public: // Constructors and destructor

    /**
    * Constructs the object. Leaves on Failure
    * @return Return the CSipSecAgreeRecord object
    * 
    */
    static CSipSecAgreeRecord*
    	NewL( RPointerArray<CState>& aStates,
			  MIpSecAgreeContextParams& aParams,
			  const TSIPTransportParams& aTransportParams );

    /**
    * Constructs the object and adds pointer to cleanup stack.
    * @return Return the CSipSecAgreeRecord object
    */
    static CSipSecAgreeRecord*
    	NewLC( RPointerArray<CState>& aStates,
			   MIpSecAgreeContextParams& aParams,
			   const TSIPTransportParams& aTransportParams );
    
    /**
    * Destructor
    */
    ~CSipSecAgreeRecord();
    
public: // From MSipSecSadbObserver

    void SAsAddedL( TBool aSuccess );

	void SAsDeletedL();

public: // From MSipSecPolicyObserver

    void PolicyActivatedL();
    
	void PolicyUnloaded();	
	
	void PolicyError( CIpSecPolicyHandler::TPolicyHandlerState aState, 
	                  TInt aError );
	
public: // MExpirationHandler

	void TimerExpiredL( TTimerId aTimerId, TAny* aTimerParam );

public: // Functions
	
	void Resp4xxL( CSIPResponse& aResponse, CSIPRequest& aRequest );
	
    TBool AuthKeyL( const TDesC8& aAuthKey );
    
    void AuthKeyFailedL();
    
	void RegisterL( TSIPTransportParams& aTransportParams, 
	                CSIPRequest& aRequest, 
	                const TDesC8& aOutboundProxy );

	void ReguestL( TSIPTransportParams& aTransportParams, 
	               CSIPRequest& aRequest, 
	               const TDesC8& aOutboundProxy );
	
	void Resp2xxL( CSIPResponse& aResponse );
	
	void ProcessSecVerifyL(	RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify );
	
	void ClearSaL();
	
	CState::TSecAgreeRecordState State() const;
	
	void ReserveTransportAndCreatePolicyL();
	
	void RemoveTransportL();	
	                   
	void CancelPendingOps();
	
	void AddSasL();
	
	void DeleteSasL();
	
	void SetSaParamsL( CSIPSecurityHeaderBase& aCliHeader, 
				       CSIPSecurityHeaderBase& aSrvHeader,
				       const TDesC8& aAuthKey );
				    
    void SADBAddMsgReceived( TUint aMsgSeq, TInt aError );

    void RemovalCompleted( TUint32 aTransportId );

    TUint ExpiresAfter() const;

	const TSIPTransportParams& TransportParams() const;

	TBool IsObsolete() const;

	/**
    * Sets the record as obsolete, meaning it will no longer affect the SIP
    * messages. Record will be deleted using the normal mechanism, except the
    * expiration timer is set to expire immediately.
    */
	void SetObsoleteL();

private: // Functions

	void CreatePolicyL();
	
	void RemovePolicy();

	RPointerArray<CSIPSecurityHeaderBase>&
		SecurityHeaders( TBool aIsSecClientHeader = ETrue );
	    
	void CopyHeadersToMessageL( CSIPMessage& aMessage,
								TBool aIsSecCliHeaders );

	void CopyHeadersFromMessageL( CSIPMessage& aMessage,
								  TBool aIsSecCliHeaders );

    void UpdateNextHopPort( CSIPRequest& aRequest,
    						const TDesC8& aOutboundProxy );
	  
	void StartTimerL( TUint aMilliseconds );

	void StopTimer();

	void NextState( CState::TSecAgreeRecordState aState );

	TBool UnloadingAndDeletionDone() const;
	
	TBool UpdateURIPort( CURIContainer* aURIContainer, 
                         const TDesC8& aOutboundProxy );
      
    static void CopyHeadersToMessageL( RPointerArray<CSIPSecurityHeaderBase>& aHeaders,
	                                   CSIPMessage& aMessage,
	                                   TBool aIsSecCliHeaders );

    static void CopyHeadersFromMessageL( CSIPMessage& aMessage,
	                                     RPointerArray<CSIPSecurityHeaderBase>& aHeaders,
	                                     TBool aIsSecCliHeaders );

    static TUint ExpiresFromContactL( CSIPMessage& aMessage );
    
private: // Private functions

    /*
     * Two phase constructor
     */
    void ConstructL();

    /*
     * Constructor
     */
    CSipSecAgreeRecord( RPointerArray<CState>& aStates, 
						MIpSecAgreeContextParams& aParams,
						const TSIPTransportParams& aTransportParams );

private: // Data

	// Sec-Agree context parameters
	MIpSecAgreeContextParams& iParams;

	// Security-Client headers that contain list of client's security
	// mechanisms. Owned.
	RPointerArray<CSIPSecurityHeaderBase> iSecClientHeaders;

	// Security-Verify headers that contain list of server's security
	// mechanisms. Owned.
	RPointerArray<CSIPSecurityHeaderBase> iSecVerifyHeaders;

	// SA parameters
	TSipSecSaParams iSaParams;

	// Transport parameters
	TSIPTransportParams iTransportParams;

	// Current timer id
	TTimerId iTimerId;

	// All states of the record
	RPointerArray<CState> &iStates;
		
	// Information about completion of SAs deletion
	TBool iSAsDeleted;
	
	// Information about completion of policy unloading
	TBool iPolicyUnloaded;
	
	// SADB handler, owned
	CIpSecSadbHandler* iSadbHandler;

	// Policy handler, owned
    CIpSecPolicyHandler* iPolicyHandler;

	// Current state of the record, Does not own
	CState* iState;
	
	// Number of outstanding async transport removal operations
	TInt iNumAsyncRemovals;

	// Initially this flag is EFalse, but when there arises a need to create a
	// third SA, the oldest SA is marked obsolete by setting this flag ETrue.
	// If ETrue, then this record only exists for releasing its transport, SADB
	// and policy resources. After the resources have been released, this
	// record will be deleted. During that, this record won't affect any SIP
	// messages being passed to this plugin.
	TBool iObsoleteRecord;	

friend class CState;
friend class CTentative;
friend class CPendingKey;
friend class CPendingSa;
friend class CPendingPolicy;
friend class CTemporal;
friend class CInUse;
friend class COld;
friend class CDying;
friend class CDeleting;
friend class CClearing;

#ifdef CPPUNIT_TEST
friend class CActiveObjController1;
friend class CSipIpSecTest;
public:
	CActiveObjController1* iObjCtr;   
#endif   
	};

#endif // CSIPSECAGREERECORD_H

// End of File
