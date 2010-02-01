// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : csipunregistering.cpp
// Part of       : SIP Registration
// Version       : SIP/4.0 
//



#include "csipunregistering.h"
#include "CSIPRegistrar.h"
#include "SipAssert.h"
#include "sipcontactheader.h"
#include "sipresponse.h"
#include "MRegistrationOwner.h"
#include "siprequest.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSIPUnregistering::NewL
// -----------------------------------------------------------------------------
//
CSIPUnregistering* CSIPUnregistering::NewL(MRegistrationContext& aRegistration)
	{
	CSIPUnregistering* self = CSIPUnregistering::NewLC(aRegistration);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPUnregistering::NewLC
// -----------------------------------------------------------------------------
//
CSIPUnregistering* CSIPUnregistering::NewLC(MRegistrationContext& aRegistration)
	{
	CSIPUnregistering* self = new (ELeave) CSIPUnregistering(aRegistration);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPUnregistering::NewLC
// -----------------------------------------------------------------------------
//
void CSIPUnregistering::SetNeighbourStates(
	CSIPRegistrationState* aUnregistered)
	{
	__SIP_ASSERT_RETURN(aUnregistered, KErrArgument);

	iUnregistered = aUnregistered;
	}

// -----------------------------------------------------------------------------
// CSIPUnregistering::~CSIPUnregistering
// -----------------------------------------------------------------------------
//
CSIPUnregistering::~CSIPUnregistering()
	{
	}

// -----------------------------------------------------------------------------
// CSIPUnregistering::UnregisterL
// -----------------------------------------------------------------------------
//	
void CSIPUnregistering::UnregisterL(TTransactionId& aTransactionId,
					                CSIPRequest*    aRequest)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);	

	CSIPContactHeader* contact = static_cast<CSIPContactHeader*>(
		aRequest->Header(SIPStrings::StringF(SipStrConsts::EContactHeader),0));

	__SIP_ASSERT_LEAVE(contact->ExpiresParameter() == 0, KErrArgument);

	// registrar must not be NULL after registering succeed.
	__SIP_ASSERT_LEAVE(iRegistration.Registrar(), KErrGeneral);

	// copy CallId and CSeq header from binding's registrar to request.
	iRegistration.Registrar()->FillCallIdAndCSeqL(*aRequest);
	// update registration expires parameter from aRequest Contact header
	iRegistration.SetExpiresValueL(*aRequest);
	// copy To and From header from registration binding to request
	iRegistration.FillToFromL(*aRequest);

	iRegistration.UnregisterL(aTransactionId, aRequest);
	}

// -----------------------------------------------------------------------------
// CSIPUnregistering::ReceiveL
// -----------------------------------------------------------------------------
//	
void CSIPUnregistering::ReceiveL(TTransactionId aTransactionId,
		                         CSIPResponse*  aResponse)
	{
	__SIP_ASSERT_LEAVE(aResponse, KErrArgument);
	// the expires parameter value must be 0, other wise panic in debug
	// mode, leaves in release mode.
	__SIP_ASSERT_LEAVE(iRegistration.Contact().ExpiresParameter() == 0, 
		KErrGeneral);

	if (aResponse->Type() == CSIPResponse::E1XX)
		{
		// pass provisional response to owner. 
		PassResponseToOwnerL(aResponse, aTransactionId,
			iRegistration.RegistrationId());
		}
	else 
		{
        // any final response (response code start from 200-)
		// will terminate the binding. so this function is 
		// called in order to remove the binding. 
		iRegistration.ReceivedErrorResponseL();
		PassResponseToOwnerL(aResponse, aTransactionId,
			iRegistration.RegistrationId());
		iRegistration.SelfDetach(ETrue);
		iRegistration.ChangeState(iUnregistered);
		}
	}

// -----------------------------------------------------------------------------
// CSIPUnregistering::TransactionEnded
// -----------------------------------------------------------------------------
//
TInt CSIPUnregistering::TransactionEnded(TTransactionId aTransactionId, 
		                                 TInt           aReason)
	{
	if (aReason != KErrNone)
		{
		iRegistration.ChangeState(iUnregistered);
		}
	// pass error to owner.
	return (PassErrorToOwner(aReason, aTransactionId, 
		iRegistration.RegistrationId()));	
	}

// -----------------------------------------------------------------------------
// CSIPUnregistering::CSIPUnregistering
// -----------------------------------------------------------------------------
//
CSIPUnregistering::CSIPUnregistering(MRegistrationContext& aRegistration)
	: CSIPRegistrationState (aRegistration)
	{
	}
