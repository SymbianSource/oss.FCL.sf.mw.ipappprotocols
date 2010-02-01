// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : sipdialogterminated.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0
//



#include "SipAssert.h"
#include "sipdialogterminated.h"
#include "SipDialogImplementation.h"
#include "sipclienttransaction.h"
#include "sipmessageelements.h"
#include "sipresponseelements.h"
#include "sipconnectioncallback.h"


// -----------------------------------------------------------------------------
// CDialogTerminated::NewL
// -----------------------------------------------------------------------------
//
CDialogTerminated* CDialogTerminated::NewL()
    {
    return new (ELeave) CDialogTerminated();
    }

// -----------------------------------------------------------------------------
// CDialogTerminated::CDialogTerminated
// -----------------------------------------------------------------------------
//
CDialogTerminated::CDialogTerminated()
	{
	}

// -----------------------------------------------------------------------------
// CDialogTerminated::~CDialogTerminated
// -----------------------------------------------------------------------------
//
CDialogTerminated::~CDialogTerminated()
	{
	}

// -----------------------------------------------------------------------------
// CDialogTerminated::State
// -----------------------------------------------------------------------------
//
CSIPDialog::TState CDialogTerminated::State() const
    {
    return CSIPDialog::ETerminated;
    }

// -----------------------------------------------------------------------------
// CDialogTerminated::IncomingResponseL
// If a transaction is found, response is passed to application also in
// terminated-state. Otherwise the response is ignored.
// -----------------------------------------------------------------------------
//
TBool CDialogTerminated::IncomingResponseL(CSIPDialogImplementation& aDialog,
                                           CSIPResponseElements* aElements,
		                                   TUint32 aRequestId,
                                           TUint32 /*aDialogId*/,
                                           CConnectionCallback& aCallback) const
    {
    __SIP_ASSERT_LEAVE(aElements, KErrArgument);

    CSIPTransactionBase* ta = NULL;
    CSIPDialogAssocBase* assoc = NULL;
    if (aDialog.FindTransactionAndAssoc(aRequestId, &ta, &assoc))
        {
        __SIP_ASSERT_LEAVE(ta->IsSIPClientTransaction(), KErrGeneral);

        CSIPClientTransaction* clientTa =
            static_cast<CSIPClientTransaction*>(ta);        
        clientTa->SetResponseElements(aElements);
        aCallback.Set(CConnectionCallback::EIncomingResponse,
                      ta,
                      NULL,
                      NULL,
                      assoc);
        return ETrue;
        }
    
    delete aElements;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDialogTerminated::IncomingResponseL
// Ignore the response
// -----------------------------------------------------------------------------
//
TBool
CDialogTerminated::IncomingResponseL(CSIPDialogImplementation& /*aDialog*/,
                                     CSIPResponseElements* aElements,
		                             TUint32 /*aRequestId*/,
                                     TUint32 /*aRefreshId*/,
                                     TUint32 /*aDialogId*/,
                                     CConnectionCallback& /*aCallback*/) const
    {
    __SIP_ASSERT_LEAVE(aElements, KErrArgument);
    
    delete aElements;
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// CDialogTerminated::ErrorOccurred
// Ignore the error
// -----------------------------------------------------------------------------
//
TBool
CDialogTerminated::ErrorOccured(CSIPDialogImplementation& /*aDialog*/,
                                TInt /*aError*/,
                                TUint32 /*aRequestId*/,
                                CConnectionCallback& /*aCallback*/) const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDialogTerminated::ErrorOccured
// Ignore the error
// -----------------------------------------------------------------------------
//
TBool CDialogTerminated::ErrorOccured(CSIPDialogImplementation& /*aDialog*/,
                                TInt /*aError*/,
                                TUint32 /*aRefreshId*/,
                                TUint32 /*aRequestId*/,
                                CConnectionCallback& /*aCallback*/) const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDialogTerminated::ConnectionLost
// -----------------------------------------------------------------------------
//
void
CDialogTerminated::ConnectionLost(CSIPDialogImplementation& /*aDialog*/) const
    {
    //no action
    }
