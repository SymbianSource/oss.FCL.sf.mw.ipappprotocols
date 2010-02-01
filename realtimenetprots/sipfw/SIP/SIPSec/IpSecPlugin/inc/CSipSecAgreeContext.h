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
* Name          : CSipSecAgreeContext.h
* Part of       : SIPSec
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSECAGREECONTEXT_H
#define CSIPSECAGREECONTEXT_H

#include <e32base.h>

#include "MIpSecAgreeContextParams.h"
#include "CState.h"

class CSipSecAgreeRecord;
class MSIPSecUser;
class CSIPRequest;
class CSIPResponse;
class CSIPMessage;
class TInetAddr;
class MSIPSecSecurityMechanismObserver;
class CSIPSecurityServerHeader;
class CSIPSecurityClientHeader;
class MIpSecMechanismParams;
class CState;
class CIpSecPolicyHandler;

#ifdef CPPUNIT_TEST
class CActiveObjController1;
#endif

/**
 * @brief Container of array of sec-agree records
 */
class CSipSecAgreeContext :
	public CBase,
	public MIpSecAgreeContextParams
	{
public: // Constructors and destructor

    static CSipSecAgreeContext* NewL(
      			MIpSecMechanismParams& aParams,
      			TSIPTransportParams& aTransportParams,
      			CSIPResponse& aResponse,
      			CSIPRequest& aRequest,
    			TInetAddr& aRemoteAddress,
				MSIPSecUser* aUser,
				MSIPSecSecurityMechanismObserver& aObserver,
				RPointerArray<CState>& aStates );

    static CSipSecAgreeContext* NewLC(
    			MIpSecMechanismParams& aParams,
    			TSIPTransportParams& aTransportParams,
    			CSIPResponse& aResponse, 
    			CSIPRequest& aRequest,	
    			TInetAddr& aRemoteAddress,
				MSIPSecUser* aUser,
				MSIPSecSecurityMechanismObserver& aObserver,
				RPointerArray<CState>& aStates );
    	
    /**
    * Destructor
    */
    ~CSipSecAgreeContext();
    
public: // From MIpSecAgreeContextParams
 
	MIpSecMechanismParams& MechParams();

	TInetAddr LocalAddress();
	TInetAddr RemoteAddress();
	RIpsecPolicyServ& PolicyServer();
	TBool HasOnlyOneRecord() const;
	TBool HasRecordInState( CState::TSecAgreeRecordState aState );

	void SaDeleted( CSipSecAgreeRecord* aRecord );
	void SaCleared( CSipSecAgreeRecord* aRecord );
	
	void SAReady( TBool aSuccess );
	
	TBool HasLongerLifetimeSA( TUint aTimeToCompareInMillisecs, 
	                           TUint& aLongerLifetimeInMillisecs );

public: // Functions
			     
    TBool HasSecUser( const MSIPSecUser* aSecUser ) const;
    
    TBool HasNextHop( const TInetAddr& aNextHop ) const;
    
	void ApplyRulesL( TSIPTransportParams& aTransportParams, 
	                  CSIPRequest& aRequest,
	                  const TDesC8& aOutboundProxy,
	                  MSIPSecUser* aUser );
	
	void ApplyRulesL( CSIPResponse& aResponse,
	                  CSIPRequest& aRequest,
					  MSIPSecSecurityMechanismObserver& aObserver );
					  
	void ApplyRulesL( RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify );
	
	TBool SetAuthKeyL( const TDesC8& aAuthKey );
	
	void AuthKeyFailedL();
	
	void ClearL();
	
	void CancelPendingOps( const MSIPSecSecurityMechanismObserver* aObserver );
	
	void SADBAddMsgReceived( TUint aMsgSeq, TInt aError );
	
	void RemovalCompleted( TUint32 aTransportId );
  
private:

	/*
     * Constructor
     */
    CSipSecAgreeContext( MIpSecMechanismParams& aParams,
						 MSIPSecSecurityMechanismObserver& aObserver,
						 RPointerArray<CState>& aStates );

    void ConstructL( TSIPTransportParams& aTransportParams,
                     CSIPResponse& aResponse,
                     CSIPRequest& aRequest,	
    				 TInetAddr& aRemoteAddress,
					 MSIPSecUser* aUser );    

	void CreateNewSecAgreeRecordIfNeededL();

	void DeleteRecord( CSipSecAgreeRecord* aRecord );	

	/**
    * Returns the next non-obsolete record, starting from position indicated by
    * aPos.
    * @param aPos IN: position in array where the search in started
    *			  OUT: position in array where the search will be continued
    * @return CSipSecAgreeRecord* Found record, or NULL if not found. Ownership
    *		  is not transferred.
    */
	CSipSecAgreeRecord* NextRecord( TInt& aPos ) const;

private: // Data

	// Array of SecAgee records
	RPointerArray<CSipSecAgreeRecord> iSecAgreeRecords;

	TInetAddr iLocalAddress;
	TInetAddr iRemoteAddress;
	MIpSecMechanismParams& iParams;
	MSIPSecUser* iSecUser;
	MSIPSecSecurityMechanismObserver* iObserver;

	// All states of the record
	RPointerArray<CState>& iStates;

	// Indicates if the ClearL has been called and Sec-Agree records are
	// begin cleared.
	TBool iClearOrdered;	


#ifdef CPPUNIT_TEST
friend class CActiveObjController1;
public:
	CActiveObjController1* iObjCtr;   
#endif
   
	};

#endif // CSIPSECAGREECONTEXT_H


// End of File
