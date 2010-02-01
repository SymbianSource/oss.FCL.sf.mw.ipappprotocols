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
* Name          : sipsecagreement.h
* Part of       : SIPSec
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPSECAGREEMENT_H
#define CSIPSECAGREEMENT_H

// INCLUDES
#include "SipStackServerDefs.h"
#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class CUri8;
class CSIPURI;
class CSIPRequest;
class CSIPResponse;
class CSIPSecurityVerifyHeader;
class CSIPSecurityServerHeader;
class CSIPSecClientMechanisms;
class MSIPSecMechanismStore;
class MSIPSecSecurityMechanism;
class MSIPSecUser;
class TSIPTransportParams;

// CLASS DECLARATION

/**
*  @lib SipServer.exe
*/
class CSIPSecAgreement: public CBase
	{
public: // Constructor and destructor

	/**
	 * Create agreement
	 *
	 * @param aNextHopAddr Next-hop ip-address
	 * @param aNextHop Next-hop as descriptor
	 * @param aUser SIPSec user
	 * @param aMechanismStore Existing mechanisms
	 * @return CSIPSecAgreement* New instance, ownership is transferred.
	 */
	static CSIPSecAgreement* NewLC( const TInetAddr& aNextHopAddr,
								    const TDesC8& aNextHop,
								    const MSIPSecUser& aUser,
								    MSIPSecMechanismStore& aMechanismStore );

	~CSIPSecAgreement();

public: // New functions

	TBool NextHop( const TInetAddr& aNextHopAddr ) const;

	TBool NextHop( const TDesC8& aNextHop ) const;

	HBufC8* MechanismNameL() const;

	void HandleRequestL( TSIPTransportParams& aTransportParams,
					 	 CSIPRequest& aRequest,
						 const CUri8& aRemoteTarget,
						 const TDesC8& aOutboundProxy,
						 MSIPSecUser& aUser,
						 TRegistrationId aRegistrationId );

	/**
	 * Process a received SIP response.
	 *
	 * @param aResponse SIP response
	 * @param aUser SIPSec user
	 * @param aUseSecurityServerHeaders Tells if the Security-Server headers in
	 *		  aResponse can be used.
	 */
	void ResponseReceivedL( CSIPResponse& aResponse,
							const MSIPSecUser& aUser,
							TBool aUseSecurityServerHeaders );

	/**
	 * Removes a SIPSec user from the agreement, and checks if this agreement
	 * can be removed. Agreement can be removed when it has no users.
	 *
	 * @param aUser SIPSec user to detach
	 * @return ETrue This agreement can be removed, EFalse otherwise
	 */	
	TBool RemoveUser( const MSIPSecUser& aUser );

	/**
	 * Checks if the SIP response should be handled by the security agreement
	 * procedures.
	 *
	 * @param aResponseCode SIP response code
	 * @return ETrue If the response should be processed by security agreement.
	 *		   EFalse otherwise.
	 */
	static TBool IsSecAgreeResponse( TUint aResponseCode );
	
public:	// Data

	static const TInt iOffset;
	TSglQueLink iLink;
	
private:

	enum TState
        {
	    EWaitResponse, 		//Waiting a response with Security-Server headers
	    EMechanismSelected, //A common security mechanism has been selected
	    EVerified			//Security-Verify headers have been formed
	    };

private: // Constructors

	CSIPSecAgreement( const TInetAddr& aNextHopAddr,
					  MSIPSecMechanismStore& aMechanismStore );

	void ConstructL( const TDesC8& aNextHop, const MSIPSecUser& aUser );

private: // New functions, for internal use

	void ProcessSecurityClientL( CSIPRequest& aRequest ) const;
								  			  	
	void ChooseMechanismL( CSIPResponse& aResponse );

	/**
	 * If aUser is not yet present in iUsers, it is added.
	 *
	 * @param aUser SIPSec user
	 */
	void AddUserIfNotStoredL( const MSIPSecUser& aUser );
	
	/**
	 * Checks if the SIP request is one of those for which the Security-Verify
	 * headers are not processed.
	 *
	 * @return ETrue If the request is CANCEL, ACK or PRACK.
	 *		   EFalse otherwise.
	 */
	TBool IsExceptionRequest( const CSIPRequest& aRequest ) const;

	void CreateSecurityVerifyL();

	void CopySecurityVerifyToRequestL( CSIPRequest& aRequest ) const;
	
	void CopySecurityServerFromResponseL( CSIPResponse& aResponse );

	CSIPSecAgreement::TState CurrentState() const;

private: // Data

	CSIPURI* iUri;
	TInetAddr iNextHopAddr;

	// This textual representation of next hop is only needed for getting the
	// negotiated security mechanism.
	HBufC8* iNextHop;

	// Contains the MSIPSecUsers that are using this CSIPSecAgreement.
	// MSIPSecUsers are not owned.
	RPointerArray<MSIPSecUser> iUsers;

	// Selected security mechanism, not owned.
	MSIPSecSecurityMechanism* iMechanism;

	// Security mechanisms that application wants to use. Taken from
	// Security-Client headers.
	CSIPSecClientMechanisms* iClientMechanisms;
	
	MSIPSecMechanismStore& iMechanismStore;

	// Security-Server headers, obtained from response and deleted after
	// Security-Verify headers have been formed.
	RPointerArray<CSIPSecurityServerHeader> iSecurityServer;

	// Security-Verify headers, constructed from Security-Server headers.	
	RPointerArray<CSIPSecurityVerifyHeader> iSecurityVerify;	

private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class CSIPSecAgreementTest;
#endif
	void __DbgTestInvariant() const;


	};

#endif // CSIPSECAGREEMENT_H

// End of File
