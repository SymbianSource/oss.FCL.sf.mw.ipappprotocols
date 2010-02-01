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
// Name          : csipregistered.cpp
// Part of       : SIP Registration
// Version       : SIP/4.0 
//



#include "csipregistered.h"
#include "csip423responsereceived.h"
#include "SipAssert.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipminexpiresheader.h"
#include "CSIPResponseUtility.h"
#include "MRegistrationOwner.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "siperr.h"

// -----------------------------------------------------------------------------
// CSIPRegistered::NewL
// -----------------------------------------------------------------------------
//
CSIPRegistered* CSIPRegistered::NewL(MRegistrationContext& aRegistration)
	{
	CSIPRegistered* self = CSIPRegistered::NewLC(aRegistration);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistered::NewLC
// -----------------------------------------------------------------------------
//	
CSIPRegistered* CSIPRegistered::NewLC(MRegistrationContext& aRegistration)
	{
	CSIPRegistered* self = new (ELeave) CSIPRegistered(aRegistration);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistered::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CSIPRegistered::SetNeighbourStates(
	CSIPRegistrationState* aUpdating,
	CSIPRegistrationState* aUnregistering,
	CSIPRegistrationState* aUnregistered,
	CSIPRegistrationState* aResponse423Received)
	{
	__SIP_ASSERT_RETURN(aUpdating, KErrArgument);
    __SIP_ASSERT_RETURN(aUnregistering, KErrArgument);
	__SIP_ASSERT_RETURN(aUnregistered, KErrArgument);
	__SIP_ASSERT_RETURN(aResponse423Received, KErrArgument);

   	iUpdating = aUpdating;
    iUnregistering = aUnregistering;
    iUnregistered = aUnregistered;
	iResponse423Received = aResponse423Received; 
	}

// -----------------------------------------------------------------------------
// CSIPRegistered::~CSIPRegistered
// -----------------------------------------------------------------------------
//	
CSIPRegistered::~CSIPRegistered()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistered::UpdateL
// -----------------------------------------------------------------------------
//		
void CSIPRegistered::UpdateL(TTransactionId& aTransactionId,
				             CSIPRequest*    aRequest)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);

	iRegistration.ChangeState(iUpdating);
	iRegistration.CurrentState().UpdateL(aTransactionId, aRequest);
	}

// -----------------------------------------------------------------------------
// CSIPRegistered::UnregisterL
// -----------------------------------------------------------------------------
//		
void CSIPRegistered::UnregisterL(TTransactionId& aTransactionId,
					             CSIPRequest*    aRequest)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);

	iRegistration.ChangeState(iUnregistering);
	iRegistration.CurrentState().UnregisterL(aTransactionId, aRequest);
	}

// -----------------------------------------------------------------------------
// CSIPRegistered::ReceiveL
//
// this function is called when subsequent refresh response is received
// -----------------------------------------------------------------------------
//		
void CSIPRegistered::ReceiveL(TTransactionId aTransactionId,
		                      CSIPResponse*  aResponse)
	{
	__SIP_ASSERT_LEAVE(aResponse, KErrArgument);

	if (aResponse->Type() == CSIPResponse::E2XX) // 200-299
		{
		if (!iRegistration.ServiceRouteOk(*aResponse))
			{
			iRegistration.ReceivedErrorResponseL();
			User::LeaveIfError(
			    PassErrorToOwner(KErrSIPTransportFailure,aTransactionId,
			                     iRegistration.RegistrationId()));
			delete aResponse;
			iRegistration.SelfDetach(ETrue);
			iRegistration.ChangeState(iUnregistered);
			return;
			}
		iRegistration.SetRouteHeaderFromResponseL(*aResponse);
		iRegistration.Received2XXResponseL(*aResponse,aTransactionId);
		if (iRegistration.Request())
			{
			iRegistration.Request()->DeleteHeaders(
				SIPStrings::StringF(SipStrConsts::ERouteHeader));
			}

		// don't forward subsequent 2xx response to owner
	    delete aResponse; 
		}
	else if (aResponse->IsErrorResponse()) // 300-
		{
        const RStringF KMinExpiresHeaderName = 
            SIPStrings::StringF(SipStrConsts::EMinExpiresHeader);		
		
		if (aResponse->ResponseCode() == K423Response &&
			aResponse->HasHeader(KMinExpiresHeaderName))
			{
			iRegistration.Received423ResponseL(*aResponse);
			iRegistration.ChangeState(iResponse423Received);	
			static_cast<CSIP423ResponseReceived*>(iResponse423Received)->
				SetUpdate(ETrue);
			iRegistration.CurrentState().RegisterL(
			    aTransactionId,iRegistration.Request(),NULL);
			delete aResponse;
			}
		// this happens when error response received from subsequent register.
		else
			{
			// ReceivedErrorResponseL, remove binding 
			iRegistration.ReceivedErrorResponseL();
			// pass response to owner
			PassResponseToOwnerL(aResponse, aTransactionId, 
				iRegistration.RegistrationId());
			iRegistration.SelfDetach(ETrue);
			iRegistration.ChangeState(iUnregistered);
			}
		}
	else // is provisional response 100-199
		{
		// do not forward subsequent 1xx response to owner
		delete aResponse; 
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistered::TransactionEnded
// -----------------------------------------------------------------------------
//		
TInt CSIPRegistered::TransactionEnded(TTransactionId aTransactionId, 
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
// CSIPRegistered::CSIPRegistered
// -----------------------------------------------------------------------------
//	
CSIPRegistered::CSIPRegistered(MRegistrationContext& aRegistration)
	: CSIPRegistrationState (aRegistration) 
	{
	}
