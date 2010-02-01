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
// Name          : csipregistering.cpp
// Part of       : SIP Registration
// Version       : SIP/4.0 
//



#include "csipregistering.h"
#include "SipAssert.h"
#include "CSIPRegistrar.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipcontactheader.h"
#include "sipminexpiresheader.h"
#include "MRegistrationOwner.h"
#include "CSIPResponseUtility.h"
#include "siperr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSIPRegistering::NewL
// -----------------------------------------------------------------------------
//
CSIPRegistering* CSIPRegistering::NewL(MRegistrationContext& aRegistration)
	{
	CSIPRegistering* self = CSIPRegistering::NewLC(aRegistration);
	CleanupStack::Pop(self);
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSIPRegistering::NewLC
// -----------------------------------------------------------------------------
//
CSIPRegistering* CSIPRegistering::NewLC(MRegistrationContext& aRegistration)
	{
	CSIPRegistering* self = new (ELeave) CSIPRegistering(aRegistration);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistering::SetNeighbourStates
// -----------------------------------------------------------------------------
//	
void CSIPRegistering::SetNeighbourStates(
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
// CSIPRegistering::~CSIPRegistering
// -----------------------------------------------------------------------------
//
CSIPRegistering::~CSIPRegistering()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistering::CSIPRegistering
// -----------------------------------------------------------------------------
//
CSIPRegistering::CSIPRegistering(MRegistrationContext& aRegistration)
	: CSIPRegistrationState(aRegistration)
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistering::RegisterL
// -----------------------------------------------------------------------------
//	
void CSIPRegistering::RegisterL(TTransactionId&  aTransactionId, 
								CSIPRequest*     aRequest,
		                        CSIPRouteHeader* aOutboundProxy)
	{
    __SIP_ASSERT_LEAVE(aRequest, KErrArgument);

	// if registrar is not NULL, fill in CallId and CSeq to request
	if (iRegistration.Registrar())
		{
		iRegistration.Registrar()->FillCallIdAndCSeqL(*aRequest);
		}
	iRegistration.RegisterL(aTransactionId, aRequest, aOutboundProxy);
	}

// -----------------------------------------------------------------------------
// CSIPRegistering::ReceiveL
// -----------------------------------------------------------------------------
//
void CSIPRegistering::ReceiveL(TTransactionId aTransactionId, 
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
			User::LeaveIfError(PassErrorToOwner(err,aTransactionId));
			delete aResponse;
			iRegistration.SelfDetach(ETrue);
			iRegistration.ChangeState(iUnregistered); 
			}
		// if there is no error, call function Received2XXResponseL,
		// which will update bindings, start timer for non refresh binding
		else 
			{
			iRegistration.Received2XXResponseL(*aResponse,aTransactionId);	
			// pass response to owner
			PassResponseToOwnerL(aResponse, aTransactionId, 
				iRegistration.RegistrationId());
			//if EFalse, detach from lower layer only for non refresh binding.
			iRegistration.SelfDetach(EFalse);
			// for non refresh register, the request ownership is not passed 
			// to lower, it can be deleted after 2xx response received. 
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
			iRegistration.Received423ResponseL(*aResponse);
			iRegistration.ChangeState(iResponse423Received);
			iRegistration.CurrentState().RegisterL(
			    aTransactionId,iRegistration.Request(),NULL);
			delete aResponse;
			}
		// either response code is 423, but no MinExpires-header in response
		// or response code not 423
		else
			{
			// function ReceivedErrorResponseL removes binding.
			iRegistration.ReceivedErrorResponseL();
			// pass response to owner
			PassResponseToOwnerL(aResponse, aTransactionId);
			iRegistration.SelfDetach(ETrue);
			iRegistration.ChangeState(iUnregistered);
			}
		}
	else // response type == 1XX
		{
		// pass provisional response to owner
		PassResponseToOwnerL(aResponse, aTransactionId);
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistering::TransactionEnded
// -----------------------------------------------------------------------------
//
TInt CSIPRegistering::TransactionEnded(TTransactionId aTransactionId, 
									   TInt           aReason)
	{
	if (aReason != KErrNone)
		{
		// changes state to unregistered if error received.
		iRegistration.ChangeState(iUnregistered);
		}
	// pass error to owner.
	return (PassErrorToOwner(aReason, aTransactionId));	
	}
