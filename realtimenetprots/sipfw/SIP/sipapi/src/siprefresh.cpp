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
// Name          : SIPRefresh.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "sipdefs.h"
#include "siperr.h"
#include "SipAssert.h"
#include "sipstrings.h"
#include "sipclientconnection.h"
#include "siprefresh.h"
#include "sipclienttransaction.h"
#include "siprefreshassociation.h"
#include "transactionassociation.h"


#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIPRefresh::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRefresh* CSIPRefresh::NewL()
    {
    CSIPRefresh* self = CSIPRefresh::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRefresh* CSIPRefresh::NewLC()
    {
    CSIPRefresh* self = new (ELeave) CSIPRefresh();
    CleanupStack::PushL(self);    
    self->ConstructL();    
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::CSIPRefresh
// -----------------------------------------------------------------------------
//
CSIPRefresh::CSIPRefresh() : iState(EConstructing)
    {
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRefresh::ConstructL()
	{
	SIPStrings::OpenL();
	iState = EInactive;
	}

// -----------------------------------------------------------------------------
// CSIPRefresh::~CSIPRefresh
// If iState is EConstructing, string pool was not opened.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRefresh::~CSIPRefresh()
    {
    if (iClientTransaction)
        {
        iClientTransaction->RemoveRefresh();
        }

    if (iOwner)
        {
        iOwner->DeletingRefresh(*this, iRefreshId);
        }

    iRequestType.Close();
    if (iState != EConstructing)
    	{
    	SIPStrings::Close();
    	}
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::State
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRefresh::TState CSIPRefresh::State() const
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::IsStandAlone
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPRefresh::IsStandAlone() const
	{	
	return iOwner && (iOwner->CheckIfStandAlone() == KErrNone);
	}

// -----------------------------------------------------------------------------
// CSIPRefresh::SIPTransaction
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPClientTransaction* CSIPRefresh::SIPTransaction() const
    {
    return iClientTransaction;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::TerminateL
// No need to check if this is a standalone refresh, as UpdateRefreshL leaves
// with the proper error if this refresh isn't standalone.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPRefresh::TerminateL(CSIPMessageElements* aElements)
    {
    CheckStateL();
    return iOwner->UpdateRefreshL(*this, aElements, ETrue);
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::UpdateL
// No need to check if this is a standalone refresh, as UpdateRefreshL leaves
// with the proper error if this refresh isn't standalone.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPRefresh::UpdateL(CSIPMessageElements* aElements)
    {
	CheckStateL();
    return iOwner->UpdateRefreshL(*this, aElements, EFalse);
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::operator==
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPRefresh::operator==(const CSIPRefresh& aRefresh) const
    {
    if (!iRefreshId || !aRefresh.iRefreshId)
        {
        //If either of the ids is empty, the refreshes are not considired same,
        //unless the objects are actually same
        return this == &aRefresh;
        }

    return iRefreshId == aRefresh.iRefreshId;    
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::IntervalL
// Allow getting interval even before active state.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPRefresh::IntervalL() const
	{
	__ASSERT_ALWAYS(iOwner, User::Leave(KErrSIPResourceNotAvailable));

	return iOwner->TransactionAssociation().ClientConnectionL().
		RefreshIntervalL(iRefreshId);
	}

// -----------------------------------------------------------------------------
// CSIPRefresh::SetIntervalL
// Allow interval setting even before active state.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPRefresh::SetIntervalL(TUint aInterval)
	{
	__ASSERT_ALWAYS(iOwner, User::Leave(KErrSIPResourceNotAvailable));
    __ASSERT_ALWAYS(aInterval > 0, User::Leave(KErrArgument));

	return iOwner->TransactionAssociation().ClientConnectionL().
		SetRefreshIntervalL(iRefreshId, aInterval);
	}

// -----------------------------------------------------------------------------
// CSIPRefresh::SetRefreshOwner
// -----------------------------------------------------------------------------
//
void CSIPRefresh::SetRefreshOwner(MSIPRefreshAssociation& aAssoc)
    {
    iOwner = &aAssoc;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::RemoveRefreshOwner
// -----------------------------------------------------------------------------
//
void CSIPRefresh::RemoveRefreshOwner(const MSIPRefreshAssociation& aAssoc)
    {
    __SIP_ASSERT_RETURN(iOwner == &aAssoc, KErrUnknown);
    
    iOwner = NULL;    
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::ChangeState
// -----------------------------------------------------------------------------
//
void CSIPRefresh::ChangeState(CSIPRefresh::TState aNextState)
    {
    iState = aNextState;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::RefreshId
// -----------------------------------------------------------------------------
//
TUint32 CSIPRefresh::RefreshId() const
    {
    return iRefreshId;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::SetRefreshIdIfEmpty
// -----------------------------------------------------------------------------
//
void CSIPRefresh::SetRefreshIdIfEmpty(TUint32 aRefreshId)
    {    
    if (!iRefreshId)
        {        
        iRefreshId = aRefreshId;
        }
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::RequestType
// -----------------------------------------------------------------------------
//
RStringF CSIPRefresh::RequestType() const
    {
    return iRequestType;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::SetRequestType
// -----------------------------------------------------------------------------
//
void CSIPRefresh::SetRequestType(RStringF aType)
    {
    iRequestType.Close();
	iRequestType = aType.Copy();	
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::DoesMatch
// -----------------------------------------------------------------------------
//
TBool CSIPRefresh::DoesMatch(TUint32 aRefreshId) const
    {
    return aRefreshId == iRefreshId;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::AddTransaction
// Refresh is associated to a new client transaction. iClientTransaction might
// be non-NULL in case application has not deleted the previous transaction yet.
// -----------------------------------------------------------------------------
//
void CSIPRefresh::AddTransaction(CSIPClientTransaction& aTransaction)
    {
    //Remove possibly existing association to current transaction
    if (iClientTransaction)
        {
        iClientTransaction->RemoveRefresh();
        }

    iClientTransaction = &aTransaction;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::RemoveTransaction
// -----------------------------------------------------------------------------
//
void CSIPRefresh::RemoveTransaction()
    {
    __SIP_ASSERT_RETURN(iClientTransaction, KErrNotFound);

    iClientTransaction = NULL;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::Transaction
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CSIPRefresh::Transaction()
    {
    return iClientTransaction;
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::UpdateRefreshState
// -----------------------------------------------------------------------------
//
void CSIPRefresh::UpdateRefreshState(TUint aStatusCode)
    {
    if (aStatusCode >= 300)
        {
        ChangeState(CSIPRefresh::ETerminated);
        }
    else
        {
        if (aStatusCode >= 200)
            {
            ChangeState(CSIPRefresh::EActive);
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPRefresh::CheckStateL
// -----------------------------------------------------------------------------
//
void CSIPRefresh::CheckStateL() const
	{
	__ASSERT_ALWAYS(iState == EActive,
    				User::Leave(KErrSIPInvalidTransactionState));
    __ASSERT_ALWAYS(iOwner, User::Leave(KErrSIPResourceNotAvailable));
	}
