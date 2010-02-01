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
// Name          : SIPTransactionBase.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "sip.h"
#include "SipImplementation.h"
#include "sipconnection.h"
#include "sipclient.h"
#include "siperr.h"
#include "SipAssert.h"
#include "siptransactionbase.h"
#include "transactionassociation.h"
#include "sipresponseelements.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipinternalstates.h"
#include "sipclientserver.h"

#ifdef CPPUNIT_TEST
#undef EXPORT_C
#define EXPORT_C
#endif


// -----------------------------------------------------------------------------
// CSIPTransactionBase::CSIPTransactionBase
// -----------------------------------------------------------------------------
//
CSIPTransactionBase::CSIPTransactionBase(TBool aIsClientTransaction,
                                     TUint32 aRequestId,
                                     MTransactionAssociation& aAssociation) :
    iRequestId(aRequestId),
    iAssociation(&aAssociation),    
    iIsClientTransaction(aIsClientTransaction),
    iState(EConstructing),
    iAffectsDialogState(EFalse)
	{	
	}

// -----------------------------------------------------------------------------
// CSIPTransactionBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPTransactionBase::ConstructL(RStringF aType)
    {
    SIPStrings::OpenL();
    iState = ETrying;
    iType = aType.Copy();

    if (iType == SIPStrings::StringF(SipStrConsts::EInvite))
        {
        if (iIsClientTransaction)
            {
            iState = ECalling;
            }
        else
            {
            iState = EProceeding;
            }
        }

    iAssociation->AddTransactionL(*this);
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::~CSIPTransactionBase
// -----------------------------------------------------------------------------
//
CSIPTransactionBase::~CSIPTransactionBase()
	{
    delete iResponseElements;

    if (iAssociation)
        {
        iAssociation->RemoveTransaction(*this);
        }
    iType.Close();
  
    if (iState != EConstructing)
    	{
    	SIPStrings::Close();
    	}
	}

// -----------------------------------------------------------------------------
// CSIPTransactionBase::Type
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPTransactionBase::Type() const
    {
    __TEST_INVARIANT;
    return iType;
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::State
// If the current state is Completed or Confirmed, it is possible that the
// transaction in the SIP server has entered another state, as some state
// transitions happen without SIP Client API being aware of them.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPTransactionBase::TState CSIPTransactionBase::StateL()
    {
    __TEST_INVARIANT;

    if (iState == ECompleted || iState == EConfirmed)
    	{
    	CheckAssociationL();

    	RSIP& rsip = iAssociation->SIPConnectionL().SIP()->Implementation().
    		SIPClient().SIP();
    	CSIPInternalStates::TState
    		state(CSIPInternalStates::ETransactionTrying);

		TSIPIds ids;
    	ids.iRequestId = iRequestId;
    	CSIPInternalStates::GetState(rsip,
	    							 ids,
	    							 state,
	    							 ESipItcGetTransactionState);
    	//Convert state value. Only confirmed and terminated are needed.
    	if (state == CSIPInternalStates::ETransactionConfirmed)
    		{    		
    		iState = EConfirmed;
    		}
    	else
    		{
    		if (state == CSIPInternalStates::ETransactionTerminated)
    			{
    			iState = ETerminated;
    			}
    		}
    	
    	}

    return iState;
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::IsSIPClientTransaction
// Don't call __TEST_INVARIANT here.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPTransactionBase::IsSIPClientTransaction() const
    {    
    return iIsClientTransaction;
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::operator==
// Don't call __TEST_INVARIANT here.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool
CSIPTransactionBase::operator==(const CSIPTransactionBase& aTransaction) const
    {    
    if (!iRequestId || !aTransaction.RequestId())
        {
        //If either of the ids is empty, the transactions are not considered
        //same, unless their addresses are same
        return this == &aTransaction;
        }

    return iRequestId == aTransaction.RequestId();    
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::RequestId
// -----------------------------------------------------------------------------
//
TUint32 CSIPTransactionBase::RequestId() const
    {
    __TEST_INVARIANT;
    return iRequestId;    
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::Detach
// -----------------------------------------------------------------------------
//
void CSIPTransactionBase::Detach(const MTransactionAssociation& aAssociation)
    {
    __SIP_ASSERT_RETURN(iAssociation != NULL, KErrNotFound);
    __ASSERT_DEBUG(&aAssociation == iAssociation,
		User::Panic(_L("CSIPTransactionBase::Detach() aAssoc != iAssoc"),
		KErrArgument));

    if (&aAssociation == iAssociation)
        {
        iAssociation = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::CheckAssociationL
// -----------------------------------------------------------------------------
//
void CSIPTransactionBase::CheckAssociationL() const
    {
    if (!iAssociation)
        {
        User::Leave(KErrSIPResourceNotAvailable);
        }
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::ChangeState
// -----------------------------------------------------------------------------
//
void CSIPTransactionBase::ChangeState(CSIPTransactionBase::TState aNextState)
    {
    __TEST_INVARIANT;
    __ASSERT_DEBUG(iAssociation != NULL,
        User::Panic(_L("CSIPTransactionBase::ChangeState() iAssociation = 0"),
        			KErrSIPResourceNotAvailable));

    iState = aNextState;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::SetResponseElements
// -----------------------------------------------------------------------------
//
void CSIPTransactionBase::SetResponseElements(CSIPResponseElements* aElements)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_RETURN(aElements, KErrArgument);

    delete iResponseElements;
    iResponseElements = aElements;

    TUint statusCode = iResponseElements->StatusCode();
    if (statusCode < 200 || statusCode == 401 || statusCode == 407)
        {
        ChangeState(EProceeding);
        }
    else
        {
        if (iType == SIPStrings::StringF(SipStrConsts::EInvite) &&
            iResponseElements->StatusCode() < 300)
            {
            ChangeState(ETerminated);
            }
        else
            {
            ChangeState(ECompleted);
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::AffectsDialogState
// -----------------------------------------------------------------------------
//
TBool CSIPTransactionBase::AffectsDialogState() const
    {
    __TEST_INVARIANT;
    __ASSERT_DEBUG(iAssociation != NULL,
        User::Panic(_L("CSIPTaBase::AffectsDialogState() iAssociation = 0"),
        			KErrSIPResourceNotAvailable));

    return iAffectsDialogState;
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::SetAffectsDialogState
// -----------------------------------------------------------------------------
//
void CSIPTransactionBase::SetAffectsDialogState()
    {
    __TEST_INVARIANT;    
    __SIP_ASSERT_RETURN(!iAffectsDialogState, KErrNotFound);
    __ASSERT_DEBUG(iAssociation != NULL,
        User::Panic(_L("CSIPTaBase::SetAffectsDialogState() iAssociation = 0"),
        			KErrSIPResourceNotAvailable));
    
    iAffectsDialogState = ETrue;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::IsTargetRefresh
// -----------------------------------------------------------------------------
//
TBool CSIPTransactionBase::IsTargetRefresh(RStringF aType)
    {
    return (aType == SIPStrings::StringF(SipStrConsts::EInvite) ||
        	aType == SIPStrings::StringF(SipStrConsts::EUpdate) ||
        	aType == SIPStrings::StringF(SipStrConsts::ENotify) ||
        	aType == SIPStrings::StringF(SipStrConsts::ESubscribe) ||        
        	aType == SIPStrings::StringF(SipStrConsts::ERefer));
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::ResponseElements
// -----------------------------------------------------------------------------
//
const CSIPResponseElements* CSIPTransactionBase::ResponseElements() const
    {
    return iResponseElements;
    }

// -----------------------------------------------------------------------------
// CSIPTransactionBase::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CSIPTransactionBase::__DbgTestInvariant() const
	{
	if (!iRequestId)
		{
		User::Invariant();
		}
	}
