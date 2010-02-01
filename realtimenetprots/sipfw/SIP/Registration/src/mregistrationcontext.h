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
* Name          : MRegistrationContext.h
* Part of       : SIP Registrations
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/



#ifndef MREGISTRATIONCONTEXT_H
#define MREGISTRATIONCONTEXT_H

// includes
#include <e32base.h>
#include "SipStackServerDefs.h"

// forward declarations
class CSIPRequest;
class CSIPRouteHeader;
class CSIPResponse;
class CSIPRegistrationState;
class CSIPContactHeader;
class CSIPRegistrar;
class CSIPToHeader;
class MRegistrationOwner;


class MRegistrationContext
	{
public:

    virtual ~MRegistrationContext () { }

    virtual TRefreshId RefreshId() const = 0;

	virtual void RegisterL(TTransactionId&  aTransactionId,
		                   CSIPRequest*     aRequest,
						   CSIPRouteHeader* aOutboundProxy) = 0;

	virtual void UpdateL(TTransactionId& aTransactionId,
		                 CSIPRequest*    aRequest) = 0;

	virtual void UnregisterL(TTransactionId& aTransactionId,
		                     CSIPRequest*    aRequest) = 0;

	virtual CSIPRegistrar* Registrar() = 0;

	virtual void FillToFromL(CSIPRequest& aRequest) = 0;

	virtual void SetExpiresValueL(CSIPRequest& aRequest) = 0;

	virtual CSIPRegistrationState& CurrentState() = 0;

	virtual void ChangeState(CSIPRegistrationState* aState) = 0;

	virtual const CSIPToHeader& AOR () const = 0;
	
	virtual void UpdateContactFromRequestL() = 0;

	virtual CSIPContactHeader& Contact() = 0;

	virtual CSIPRequest* Request() = 0;

	virtual const MRegistrationOwner* Owner() const = 0;

	virtual void SelfDetach(TBool aRefresh) = 0;
	
	// Overloaded version to detatch retry transaction
	virtual void SelfDetach(TBool aRefresh, TBool aUseRetryId) = 0;

	virtual void Received2XXResponseL(CSIPResponse& aResponse,
				TTransactionId aTransactionId) = 0;

	virtual void ReceivedErrorResponseL() = 0;

	virtual void Received423ResponseL(CSIPResponse& aResponse) = 0;

	virtual TRegistrationId RegistrationId() const = 0;

	virtual void ClearRequest() = 0;

	virtual void SetRouteHeaderFromResponseL(CSIPResponse& aResponse) = 0;
	
	virtual TBool ServiceRouteOk(CSIPResponse& aResponse) const = 0;
 	};

#endif // MREGISTRATIONCONTEXT_H

// End of File
