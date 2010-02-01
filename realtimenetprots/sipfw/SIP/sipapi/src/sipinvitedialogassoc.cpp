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
// Name          : SIPInviteDialogAssoc.cpp
// Part of       : SIPAPI
// Version       : SIP/5.0 
//



#include <uri8.h>
#include "sip.h"
#include "siperr.h"
#include "sipinvitedialogassoc.h"
#include "SipDialogImplementation.h"
#include "SipDialogAssocImplementation.h"
#include "SipConnectionImplementation.h"
#include "sipinviteclienttransaction.h"
#include "sipservertransaction.h"
#include "sipclientconnection.h"
#include "sipregistrationcontext.h"
#include "sipmessageelements.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipcontactheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "MessageHeaderCleanup.h"

#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc*
CSIPInviteDialogAssoc::NewL(CSIPDialog& aDialog)
    {
    CSIPInviteDialogAssoc* self = CSIPInviteDialogAssoc::NewLC(aDialog);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc*
CSIPInviteDialogAssoc::NewLC(CSIPDialog& aDialog)
    {
    CSIPInviteDialogAssoc* self = new (ELeave) CSIPInviteDialogAssoc();
    CleanupStack::PushL(self);
    self->CSIPDialogAssocBase::ConstructL(
    	SIPStrings::StringF(SipStrConsts::EInvite), aDialog);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc*
CSIPInviteDialogAssoc::NewL(CSIPServerTransaction& aTransaction)
    {    
    CSIPInviteDialogAssoc* self = CSIPInviteDialogAssoc::NewLC(aTransaction);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc*
CSIPInviteDialogAssoc::NewLC(CSIPServerTransaction& aTransaction)
    {
    CSIPInviteDialogAssoc* self = new (ELeave) CSIPInviteDialogAssoc();
    CleanupStack::PushL(self);
    self->ConstructL(aTransaction,NULL);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc*
CSIPInviteDialogAssoc::NewL(CSIPServerTransaction& aTransaction,
                            const MSIPRegistrationContext& aContext)
    {    
    CSIPInviteDialogAssoc* self = 
        CSIPInviteDialogAssoc::NewLC(aTransaction,aContext);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc*
CSIPInviteDialogAssoc::NewLC(CSIPServerTransaction& aTransaction,
                             const MSIPRegistrationContext& aContext)
    {    
    CSIPInviteDialogAssoc* self = new (ELeave) CSIPInviteDialogAssoc();
    CleanupStack::PushL(self);
    self->ConstructL(aTransaction,&aContext);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc*
CSIPInviteDialogAssoc::NewL(CSIPConnection& aConnection,
                            CSIPFromHeader* aFrom,
                            CUri8* aRemoteUri,
                            CSIPToHeader* aTo,                            
                            CSIPContactHeader* aContact)                            
    {
    CSIPInviteDialogAssoc* self = CSIPInviteDialogAssoc::NewLC(aConnection,
                                                               aFrom,
                                                               aRemoteUri,
                                                               aTo,                                                               
                                                               aContact);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc*
CSIPInviteDialogAssoc::NewLC(CSIPConnection& aConnection,
                             CSIPFromHeader* aFrom,
                             CUri8* aRemoteUri,
                             CSIPToHeader* aTo,                             
                             CSIPContactHeader* aContact)
    {
    __ASSERT_ALWAYS(aFrom, User::Leave(KErrArgument));
	return CSIPInviteDialogAssoc::NewLC(aConnection,
										aRemoteUri,
										aFrom,
										aTo,
										aContact,
										NULL);
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc*
CSIPInviteDialogAssoc::NewL(CSIPConnection& aConnection,                                            
                            CUri8* aRemoteUri,
                            const MSIPRegistrationContext& aContext,
                            CSIPFromHeader* aFrom,
                            CSIPToHeader* aTo,	                        
	                        CSIPContactHeader* aContact)
    {
    CSIPInviteDialogAssoc* self = CSIPInviteDialogAssoc::NewLC(aConnection,
    														   aRemoteUri,                                                               
                                                               aContext,
                                                               aFrom,
                                                               aTo,
                                                               aContact);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc*
CSIPInviteDialogAssoc::NewLC(CSIPConnection& aConnection,
							 CUri8* aRemoteUri,
                             const MSIPRegistrationContext& aContext,
                             CSIPFromHeader* aFrom,
                             CSIPToHeader* aTo,	                        
	                         CSIPContactHeader* aContact)
    {    
    __ASSERT_ALWAYS(aContext.IsContextActive(),
                    User::Leave(KErrSIPInvalidRegistrationState));
	return CSIPInviteDialogAssoc::NewLC(aConnection,
										aRemoteUri,
										aFrom,
										aTo,
										aContact,
										&aContext);
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
CSIPInviteDialogAssoc* CSIPInviteDialogAssoc::NewLC(CSIPConnection& aConnection,
								 CUri8* aRemoteUri,
	                             CSIPFromHeader* aFrom,
	                             CSIPToHeader* aTo,
		                         CSIPContactHeader* aContact,
		                         const MSIPRegistrationContext* aContext)
	{
	__ASSERT_ALWAYS(aRemoteUri, User::Leave(KErrArgument));

	CSIPDialogImplementation* dialogImpl =
			CSIPDialogImplementation::NewLC(aConnection.Implementation(),
											aContext);
	//Associates dialogImpl with CSIPInviteDialogAssoc
    CSIPInviteDialogAssoc* self =
    	CSIPInviteDialogAssoc::NewL(dialogImpl->Dialog());
	CleanupStack::Pop(dialogImpl);
    CleanupStack::PushL(self);

    //Ownership of headers is passed when leave can't occur
    dialogImpl->SetHeadersL(aFrom, aTo, aRemoteUri, aContact);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::CSIPInviteDialogAssoc
// -----------------------------------------------------------------------------
//
CSIPInviteDialogAssoc::CSIPInviteDialogAssoc()
    {
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::~CSIPInviteDialogAssoc
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInviteDialogAssoc::~CSIPInviteDialogAssoc()
    {    
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::SendInviteL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPInviteDialogAssoc::SendInviteL(CSIPMessageElements* aElements)
    {    
    return Dialog().Implementation().SendInviteL(*this, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::SendPrackL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPInviteDialogAssoc::SendPrackL(CSIPMessageElements* aElements)
    {    
    return Dialog().Implementation().SendPrackL(*this, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::SendUpdateL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPInviteDialogAssoc::SendUpdateL(CSIPMessageElements* aElements)
    {
    return Dialog().Implementation().SendUpdateL(*this, aElements);    
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::SendAckL
// aTransaction must be INVITE transaction
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPInviteDialogAssoc::SendAckL(
                           const CSIPClientTransaction& aTransaction,
                           CSIPMessageElements* aElements)
    {
    __ASSERT_ALWAYS(aTransaction.Type() == Type(), User::Leave(KErrArgument));

    Dialog().Implementation().SendAckL(*this, aTransaction, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::SendByeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPInviteDialogAssoc::SendByeL(CSIPMessageElements* aElements)
    {
    return Dialog().Implementation().SendByeL(*this, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::DoSendInviteL
// CSIPClientConnection checks connection state is active.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPInviteDialogAssoc::DoSendInviteL(CSIPMessageElements* aElements)
    {
	CSIPMessageElements* elem = aElements;
    if (!aElements)
    	{
    	elem = CSIPMessageElements::NewLC();
    	}

    TMessageHeaderCleanup headerCleanup(*elem);
    TCleanupItem cleanupItem(TMessageHeaderCleanup::Cleanup, &headerCleanup);
    if (aElements)
    	{
    	CleanupStack::PushL(cleanupItem);
    	}
   	const CSIPContactHeader* contact =
   		Dialog().Implementation().ContactHeader();
	if (contact)
		{
		CSIPDialogAssocImplementation::CopyHeaderL(*contact,
												   *elem,
												   headerCleanup);
		}

	CSIPClientTransaction* ta =
		Dialog().Implementation().SendDialogCreatingRequestL(Implementation(),
															 elem);
    CleanupStack::Pop(); //TCleanupItem or elem

    return ta;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::DoSendAckL
// Ownership of aElements is taken when leave can't occur.
// -----------------------------------------------------------------------------
//
void
CSIPInviteDialogAssoc::DoSendAckL(const CSIPClientTransaction& aTransaction,
                                  CSIPMessageElements* aElements)
    {
    Implementation().ClientConnectionL().SendAckL(
    							Dialog().Implementation().DialogId(),
                                aTransaction.RequestId(),
                                aElements);
    delete aElements;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::DoSendRequestWithinDialogL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPInviteDialogAssoc::DoSendRequestWithinDialogL(RStringF aMethod,
											CSIPMessageElements* aElements)
    {
    return Dialog().Implementation().SendRequestInDialogL(Implementation(),
					                                      aMethod,
					                                      aElements);
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::DoSendCancelL
// CANCEL is a request outside dialog, so associate the transaction with
// CSIPConnection.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CSIPInviteDialogAssoc::DoSendCancelL(TUint32 aRequestId)
    {
    CSIPClientTransaction* ta = CSIPClientTransaction::NewLC(
						SIPStrings::StringF(SipStrConsts::ECancel),                        
                        Implementation().SIPConnectionL().Implementation());

    TUint32 cancelRequestId(0);
    Implementation().ClientConnectionL().SendCancelL(aRequestId,
    												 cancelRequestId);
	CleanupStack::Pop(ta);
	ta->SetRequestId(cancelRequestId);
    return ta;
    }

// -----------------------------------------------------------------------------
// CSIPInviteDialogAssoc::ConstructL
// -----------------------------------------------------------------------------
//    
void CSIPInviteDialogAssoc::ConstructL(CSIPServerTransaction& aTransaction,
                                       const MSIPRegistrationContext* aContext)
    {
    __ASSERT_ALWAYS(
    	aTransaction.Type() == SIPStrings::StringF(SipStrConsts::EInvite),
    	User::Leave(KErrArgument));
    __ASSERT_ALWAYS(aTransaction.StateL() == CSIPTransactionBase::EProceeding,
        User::Leave(KErrSIPInvalidTransactionState));

	CSIPDialogImplementation* dialogImpl = aTransaction.Dialog();
	TBool createNewDialog = (dialogImpl == NULL);
	if (createNewDialog)
		{
		//Push dialogImpl to CleanupStack until it is associated with
		//CSIPInviteDialogAssoc. Once associated, it is deleted if
		//CSIPInviteDialogAssoc is deleted.
		dialogImpl = CSIPDialogImplementation::NewLC(
			aTransaction.SIPConnectionL().Implementation(),aContext);
		}

    //Associates dialogImpl with CSIPInviteDialogAssoc
    CSIPDialogAssocBase::ConstructL(SIPStrings::StringF(SipStrConsts::EInvite),
						    	    dialogImpl->Dialog(),aTransaction);
	if (createNewDialog)
    	{
    	CleanupStack::Pop(dialogImpl);
    	dialogImpl->InitialTransactionReceivedL(aTransaction);
    	}    
    }
