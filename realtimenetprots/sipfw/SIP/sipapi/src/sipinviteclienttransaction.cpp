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
// Name          : SIPInviteClientTransaction.cpp
// Part of       : SIPAPI
// Version       : SIP/5.0
//



#include "sip.h"
#include "SipAssert.h"
#include "transactionassociation.h"
#include "sipinviteclienttransaction.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIPInviteClientTransaction::NewL
// -----------------------------------------------------------------------------
//
CSIPInviteClientTransaction*
CSIPInviteClientTransaction::NewL(MTransactionAssociation& aAssociation)
    {
    CSIPInviteClientTransaction* self =
        CSIPInviteClientTransaction::NewLC(aAssociation);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPInviteClientTransaction::NewLC
// -----------------------------------------------------------------------------
//
CSIPInviteClientTransaction*
CSIPInviteClientTransaction::NewLC(MTransactionAssociation& aAssociation)
    {
    CSIPInviteClientTransaction* self =
        new (ELeave) CSIPInviteClientTransaction(aAssociation);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPInviteClientTransaction::CSIPInviteClientTransaction
// -----------------------------------------------------------------------------
//
CSIPInviteClientTransaction::CSIPInviteClientTransaction(
                                     MTransactionAssociation& aAssociation) :
    CSIPClientTransaction(aAssociation, NULL)
#ifdef CPPUNIT_TEST
    , iAssociationArray(1)
#endif
    {
    }

// -----------------------------------------------------------------------------
// CSIPInviteClientTransaction::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPInviteClientTransaction::ConstructL()
    {
    iAssociationArray.AppendL(iAssociation);

    CSIPTransactionBase::ConstructL(SIPStrings::StringF(SipStrConsts::EInvite));
    }

// -----------------------------------------------------------------------------
// CSIPInviteClientTransaction::~CSIPInviteClientTransaction
// RemoveTransaction event is sent to all associations.
// CSIPTransactionBase::iAssociation is set to NULL to prevent
// ~CSIPTransactionBase() from also sending RemoveTransaction.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteClientTransaction::~CSIPInviteClientTransaction()
    {
    for (TInt i = 0; i < iAssociationArray.Count(); i++)
        {
        iAssociationArray[i]->RemoveTransaction(*this);
        }
    iAssociationArray.Reset();

    iAssociation = NULL;
    }

// -----------------------------------------------------------------------------
// CSIPInviteClientTransaction::CancelAllowed
// It is possible to cancel an INVITE client transaction, though it can be done
// only in "proceeding state". This method just indicates whether canceling is
// possible at all, and does not check the current state.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPInviteClientTransaction::CancelAllowed() const
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPInviteClientTransaction::Detach
// iAssociation is set to NULL when the last association is detached. Until that
// happens it is set point one of the associations (does not matter which one).
// -----------------------------------------------------------------------------
//
void CSIPInviteClientTransaction::Detach(
                                 const MTransactionAssociation& aAssociation)
    {
    TInt pos = iAssociationArray.Find(&aAssociation);

    __ASSERT_DEBUG(pos != KErrNotFound,
		User::Panic(_L("CSIPInviteClientTransaction::Detach() not found"),
		KErrNotFound));

    if (pos != KErrNotFound)
        {
        iAssociationArray.Remove(pos);
        }

    if (iAssociationArray.Count() > 0)
        {
        iAssociation = iAssociationArray[0];
        }
    else
        {
        iAssociation = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CSIPInviteClientTransaction::AddAssociationL
// -----------------------------------------------------------------------------
//
void CSIPInviteClientTransaction::AddAssociationL(
                                        MTransactionAssociation& aAssociation)
    {
    __ASSERT_ALWAYS(iAssociationArray.Find(&aAssociation) == KErrNotFound,
        User::Leave(KErrAlreadyExists));

    aAssociation.AddTransactionL(*this);

    TInt err = iAssociationArray.Append(&aAssociation);
    if (err != KErrNone)
        {
        aAssociation.RemoveTransaction(*this);
        User::Leave(err);
        }
    }
