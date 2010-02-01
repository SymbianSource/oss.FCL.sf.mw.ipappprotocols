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
* Name        : csipregistrationstate.h
* Part of     : SIP Registration
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGISTRATIONSTATE_H
#define CSIPREGISTRATIONSTATE_H

// INCLUDES
#include <e32base.h>
#include "SipStackServerDefs.h"
#include "mregistrationcontext.h"

// FORWARD DECLARATIONS
class CSIPRequest;
class CSIPResponse;
class CSIPRouteHeader;

// CONSTANTS
const TUint K423Response = 423;

/*
 * Base class for all registration states.
 */
class CSIPRegistrationState : public CBase
	{
public:
	virtual ~CSIPRegistrationState();

	virtual void RegisterL(TTransactionId&  aTransactionId, 
		                   CSIPRequest*     aRequest,
		                   CSIPRouteHeader* aOutboundProxy);

	virtual void UpdateL(TTransactionId& aTransactionId,
					     CSIPRequest*    aRequest);

	virtual void UnregisterL(TTransactionId& aTransactionId,
							 CSIPRequest*    aRequest);
	
	virtual void ReceiveL(TTransactionId aTransactionId,
		                  CSIPResponse*  aResponse);

	virtual TInt TransactionEnded(TTransactionId aTransactionId, 
		                          TInt           aReason) = 0;
		                          
	virtual TBool Retrying() const;	                    

protected:

	void PassResponseToOwnerL(
		CSIPResponse*   aResponse,
		TTransactionId  aTransactionId,
		TRegistrationId aRegistrationId = KEmptyRegistrationId);

	TInt PassErrorToOwner(
		TInt aReason, 
		TTransactionId  aTransactionId,
		TRegistrationId aRegistrationId = KEmptyRegistrationId);

protected:

	CSIPRegistrationState(MRegistrationContext& aRegistration);

protected: // Data

	MRegistrationContext& iRegistration;
	};

#endif // CSIPREGISTRATIONSTATE_H

// end of file
