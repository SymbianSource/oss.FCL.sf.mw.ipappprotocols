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
// Name          : SIPSubscribeDialogAssoc.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0
//



#include <uri8.h>
#include "sip.h"
#include "siperr.h"
#include "sipsubscribedialogassoc.h"
#include "SipDialogAssocImplementation.h"
#include "SipDialogImplementation.h"
#include "SipConnectionImplementation.h"
#include "sipclientconnection.h"
#include "sipclienttransaction.h"
#include "sipregistrationcontext.h"
#include "siprefresh.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipexpiresheader.h"
#include "sipcontactheader.h"
#include "sipeventheader.h"
#include "sipmessageelements.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "MessageHeaderCleanup.h"

#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscribeDialogAssoc*
CSIPSubscribeDialogAssoc::NewL(CSIPDialog& aDialog, CSIPEventHeader* aEvent)
    {
    CSIPSubscribeDialogAssoc* self =
    	CSIPSubscribeDialogAssoc::NewLC(aDialog, aEvent);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscribeDialogAssoc*
CSIPSubscribeDialogAssoc::NewLC(CSIPDialog& aDialog, CSIPEventHeader* aEvent)
    {
    __ASSERT_ALWAYS(aEvent, User::Leave(KErrArgument));

    CSIPSubscribeDialogAssoc* self = new (ELeave) CSIPSubscribeDialogAssoc();
    CleanupStack::PushL(self);
    self->CSIPDialogAssocBase::ConstructL(
    						SIPStrings::StringF(SipStrConsts::ESubscribe),
    						aDialog);
    self->iEvent = aEvent;
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscribeDialogAssoc*
CSIPSubscribeDialogAssoc::NewL(CSIPConnection& aConnection,
						       CSIPFromHeader* aFrom,
						       CUri8* aRemoteUri,
						       CSIPEventHeader* aEvent,        
						       CSIPToHeader* aTo,        
						       CSIPContactHeader* aContact)
    {    
    CSIPSubscribeDialogAssoc* self =
        CSIPSubscribeDialogAssoc::NewLC(aConnection,
                                        aFrom,
                                        aRemoteUri,
                                        aEvent,
                                        aTo,                                        
                                        aContact);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscribeDialogAssoc*
CSIPSubscribeDialogAssoc::NewLC(CSIPConnection& aConnection,
                                CSIPFromHeader* aFrom,
                                CUri8* aRemoteUri,
						        CSIPEventHeader* aEvent,
						        CSIPToHeader* aTo,        
						        CSIPContactHeader* aContact)
    {
    __ASSERT_ALWAYS(aFrom, User::Leave(KErrArgument));

	return CSIPSubscribeDialogAssoc::NewLC(aConnection,
										   aRemoteUri,
										   aEvent,
                                		   aFrom,
                                		   aTo,
			                    		   aContact,
			                    		   NULL);
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscribeDialogAssoc*
CSIPSubscribeDialogAssoc::NewL(CSIPConnection& aConnection,
							   CUri8* aRemoteUri,                               
                               const MSIPRegistrationContext& aContext,
                               CSIPEventHeader* aEvent,
                               CSIPFromHeader* aFrom,
                               CSIPToHeader* aTo,
	                           CSIPContactHeader* aContact)
    {
    CSIPSubscribeDialogAssoc* self =
        CSIPSubscribeDialogAssoc::NewLC(aConnection,
        								aRemoteUri,
                                        aContext,
                                        aEvent,
                                        aFrom,
                                        aTo,                                
                                        aContact);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscribeDialogAssoc*
CSIPSubscribeDialogAssoc::NewLC(CSIPConnection& aConnection,
								CUri8* aRemoteUri,			                    
			                    const MSIPRegistrationContext& aContext,
			                    CSIPEventHeader* aEvent,
                                CSIPFromHeader* aFrom,
                                CSIPToHeader* aTo,
			                    CSIPContactHeader* aContact)
    {    
    __ASSERT_ALWAYS(aContext.IsContextActive(),
                    User::Leave(KErrSIPInvalidRegistrationState));

	return CSIPSubscribeDialogAssoc::NewLC(aConnection,
										   aRemoteUri,
										   aEvent,
                                		   aFrom,
                                		   aTo,
			                    		   aContact,
			                    		   &aContext);
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::NewLC
// -----------------------------------------------------------------------------
//
CSIPSubscribeDialogAssoc*
CSIPSubscribeDialogAssoc::NewLC(CSIPConnection& aConnection,
								CUri8* aRemoteUri,
								CSIPEventHeader* aEvent,
                                CSIPFromHeader* aFrom,
                                CSIPToHeader* aTo,
			                    CSIPContactHeader* aContact,
			                    const MSIPRegistrationContext* aContext)
    {
    __ASSERT_ALWAYS(aRemoteUri && aEvent, User::Leave(KErrArgument));	

	CSIPDialogImplementation* dialogImpl =
			CSIPDialogImplementation::NewLC(aConnection.Implementation(),
											aContext);										
    CSIPSubscribeDialogAssoc* self =
    	CSIPSubscribeDialogAssoc::NewL(dialogImpl->Dialog(), aEvent);
   	//Don't take ownership of aEvent yet, as leave can occur
    self->iEvent = NULL;

    CleanupStack::Pop(dialogImpl);
    CleanupStack::PushL(self);

	dialogImpl->SetHeadersL(aFrom, aTo, aRemoteUri, aContact);
    self->iEvent = aEvent;

    return self;
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::CSIPSubscribeDialogAssoc
// -----------------------------------------------------------------------------
//
CSIPSubscribeDialogAssoc::CSIPSubscribeDialogAssoc()
    {
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::~CSIPSubscribeDialogAssoc
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscribeDialogAssoc::~CSIPSubscribeDialogAssoc()
    {
    delete iRefresh;
    delete iEvent;
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::SendSubscribeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPSubscribeDialogAssoc::SendSubscribeL(CSIPMessageElements* aElements,
										 CSIPRefresh* aSIPRefresh)
    {
    return Dialog().Implementation().SendSubscribeL(*this,
    												aElements,
    												aSIPRefresh);
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::UpdateL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPSubscribeDialogAssoc::UpdateL(CSIPMessageElements* aElements)
	{
	__ASSERT_ALWAYS(aElements &&
		aElements->UserHeaderCount(
			SIPStrings::StringF(SipStrConsts::EEventHeader)) == 0,
			User::Leave(KErrArgument));
	return Dialog().Implementation().UpdateL(*this, aElements);
	}

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::SendUnsubscribeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction* 
CSIPSubscribeDialogAssoc::SendUnsubscribeL(CSIPMessageElements* aElements)
    {
    return Dialog().Implementation().SendUnsubscribeL(*this, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::Event
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPEventHeader& CSIPSubscribeDialogAssoc::Event() const
	{
	return *iEvent;
	}

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::ConnectionLost
// -----------------------------------------------------------------------------
//
void CSIPSubscribeDialogAssoc::ConnectionLost()
    {
    if (iRefresh)
        {
        iRefresh->ChangeState(CSIPRefresh::ETerminated);
        }
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::DoSendSubscribeL
// If subscription is already being refreshed, leaves with KErrAlreadyExists
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPSubscribeDialogAssoc::DoSendSubscribeL(CSIPMessageElements* aElements,
                                           CSIPRefresh* aRefresh,
                                           TBool aWithinDialog)
    {
    __ASSERT_ALWAYS(!(aRefresh && iRefresh), User::Leave(KErrAlreadyExists));
    
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
    CSIPDialogAssocImplementation::CopyHeaderL(*iEvent, *elem, headerCleanup);
	CSIPClientTransaction* ta = NULL;
	if (aWithinDialog)
		{
		ta = CSIPClientTransaction::NewLC(Type(),										  
										  Implementation(),
										  aRefresh ? aRefresh : iRefresh);
		TUint32 requestId(0);
		TUint32 refreshId(0);
	    if (iRefresh)
	        {
	        //Updates an existing refreshed subscription
	        Implementation().ClientConnectionL().SendRequestWithinDialogL(
        								Dialog().Implementation().DialogId(),
		                           		iRefresh->RefreshId(),
                                        requestId,
                                        elem);
	        }
	    else
	        {
	        Implementation().ClientConnectionL().SendRequestWithinDialogL(
	        				 Dialog().Implementation().DialogId(),
				           	 requestId,
				           	 refreshId,
						 	 Type(),
				             elem,
				             CSIPTransactionBase::IsTargetRefresh(Type()),
				             (aRefresh != NULL));
		    }
		if (aRefresh)
		    {
		    aRefresh->SetRefreshIdIfEmpty(refreshId);
		    }
		CleanupStack::Pop(ta);
		ta->SetRequestId(requestId);
	    //Take ownership of aElements when leave can't occur
	    delete elem;
		}
	else
		{
	    const CSIPContactHeader* contact =
	    	Dialog().Implementation().ContactHeader();
		if (contact)
			{
			CSIPDialogAssocImplementation::CopyHeaderL(*contact,
													   *elem,
													   headerCleanup);
			}
		ta = Dialog().Implementation().SendDialogCreatingRequestL(
										Implementation(), elem, aRefresh);	
		}

	CleanupStack::Pop(); //TCleanupItem or elem
	if (aRefresh)
        {
        aRefresh->SetRequestType(Type());                
    	iRefresh = aRefresh;
    	iRefresh->SetRefreshOwner(Implementation());            
        }
    return ta;
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::DoSendUnsubscribeL
// Sending (un)SUBSCRIBE does not terminate the dialog. (un)SUBSCRIBE is not a
// dialog creating request.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPSubscribeDialogAssoc::DoSendUnsubscribeL(CSIPMessageElements* aElements)
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
    CSIPDialogAssocImplementation::CopyHeaderL(*iEvent, *elem, headerCleanup);
    RPointerArray<CSIPHeaderBase> expireHeaders = elem->UserHeadersL(
    		SIPStrings::StringF(SipStrConsts::EExpiresHeader));
    CleanupClosePushL(expireHeaders);

	TBool hasExpiresZero = EFalse;
    TInt expiresHeaderCount = expireHeaders.Count();
    __ASSERT_ALWAYS(expiresHeaderCount <= 1, User::Leave(KErrArgument));
  
	if (expiresHeaderCount == 1)
        {
        CSIPExpiresHeader* expires =
        	static_cast<CSIPExpiresHeader*>(expireHeaders[0]);
        __ASSERT_ALWAYS(expires->Value() == 0, User::Leave(KErrArgument));
        hasExpiresZero = ETrue;
        }
    CleanupStack::PopAndDestroy(); //expireHeaders

    if (!hasExpiresZero)
    	{
        CSIPExpiresHeader* expires = new (ELeave) CSIPExpiresHeader(0);
        CleanupStack::PushL(expires);
        CSIPDialogAssocImplementation::CopyHeaderL(*expires,
        										   *elem,
        										   headerCleanup);
        CleanupStack::PopAndDestroy(expires);
    	}

	CSIPClientTransaction* ta =
    	CSIPClientTransaction::NewLC(Type(), Implementation(), iRefresh);
    TUint32 requestId(0);
	if (iRefresh)
	    {
	    //Terminates an existing refreshed subscription
	    Implementation().ClientConnectionL().SendRequestWithinDialogL(
	                            Dialog().Implementation().DialogId(),
	                            iRefresh->RefreshId(),
	                            requestId,
	                            elem);
        }
	else
	    {
	    TUint32 refreshId(0);
        Implementation().ClientConnectionL().SendRequestWithinDialogL(
                                Dialog().Implementation().DialogId(),
                                requestId,
                                refreshId,
                                Type(),
                                elem,
                                CSIPTransactionBase::IsTargetRefresh(Type()));
        }
    CleanupStack::Pop(ta);
	ta->SetRequestId(requestId);

    //Take ownership of aElements when leave can't occur
    delete elem;
    CleanupStack::Pop(); //TCleanupItem or elem

    return ta;
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::SIPRefresh
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPRefresh* CSIPSubscribeDialogAssoc::SIPRefresh() const
    {
    return iRefresh;
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::FindRefresh
// -----------------------------------------------------------------------------
//
CSIPRefresh* CSIPSubscribeDialogAssoc::FindRefresh(TUint32 aRefreshId)
    {
    if (iRefresh && iRefresh->DoesMatch(aRefreshId))
        {
        return iRefresh;
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPSubscribeDialogAssoc::DeletingRefresh
// -----------------------------------------------------------------------------
//
void CSIPSubscribeDialogAssoc::DeletingRefresh(CSIPRefresh& /*aRefresh*/,
										  	   TUint32 aRefreshId)
	{
	CSIPClientConnection* clientConn = Implementation().ClientConnection();
	if (clientConn)
		{
		clientConn->TerminateDialogRefresh(Dialog().Implementation().DialogId(),
          								   aRefreshId);
		}
    iRefresh = NULL;
	}
