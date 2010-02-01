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
// Name          : SIPServerTransaction.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "sip.h"
#include "siperr.h"
#include "SipAssert.h"
#include "transactionassociation.h"
#include "sipservertransaction.h"
#include "siprequestelements.h"
#include "csipresponsesender.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIPServerTransaction::NewL
// -----------------------------------------------------------------------------
//
CSIPServerTransaction*
CSIPServerTransaction::NewL(TUint32 aRequestId,
                            MTransactionAssociation& aAssociation,
                            CSIPRequestElements* aElements)
    {
    CSIPServerTransaction* self =
        CSIPServerTransaction::NewLC(aRequestId, aAssociation, aElements);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::NewLC
// -----------------------------------------------------------------------------
//
CSIPServerTransaction*
CSIPServerTransaction::NewLC(TUint32 aRequestId,
                             MTransactionAssociation& aAssociation,
                             CSIPRequestElements* aElements)
    {
    __ASSERT_ALWAYS(aElements, User::Leave(KErrArgument));

    CSIPServerTransaction* self =
        new (ELeave) CSIPServerTransaction(aRequestId, aAssociation);
    CleanupStack::PushL(self);
    self->ConstructL(aElements);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::CSIPServerTransaction
// -----------------------------------------------------------------------------
//
CSIPServerTransaction::CSIPServerTransaction(TUint32 aRequestId,
                                    MTransactionAssociation& aAssociation ) :
    CSIPTransactionBase(EFalse, aRequestId, aAssociation)
    {
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPServerTransaction::ConstructL(CSIPRequestElements* aElements)
    {
    __ASSERT_ALWAYS(aElements, User::Leave(KErrArgument));

    CSIPTransactionBase::ConstructL(aElements->Method());

    //Create default sender
    iResponseSender = CSIPResponseSender::NewL();

    //Take ownership of aElements when leave can't occur
    iRequestElements = aElements;
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::~CSIPServerTransaction
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPServerTransaction::~CSIPServerTransaction()
    {
    delete iRequestElements;
    delete iResponseSender;    
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::SendResponseL
// CSIPResponseElements checks the response is not a 100
// -----------------------------------------------------------------------------
//
EXPORT_C void
CSIPServerTransaction::SendResponseL(CSIPResponseElements* aElements)
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(aElements, User::Leave(KErrArgument));
    __ASSERT_ALWAYS(ResponseAllowed(), User::Leave(KErrGeneral));

    CSIPTransactionBase::TState state = StateL();
    __ASSERT_ALWAYS(state == CSIPTransactionBase::ETrying ||
                    state == CSIPTransactionBase::EProceeding,
                    User::Leave(KErrSIPInvalidTransactionState));    

    CheckAssociationL();
    iResponseSender->SendResponseL(iAssociation->ClientConnectionL(),
                                   *aElements,
                                   RequestId(),
                                   AffectsDialogState(),
                                   IsTargetRefresh(Type()));
    SetResponseElements(aElements);

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::RequestElements
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPRequestElements*
CSIPServerTransaction::RequestElements() const
    {
    __TEST_INVARIANT;
    return iRequestElements;
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::ResponseAllowed
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPServerTransaction::ResponseAllowed() const
    {
    __TEST_INVARIANT;
    return (Type() != SIPStrings::StringF(SipStrConsts::EAck));
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::SetResponseSender
// -----------------------------------------------------------------------------
//
void CSIPServerTransaction::SetResponseSender(MSIPResponseSender* aSender)
    {
    __TEST_INVARIANT;
    __ASSERT_DEBUG(aSender,
		User::Panic(_L("CSIPServerTransaction::SetRespSender() aSender = 0"),
		KErrArgument));

    delete iResponseSender;
    iResponseSender = aSender;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::Dialog
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation* CSIPServerTransaction::Dialog() const
	{
	__TEST_INVARIANT;
	return iResponseSender->Dialog();
	}

// -----------------------------------------------------------------------------
// CSIPServerTransaction::SIPConnectionL
// -----------------------------------------------------------------------------
//
CSIPConnection& CSIPServerTransaction::SIPConnectionL()
    {
    CheckAssociationL();

    return iAssociation->SIPConnectionL();
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::ReAssociateL
// -----------------------------------------------------------------------------
//
void CSIPServerTransaction::ReAssociateL(MTransactionAssociation& aAssociation)
    {
    __TEST_INVARIANT;
    CheckAssociationL();

    //Attach transaction to the new association
    aAssociation.AddTransactionL(*this);
    
    //Clear previous association only after attaching has succeeded
    iAssociation->RemoveTransaction(*this);

    iAssociation = &aAssociation;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPServerTransaction::DetachRequestElements
// -----------------------------------------------------------------------------
//
void CSIPServerTransaction::DetachRequestElements(TAny* aServerTransaction)
	{
    __SIP_ASSERT_RETURN(aServerTransaction, KErrArgument);

    reinterpret_cast<CSIPServerTransaction*>(aServerTransaction)->
        iRequestElements = NULL;
	}

// -----------------------------------------------------------------------------
// CSIPServerTransaction::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CSIPServerTransaction::__DbgTestInvariant() const
	{
	if (iResponseSender == NULL)
		{	
		User::Invariant();
		}
	}
