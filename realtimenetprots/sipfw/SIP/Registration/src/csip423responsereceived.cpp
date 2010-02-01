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
// Name          : csip423responsereceived.cpp
// Part of       : SIP Registration
// Version       : SIP/4.0 
//



#include "csip423responsereceived.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipcseqheader.h"
#include "SipAssert.h"
#include "MRegistrationOwner.h"
#include "sipcontactheader.h"
#include "CSIPRegistrar.h"
#include "CSIPResponseUtility.h"
#include "siperr.h"


// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::NewL
// -----------------------------------------------------------------------------
//
CSIP423ResponseReceived* CSIP423ResponseReceived::NewL(
	MRegistrationContext& aRegistration)
	{
	CSIP423ResponseReceived* self = 
		CSIP423ResponseReceived::NewLC(aRegistration);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::NewLC
// -----------------------------------------------------------------------------
//
CSIP423ResponseReceived* CSIP423ResponseReceived::NewLC(
	MRegistrationContext& aRegistration)
	{
	CSIP423ResponseReceived* self = 
		new (ELeave) CSIP423ResponseReceived(aRegistration);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CSIP423ResponseReceived::SetNeighbourStates(
	CSIPRegistrationState* aRegistered,
    CSIPRegistrationState* aUnregistered)
	{
	__SIP_ASSERT_RETURN(aRegistered, KErrArgument);
    __SIP_ASSERT_RETURN(aUnregistered, KErrArgument);

	iRegistered = aRegistered;
    iUnregistered = aUnregistered;
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::~CSIP423ResponseReceived
// -----------------------------------------------------------------------------
//
CSIP423ResponseReceived::~CSIP423ResponseReceived()
	{
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::RegisterL
// -----------------------------------------------------------------------------
//
void CSIP423ResponseReceived::RegisterL(TTransactionId&  aTransactionId, 
	  	                                CSIPRequest*     aRequest,
		                                CSIPRouteHeader* aOutboundProxy)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);

	CSIPCSeqHeader* cseq = aRequest->CSeq();
	if (iRegistration.Registrar())
		{	
		// if there is Registrar, the CallId and CSeq must be filled-in
		// from CSIPRegistering state. the CSeq need to be increased for 
		// sending REGISTER request again in this state. so panic if there is 
		// no CSeq header in debug mode,leave in release mode
		__SIP_ASSERT_LEAVE(cseq != 0, KErrGeneral);
		// increase the cseq for registrar
		iRegistration.Registrar()->IncreaseCSeqNumber();
		cseq->SetSeq(iRegistration.Registrar()->CSeqNumber());
		}
	else
		{	
		// to check if the CSeq has been filled by lower layer,
		// if so, increase the CSeq by one before sending the request.
		if (cseq)
			{
			cseq->SetSeq(cseq->Seq()+1);
			}
		}

	iRegistration.RegisterL(aTransactionId, aRequest, aOutboundProxy);
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::ReceiveL
// -----------------------------------------------------------------------------
//
void CSIP423ResponseReceived::ReceiveL(TTransactionId aTransactionId,
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
        TInt err = 
            CSIPResponseUtility::Check2XXResponse(iRegistration,*aResponse);		
		if (err)
			{
			iRegistration.ReceivedErrorResponseL();
			PassErrorL(err,aTransactionId);
			delete aResponse;
			iRegistration.SelfDetach(ETrue);
			iRegistration.ChangeState(iUnregistered); 
			}
		else
			{
			iRegistration.Received2XXResponseL(*aResponse,aTransactionId);
			// pass response to owner, registration id should be passed
			// since 2xx ok response received.
			PassResponseToOwnerL(aResponse, aTransactionId, 
				iRegistration.RegistrationId());
				
			// if EFalse,detach from lowerlayer only for nonrefresh binding.
			// Use overloaded version of SelfDetatch function
			// to detatch retry transaction.
			iRegistration.SelfDetach(EFalse, ETrue);
			
			// if the binding is non refresh binding, the request is deleted.
			iRegistration.ClearRequest();
			iRegistration.ChangeState(iRegistered); 
			}
		}
	else if (aResponse->IsErrorResponse())
		{
		// ReceivedErrorResponseL, remove binding 
		iRegistration.ReceivedErrorResponseL();
		// pass response to owner
		PassResponseL(aResponse, aTransactionId);
		iRegistration.SelfDetach(ETrue);
		iRegistration.ChangeState(iUnregistered);
		}
	else // response type == 1XX
		{
		// pass provisional response to owner
		PassResponseL(aResponse, aTransactionId);
		}
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::TransactionEnded
// -----------------------------------------------------------------------------
//
TInt CSIP423ResponseReceived::TransactionEnded(TTransactionId aTransactionId, 
		                                       TInt           aReason)
	{
	if (aReason != KErrNone)
		{
		iRegistration.ChangeState(iUnregistered);
		}
	// pass error to owner. 
	if (iUpdate)
		{
		return (PassErrorToOwner(aReason, aTransactionId,
			iRegistration.RegistrationId()));	
		}
	else
		{
		return (PassErrorToOwner(aReason, aTransactionId));
		}
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::Retrying
// -----------------------------------------------------------------------------
//

TBool CSIP423ResponseReceived::Retrying() const

	{
	return ETrue;	
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::SetUpdate
// -----------------------------------------------------------------------------
//
void CSIP423ResponseReceived::SetUpdate(TBool aUpdate)
	{
	iUpdate = aUpdate;
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::CSIP423ResponseReceived
// -----------------------------------------------------------------------------
//
CSIP423ResponseReceived::CSIP423ResponseReceived(
	MRegistrationContext& aRegistration) 
	: CSIPRegistrationState (aRegistration),
	iUpdate                 (EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::PassResponseL
// -----------------------------------------------------------------------------
//
void CSIP423ResponseReceived::PassResponseL(CSIPResponse*   aResponse,
											TTransactionId  aTransactionId)
	{
	if (iUpdate)
		{
		PassResponseToOwnerL(aResponse, aTransactionId, 
			iRegistration.RegistrationId());
		}
	else
		{
		PassResponseToOwnerL(aResponse, aTransactionId);
		}
	}

// -----------------------------------------------------------------------------
// CSIP423ResponseReceived::PassErrorL
// -----------------------------------------------------------------------------
//
void CSIP423ResponseReceived::PassErrorL(TInt aError, 
										 TTransactionId  aTransactionId)
	{
	if (iUpdate)
		{
		User::LeaveIfError(PassErrorToOwner(aError, aTransactionId,
			iRegistration.RegistrationId()));	
		}
	else
		{
		User::LeaveIfError(PassErrorToOwner(aError, aTransactionId));
		}
	}
