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
// Name          : SIPClientTransaction.cpp
// Part of       : SIPAPI
// Version       : SIP/5.0
//



#include "sip.h"
#include "siperr.h"
#include "SipAssert.h"
#include "SipDialogImplementation.h"
#include "sipclienttransaction.h"
#include "SipConnectionImplementation.h"
#include "siprefresh.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIPClientTransaction::NewL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPClientTransaction::NewL(RStringF aType,
                            MTransactionAssociation& aAssociation,
                            CSIPRefresh* aRefresh)
    {
    CSIPClientTransaction* self = CSIPClientTransaction::NewLC(aType,
                                                               aAssociation,
                                                               aRefresh);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPClientTransaction::NewLC
// For INVITE client transaction, CSIPInviteClientTransaction must be used.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPClientTransaction::NewLC(RStringF aType,
                             MTransactionAssociation& aAssociation,
                             CSIPRefresh* aRefresh)
    {
    __ASSERT_ALWAYS(aType != SIPStrings::StringF(SipStrConsts::EInvite),
                    User::Leave(KErrArgument));

    CSIPClientTransaction* self =
        new (ELeave) CSIPClientTransaction(aAssociation, aRefresh);
    CleanupStack::PushL(self);
    self->CSIPTransactionBase::ConstructL(aType);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPClientTransaction::CSIPClientTransaction
// RequestId is not yet known.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction::CSIPClientTransaction(
	MTransactionAssociation& aAssociation,
	CSIPRefresh* aRefresh) :
    CSIPTransactionBase(ETrue, 0, aAssociation),
    iRefresh(aRefresh)
    {
    if (iRefresh)
        {
        iRefresh->AddTransaction(*this);
        }
    }

// -----------------------------------------------------------------------------
// CSIPClientTransaction::~CSIPClientTransaction
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction::~CSIPClientTransaction()
    {
    if (iRefresh)
        {
        iRefresh->RemoveTransaction();
        }
    }

// -----------------------------------------------------------------------------
// CSIPClientTransaction::ResponseElements
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPResponseElements*
CSIPClientTransaction::ResponseElements() const
    {
    return CSIPTransactionBase::ResponseElements();
    }

// -----------------------------------------------------------------------------
// CSIPClientTransaction::CancelL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction* CSIPClientTransaction::CancelL()
    {
    __ASSERT_ALWAYS(CancelAllowed(),
        User::Leave(KErrSIPInvalidTransactionState));
    __ASSERT_ALWAYS(StateL() == CSIPTransactionBase::EProceeding,
        User::Leave(KErrSIPInvalidTransactionState));

    CheckAssociationL();
    CSIPDialogImplementation* dialog = iAssociation->SIPConnectionL().
    	Implementation().FindDialogByRequestId(iRequestId);

    if (!dialog)
        {
        User::Leave(KErrSIPInvalidTransactionState);
        }

    //Don't pass iAssociation to SendCancelL as user may have deleted the dialog
    //association used by this transaction. In that case iAssociation is NULL.
    //Dialog searches the dialog association with requestId. If it can't find
    //it, something is wrong and CANCEL can't be sent.
    return dialog->SendCancelL(iRequestId);
    }

// -----------------------------------------------------------------------------
// CSIPClientTransaction::Refresh
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPRefresh* CSIPClientTransaction::Refresh() const
    {
    return iRefresh;
    }

// -----------------------------------------------------------------------------
// CSIPClientTransaction::CancelAllowed
// Only INVITE client transaction can be canceled
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPClientTransaction::CancelAllowed() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPClientTransaction::SetRequestId
// -----------------------------------------------------------------------------
//
void CSIPClientTransaction::SetRequestId(TUint32 aRequestId)
	{
	__SIP_ASSERT_RETURN(!iRequestId, KErrAlreadyExists);
	__SIP_ASSERT_RETURN(aRequestId, KErrArgument);
	
	iRequestId = aRequestId;
	}

// -----------------------------------------------------------------------------
// CSIPClientTransaction::Refresh
// -----------------------------------------------------------------------------
//
CSIPRefresh* CSIPClientTransaction::Refresh()
    {
    return iRefresh;
    }

// -----------------------------------------------------------------------------
// CSIPClientTransaction::RemoveRefresh
// -----------------------------------------------------------------------------
//
void CSIPClientTransaction::RemoveRefresh()
    {
    __SIP_ASSERT_RETURN(iRefresh, KErrNotFound);

    iRefresh = NULL;
    }
