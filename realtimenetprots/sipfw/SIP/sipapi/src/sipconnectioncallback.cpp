// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : sipconnectioncallback.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "SipAssert.h"
#include "sipresponseelements.h"
#include "sipconnectioncallback.h"
#include "sipinvitedialogassoc.h"
#include "sipconnectionobserver.h"
#include "sipclienttransaction.h"
#include "sipservertransaction.h"
#include "sipdialog.h"
#include "sipregistrationbinding.h"


#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#endif


// -----------------------------------------------------------------------------
// CConnectionCallback::NewL
// -----------------------------------------------------------------------------
//
CConnectionCallback* CConnectionCallback::NewL()
    {    
    return new (ELeave) CConnectionCallback();
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::CConnectionCallback
// -----------------------------------------------------------------------------
//
CConnectionCallback::CConnectionCallback() :
    iCallbackId(CConnectionCallback::ENone)
    {
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::~CConnectionCallback
// -----------------------------------------------------------------------------
//
CConnectionCallback::~CConnectionCallback()
    {
    delete iObjectToDelete;
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::Set
// -----------------------------------------------------------------------------
//
void CConnectionCallback::Set(TCallbackMethod aCallbackId,
                              CSIPTransactionBase* aTransaction,
                              CSIPRegistrationBinding* aRegistration,
                              CSIPDialog* aDialog,
                              CSIPDialogAssocBase* aDialogAssoc,
                              TInt aError,
                              CBase* aObjectToDelete)
    {
    __SIP_ASSERT_RETURN(aCallbackId != CConnectionCallback::ENone,
                        KErrArgument);

	delete iObjectToDelete;
	iObjectToDelete = aObjectToDelete;

    iCallbackId = aCallbackId;
    iTransaction = aTransaction;
    iRegistration = aRegistration;
    iDialog = aDialog;
    iDialogAssoc = aDialogAssoc;
    iError = aError;
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::ExecuteCallback
// -----------------------------------------------------------------------------
//
void CConnectionCallback::ExecuteCallback(MSIPConnectionObserver& aObserver)
    {
    switch (iCallbackId)
        {
        case CConnectionCallback::EIncomingRequest:
            CallIncomingRequest(aObserver);
            break;            
        
        case CConnectionCallback::EIncomingResponse:
            CallIncomingResponse(aObserver);
            break;

        case CConnectionCallback::EIncomingResponseNewAssoc:
            CallIncomingResponseNewAssoc(aObserver);
            break;

        case CConnectionCallback::EIncomingResponseInRegistration:
            CallIncomingResponseInRegistration(aObserver);
            break;

        case CConnectionCallback::EErrorOccuredTransaction:
            CallErrorOccuredTransaction(aObserver);
            break;

        case CConnectionCallback::EErrorOccuredDialog:
            CallErrorOccuredDialog(aObserver);
            break;

        case CConnectionCallback::EErrorOccuredDialogTransaction:
            CallErrorOccuredDialogTransaction(aObserver);
            break;

        case CConnectionCallback::EErrorOccuredInRegistration:
            CallErrorOccuredInRegistration(aObserver);
            break;

        case CConnectionCallback::EErrorOccuredInRegistrationWithTransaction:
            CallErrorOccuredInRegistrationWithTransaction(aObserver);
            break;

        case CConnectionCallback::EInviteCanceled:
        	CallInviteCanceled(aObserver);
        	break;

        default:
            __ASSERT_DEBUG(EFalse,
		        User::Panic(_L("CConnCallback::ExecCallback() wrong id"),
		        KErrUnknown));
        }
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::CallIncomingRequest
// -----------------------------------------------------------------------------
//
void
CConnectionCallback::CallIncomingRequest(MSIPConnectionObserver& aObserver)
    {
    __SIP_ASSERT_RETURN(iTransaction, KErrArgument);
    __SIP_ASSERT_RETURN(iDialog, KErrArgument);

    aObserver.IncomingRequest(
        static_cast<CSIPServerTransaction*>(iTransaction),
        *iDialog);
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::CallIncomingResponse
// -----------------------------------------------------------------------------
//
void
CConnectionCallback::CallIncomingResponse(MSIPConnectionObserver& aObserver)
    {
    __SIP_ASSERT_RETURN(iTransaction, KErrArgument);
    __SIP_ASSERT_RETURN(iDialogAssoc, KErrArgument);    
        
    aObserver.IncomingResponse(
        static_cast<CSIPClientTransaction&>(*iTransaction),
        *iDialogAssoc);        
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::CallIncomingResponseNewAssoc
// -----------------------------------------------------------------------------
//
void CConnectionCallback::CallIncomingResponseNewAssoc(
                                        MSIPConnectionObserver& aObserver)
    {
    __SIP_ASSERT_RETURN(iTransaction, KErrArgument);
    __SIP_ASSERT_RETURN(iDialogAssoc, KErrArgument);    

    aObserver.IncomingResponse(
        static_cast<CSIPClientTransaction&>(*iTransaction),
        static_cast<CSIPInviteDialogAssoc*>(iDialogAssoc));
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::CallIncomingResponseInRegistration
// -----------------------------------------------------------------------------
//
void CConnectionCallback::CallIncomingResponseInRegistration(
                                        MSIPConnectionObserver& aObserver)
    {
    __SIP_ASSERT_RETURN(iTransaction, KErrArgument);
    __SIP_ASSERT_RETURN(iRegistration, KErrArgument);

    aObserver.IncomingResponse(
        static_cast<CSIPClientTransaction&>(*iTransaction),
        *iRegistration);        
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::CallErrorOccuredTransaction
// -----------------------------------------------------------------------------
//
void CConnectionCallback::CallErrorOccuredTransaction(
											MSIPConnectionObserver& aObserver)
	{
	__SIP_ASSERT_RETURN(iTransaction, KErrArgument);

	aObserver.ErrorOccured(iError, *iTransaction);
	}

// -----------------------------------------------------------------------------
// CConnectionCallback::CallErrorOccuredDialog
// -----------------------------------------------------------------------------
//
void
CConnectionCallback::CallErrorOccuredDialog(MSIPConnectionObserver& aObserver)
    {
    __SIP_ASSERT_RETURN(iDialogAssoc, KErrArgument);

    aObserver.ErrorOccured(iError, *iDialogAssoc);
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::CallErrorOccuredDialogTransaction
// -----------------------------------------------------------------------------
//
void CConnectionCallback::CallErrorOccuredDialogTransaction(
                                            MSIPConnectionObserver& aObserver)
    {
    __SIP_ASSERT_RETURN(iTransaction, KErrArgument);
    __SIP_ASSERT_RETURN(iDialogAssoc, KErrArgument);

    aObserver.ErrorOccured(iError, *iTransaction, *iDialogAssoc);
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::CallErrorOccuredInRegistration
// -----------------------------------------------------------------------------
//
void CConnectionCallback::CallErrorOccuredInRegistration(
                                         MSIPConnectionObserver& aObserver)
    {
    __SIP_ASSERT_RETURN(iRegistration, KErrArgument);
    
    aObserver.ErrorOccured(iError, *iRegistration);
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::CallErrorOccuredInRegistrationWithTransaction
// -----------------------------------------------------------------------------
//
void CConnectionCallback::CallErrorOccuredInRegistrationWithTransaction(
                                            MSIPConnectionObserver& aObserver)
    {
    __SIP_ASSERT_RETURN(iTransaction, KErrArgument);
    __SIP_ASSERT_RETURN(iRegistration, KErrArgument);
            
    aObserver.ErrorOccured(iError,
                           static_cast<CSIPClientTransaction&>(*iTransaction),
                           *iRegistration);
    }

// -----------------------------------------------------------------------------
// CConnectionCallback::CallInviteCanceled
// -----------------------------------------------------------------------------
//
void CConnectionCallback::CallInviteCanceled(MSIPConnectionObserver& aObserver)
	{
	__SIP_ASSERT_RETURN(iTransaction, KErrArgument);

	aObserver.InviteCanceled(
		static_cast<CSIPServerTransaction&>(*iTransaction));
	}
