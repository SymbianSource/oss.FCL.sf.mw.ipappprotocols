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
// Name          : SIPNotifyDialogAssoc.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "sip.h"
#include "siperr.h"
#include "sipnotifydialogassoc.h"
#include "SipDialogAssocImplementation.h"
#include "SipDialogImplementation.h"
#include "SipConnectionImplementation.h"
#include "sipservertransaction.h"
#include "sipclientconnection.h"
#include "sipmessageelements.h"
#include "sipeventheader.h"
#include "sipsubscriptionstateheader.h"
#include "SipAssert.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "MessageHeaderCleanup.h"

#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPNotifyDialogAssoc*
CSIPNotifyDialogAssoc::NewL(CSIPServerTransaction& aTransaction,
			     			CSIPEventHeader* aEvent,
				 			CSIPSubscriptionStateHeader* aState)
    {
    CSIPNotifyDialogAssoc* self =
    	CSIPNotifyDialogAssoc::NewLC(aTransaction, aEvent, aState);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPNotifyDialogAssoc*
CSIPNotifyDialogAssoc::NewLC(CSIPServerTransaction& aTransaction,
			      			 CSIPEventHeader* aEvent,
				  			 CSIPSubscriptionStateHeader* aState)
    {    
    CSIPNotifyDialogAssoc* self = new (ELeave) CSIPNotifyDialogAssoc();
    CleanupStack::PushL(self);
    self->ConstructL(aTransaction,NULL,aEvent,aState);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPNotifyDialogAssoc*
CSIPNotifyDialogAssoc::NewL(CSIPServerTransaction& aTransaction,
                            const MSIPRegistrationContext& aContext,
			     			CSIPEventHeader* aEvent,
				 			CSIPSubscriptionStateHeader* aState)
    {
    CSIPNotifyDialogAssoc* self =
    	CSIPNotifyDialogAssoc::NewLC(aTransaction, aContext, aEvent, aState);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPNotifyDialogAssoc*
CSIPNotifyDialogAssoc::NewLC(CSIPServerTransaction& aTransaction,
                             const MSIPRegistrationContext& aContext,
			      			 CSIPEventHeader* aEvent,
				  			 CSIPSubscriptionStateHeader* aState)
    {    
    CSIPNotifyDialogAssoc* self = new (ELeave) CSIPNotifyDialogAssoc();
    CleanupStack::PushL(self);
    self->ConstructL(aTransaction,&aContext,aEvent,aState);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::CSIPNotifyDialogAssoc
// -----------------------------------------------------------------------------
//
CSIPNotifyDialogAssoc::CSIPNotifyDialogAssoc()
    {
    }

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::~CSIPNotifyDialogAssoc
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPNotifyDialogAssoc::~CSIPNotifyDialogAssoc()
    {
    delete iEvent;
    delete iSubscriptionState;
    }

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::Event
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPEventHeader& CSIPNotifyDialogAssoc::Event() const
	{
	return *iEvent;
	}

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::SubscriptionState
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscriptionStateHeader& CSIPNotifyDialogAssoc::SubscriptionState()
	{
	return *iSubscriptionState;
	}

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::SubscriptionState
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPSubscriptionStateHeader&
CSIPNotifyDialogAssoc::SubscriptionState() const
	{
	return *iSubscriptionState;
	}

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::SendNotifyL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPNotifyDialogAssoc::SendNotifyL(CSIPMessageElements* aElements)
    {
    return Dialog().Implementation().SendNotifyL(*this, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::DoSendNotifyWithinDialogL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPNotifyDialogAssoc::DoSendNotifyWithinDialogL(CSIPMessageElements* aElements)
    {
    CSIPMessageElements* elem = aElements;
    if (!aElements)
    	{
    	//CSIPMessageElements is created by SIP Client API, and will be deleted
    	//in all cases.
    	elem = CSIPMessageElements::NewLC();
    	}

    TMessageHeaderCleanup headerCleanup(*elem);
    TCleanupItem cleanupItem(TMessageHeaderCleanup::Cleanup, &headerCleanup);
    if (aElements)
    	{
    	//CSIPMessageElements is created by application. If leave occurs, the
    	//copied Event and SubscribtionState headers are detached from
    	//CSIPMessageElements, but CSIPMessageElements won't be deleted.
    	//If leave doesn't occur, CSIPMessageElements will be deleted.  	    	
    	CleanupStack::PushL(cleanupItem);
    	}

	CSIPDialogAssocImplementation::CopyHeaderL(*iEvent,
											   *elem,
											   headerCleanup);
	CSIPDialogAssocImplementation::CopyHeaderL(*iSubscriptionState,
											   *elem,
											   headerCleanup);
    CSIPClientTransaction* ta =
    	Dialog().Implementation().SendRequestInDialogL(
    							Implementation(),
    							SIPStrings::StringF(SipStrConsts::ENotify),
                                elem);
	CleanupStack::Pop(); //TCleanupItem or elem
	return ta;
    }

// -----------------------------------------------------------------------------
// CSIPNotifyDialogAssoc::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPNotifyDialogAssoc::ConstructL(CSIPServerTransaction& aTransaction,
                                       const MSIPRegistrationContext* aContext,
                                       CSIPEventHeader* aEvent,
                                       CSIPSubscriptionStateHeader* aState)
    {
    __ASSERT_ALWAYS(
    	aTransaction.Type() == SIPStrings::StringF(SipStrConsts::ESubscribe) ||
    	aTransaction.Type() == SIPStrings::StringF(SipStrConsts::ERefer),
        User::Leave(KErrArgument));

    CSIPTransactionBase::TState state = aTransaction.StateL();
    __ASSERT_ALWAYS(state == CSIPTransactionBase::ETrying ||
    				state == CSIPTransactionBase::EProceeding,
        			User::Leave(KErrSIPInvalidTransactionState));
    __ASSERT_ALWAYS(aEvent && aState, User::Leave(KErrArgument));

	CSIPDialogImplementation* dialogImpl = aTransaction.Dialog();
	TBool createNewDialog = (dialogImpl == NULL);
	if (createNewDialog)
		{
		//Push dialogImpl to CleanupStack until it has been associated with
		//CSIPNotifyDialogAssoc, using CSIPDialogAssocBase::ConstructL().
		//Once associated, it is deleted if CSIPNotifyDialogAssoc is deleted.
		dialogImpl = CSIPDialogImplementation::NewLC(
			aTransaction.SIPConnectionL().Implementation(),aContext);
		}	

    //Associates CSIPDialogImplementation with CSIPNotifyDialogAssoc
    CSIPDialogAssocBase::ConstructL(SIPStrings::StringF(SipStrConsts::ENotify),
                                    dialogImpl->Dialog(),aTransaction);
    if (createNewDialog)
    	{
    	CleanupStack::Pop(dialogImpl);
    	dialogImpl->InitialTransactionReceivedL(aTransaction);
    	}

    iEvent = aEvent;
    iSubscriptionState = aState;    
    }
