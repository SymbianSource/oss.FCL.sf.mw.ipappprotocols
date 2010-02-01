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
* Name          : sipsecagreementcontainer.h
* Part of       : SIPSec
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPSECAGREEMENTCONTAINER_H
#define CSIPSECAGREEMENTCONTAINER_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "MSIPSecUser.h"
#include "mechanismstore.h"
#include "sipsecagreement.h"
#include "sipsecobserver.h"

// FORWARD DECLARATIONS
class CUri8;

// CLASS DECLARATION
class CSIPSecAgreementContainer: public CBase
	{
public: // Constructors and destructor

	static CSIPSecAgreementContainer* NewL(
				MSIPSecMechanismStore& aMechanismStore );

	static CSIPSecAgreementContainer* NewLC(
				MSIPSecMechanismStore& aMechanismStore );

	~CSIPSecAgreementContainer();

public: // New functions

	void ResponseReceivedL( CSIPResponse& aResponse,
		                    TInetAddr& aNextHopAddr,
		                    const TDesC8& aNextHop,
		                    const MSIPSecUser& aUser,
		                    TBool aUseSecurityServerHeaders );

	void AddSecurityParamsL( TSIPTransportParams& aTransportParams,
							 CSIPRequest& aRequest,
		                     TInetAddr& aNextHopAddr,
			                 const TDesC8& aNextHop,
		                     const CUri8& aRemoteTarget,
		                     const TDesC8& aOutboundProxy,
						     MSIPSecUser& aUser,
						     TRegistrationId aRegistrationId );

	/**
	* Clear security agreements of the specified MSIPSecUser.
	* @param aUser User whose agreements are removed.
	*/
	void ClearCache( const MSIPSecUser& aUser );

	void CancelPendingOperations( MSIPSecObserver* aObserver );

	HBufC8* NegotiatedSecurityMechanismL( const TDesC8& aHop );
		
private: // For internal use

	CSIPSecAgreementContainer( MSIPSecMechanismStore& aMechanismStore );

	/**
	* Search whether there exists an agreement for the specified next-hop.
	* @param aNextHopAddr IP-address of the next-hop	
	* @return CSIPSecAgreement* Found agreement, or NULL if agreement doesn't
	*	exist for the next-hop. Ownership is not transferred.
	*/
	CSIPSecAgreement* Agreement( const TInetAddr& aNextHopAddr );

	/**
	* Creates a new agreement and stores it into iAgreementList.
	* @param aNextHopAddr IP-address of the next-hop
	* @param aNextHop Next hop as IP-address or domain name.
	* @param aUser User related to creating the agreement.
	* @return CSIPSecAgreement* New object, ownership is not transferred as
	*	the agreement is now owned by the iAgreementList.	
	*/
	CSIPSecAgreement* CreateAgreementL( TInetAddr& aNextHopAddr,
							            const TDesC8& aNextHop,
							            const MSIPSecUser& aUser );

	void EmptyAgreementList();
	void AddRequiredHeadersIfNeededL( CSIPRequest& aSIPRequest );

private: // Data

	MSIPSecMechanismStore& iMechanismStore;

	//Owns the agreements
	TSglQue<CSIPSecAgreement> iAgreementList;
	TSglQueIter<CSIPSecAgreement> iAgreementListIter;
private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class CSIPSecAgreementContainerTest;
#endif
	};
	
#endif
