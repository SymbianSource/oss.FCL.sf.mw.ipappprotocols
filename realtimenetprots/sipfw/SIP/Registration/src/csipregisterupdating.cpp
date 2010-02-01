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
// Name          : csipregisterupdating.cpp
// Part of       : SIP Registration
// Version       : SIP/4.0 
//



#include "csipregisterupdating.h"
#include "csip423responsereceived.h"
#include "sipcontactheader.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "CSIPRegistrar.h"
#include "SipAssert.h"
#include "sipminexpiresheader.h"
#include "MRegistrationOwner.h"
#include "CSIPResponseUtility.h"
#include "siperr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSIPRegisterUpdating::NewL
// -----------------------------------------------------------------------------
//
CSIPRegisterUpdating* CSIPRegisterUpdating::NewL(
	MRegistrationContext& aRegistration)
	{
	CSIPRegisterUpdating* self = CSIPRegisterUpdating::NewLC(aRegistration);
	CleanupStack::Pop(self);
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSIPRegisterUpdating::NewLC
// -----------------------------------------------------------------------------
//
CSIPRegisterUpdating* CSIPRegisterUpdating::NewLC(
	MRegistrationContext& aRegistration)
	{
	CSIPRegisterUpdating* self =
		new (ELeave) CSIPRegisterUpdating(aRegistration);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegisterUpdating::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CSIPRegisterUpdating::SetNeighbourStates(
	CSIPRegistrationState* aResponse423Received,
	CSIPRegistrationState* aRegistered,
	CSIPRegistrationState* aUnregistered)
	{
	__SIP_ASSERT_RETURN(aResponse423Received, KErrArgument);
    __SIP_ASSERT_RETURN(aRegistered, KErrArgument);
	__SIP_ASSERT_RETURN(aUnregistered, KErrArgument);

    iResponse423Received = aResponse423Received;
	iRegistered = aRegistered;
	iUnregistered = aUnregistered;
	}

// -----------------------------------------------------------------------------
// CSIPRegisterUpdating::~CSIPRegisterUpdating
// -----------------------------------------------------------------------------
//
CSIPRegisterUpdating::~CSIPRegisterUpdating()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegisterUpdating::UpdateL
// -----------------------------------------------------------------------------
//	
void CSIPRegisterUpdating::UpdateL(TTransactionId& aTransactionId,
				                   CSIPRequest*    aRequest)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);

	CSIPContactHeader* contact = static_cast<CSIPContactHeader*>(
		aRequest->Header(SIPStrings::StringF(SipStrConsts::EContactHeader),0));

	__SIP_ASSERT_LEAVE(contact != 0, KErrArgument);
		               
	__SIP_ASSERT_LEAVE(contact->ExpiresParameter() > 0, KErrArgument);

	// registrar must not be NULL after registering state.
	__SIP_ASSERT_LEAVE(iRegistration.Registrar(), KErrGeneral);

	// update registration expires parameter from aRequest Contact header
	iRegistration.SetExpiresValueL(*aRequest);
	// copy CallId and CSeq to request
	iRegistration.Registrar()->FillCallIdAndCSeqL(*aRequest);
	// copy To and From header to request
	iRegistration.FillToFromL(*aRequest);

	iRegistration.UpdateL(aTransactionId, aRequest);
	}

// -----------------------------------------------------------------------------
// CSIPRegisterUpdating::ReceiveL
// -----------------------------------------------------------------------------
//
void CSIPRegisterUpdating::ReceiveL(TTransactionId aTransactionId,
		                            CSIPResponse*  aResponse)
	{
	__SIP_ASSERT_LEAVE(aResponse, KErrArgument);
	// the expires parameter value must not be 0, other wise panic in debug
	// mode, leaves in release mode.
	__SIP_ASSERT_LEAVE(iRegistration.Contact().ExpiresParameter() > 0, 
		KErrGeneral);

	if (aResponse->Type() == CSIPResponse::E2XX)
		{
		// ConnectionMgr may have changed the the host part of the Contact
		iRegistration.UpdateContactFromRequestL();
		if (iRegistration.Request())
			{
			iRegistration.Request()->DeleteHeaders(
				SIPStrings::StringF(SipStrConsts::ERouteHeader));
			}
        TInt err = 
            CSIPResponseUtility::Check2XXResponse(iRegistration,*aResponse);
        if (err)
			{
			iRegistration.ReceivedErrorResponseL();
			User::LeaveIfError(
			    PassErrorToOwner(err,aTransactionId,
			                     iRegistration.RegistrationId()));
			delete aResponse;
			iRegistration.SelfDetach(ETrue);
			iRegistration.ChangeState(iUnregistered); 
			}
		// if there is no error, call function Received2XXResponseL,
		// which will update bindings, start timer for non refresh binding
		else 
			{
			// function update the binding from response.
			iRegistration.Received2XXResponseL(*aResponse,aTransactionId);
			// pass response to owner
			PassResponseToOwnerL(aResponse, aTransactionId, 
				iRegistration.RegistrationId());
			iRegistration.SelfDetach(EFalse);
			// if the binding is non refresh binding, the request is deleted
			// after 2xx received
			iRegistration.ClearRequest();
			iRegistration.ChangeState(iRegistered); 
			}
		}
	else if (aResponse->IsErrorResponse())
		{
		// if 423 response is received and there is min expires header,
		// set min expires header from response to request and send 
		// the request again.
        const RStringF KMinExpiresHeaderName = 
            SIPStrings::StringF(SipStrConsts::EMinExpiresHeader);		
		
		if (aResponse->ResponseCode() == K423Response && 
			aResponse->HasHeader(KMinExpiresHeaderName))
			{
			// function set min expires header from response to request.
			iRegistration.Received423ResponseL(*aResponse);
			iRegistration.ChangeState(iResponse423Received);
			static_cast<CSIP423ResponseReceived*>(iResponse423Received)->
				SetUpdate(ETrue);
			iRegistration.CurrentState().RegisterL(
			    aTransactionId,iRegistration.Request(),NULL);
			delete aResponse;
			}
		// either response code is 423, but no MinExpires-header in response
		// or response code not 423
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
	// response type == 1XX
	else
		{
		// pass provisional response to owner
		PassResponseToOwnerL(aResponse, aTransactionId, 
			iRegistration.RegistrationId());
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegisterUpdating::TransactionEnded
// -----------------------------------------------------------------------------
//
TInt CSIPRegisterUpdating::TransactionEnded(TTransactionId aTransactionId, 
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
// CSIPRegisterUpdating::CSIPRegisterUpdating
// -----------------------------------------------------------------------------
//
CSIPRegisterUpdating::CSIPRegisterUpdating(MRegistrationContext& aRegistration)
	: CSIPRegistrationState (aRegistration)
	{
	}
