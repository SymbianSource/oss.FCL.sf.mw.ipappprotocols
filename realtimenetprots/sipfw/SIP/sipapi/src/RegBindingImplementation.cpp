// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : RegBindingImplementation.cpp
// Part of       : SIPAPI
// Version       : SIP/5.0 
//



#include <uri8.h>
#include "siperr.h"
#include "sipdefs.h"
#include "SipAssert.h"
#include "sipclientconnection.h"
#include "SipConnectionImplementation.h"
#include "RegBindingImplementation.h"
#include "sipregistrationstate.h"
#include "sipclienttransaction.h"
#include "sipmessageelements.h"
#include "sipresponseelements.h"
#include "siprefresh.h"
#include "siprouteheader.h"
#include "sipcontactheader.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipconnectioncallback.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include <sipregistrationbinding.h>

#ifdef CPPUNIT_TEST
#include "TestCleanupStack.h"
#endif


// -----------------------------------------------------------------------------
// CRegBindingImplementation::NewL
// -----------------------------------------------------------------------------
//
CRegBindingImplementation*
CRegBindingImplementation::NewL(CSIPRegistrationBinding& aRegistration,
								CSIPConnection& aConnection,
					   		    CSIPToHeader* aAor,
					   		    CSIPContactHeader* aContact,
					   		    CSIPRefresh* aRefresh,
					   		    CSIPRouteHeader* aOutboundProxy,
					   		    CUri8* aRemoteUri,
                       		    CSIPFromHeader* aFrom)
    {
    __ASSERT_ALWAYS(aAor && aContact, User::Leave(KErrArgument));

    TInt expires = aContact->ExpiresParameter();
    __ASSERT_ALWAYS(expires >= static_cast<TInt>(KSIPMinExpirationValue) ||
    				expires == KErrNotFound, User::Leave(KErrArgument));
    CRegBindingImplementation* self =
    	new (ELeave) CRegBindingImplementation(aRegistration, aConnection);
    CleanupStack::PushL(self);
    self->ConstructL(aAor,
                     aContact,
                     aRefresh,
                     aOutboundProxy,
                     aRemoteUri,
                     aFrom);	
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::CRegBindingImplementation
// -----------------------------------------------------------------------------
//
CRegBindingImplementation::CRegBindingImplementation(
									CSIPRegistrationBinding& aRegistration,
									CSIPConnection& aConnection) :
	iRegistration(aRegistration),
    iConnection(&aConnection)
    {
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::ConstructL
// aAor and aContact have been checked in NewLC so they aren't checked again
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::ConstructL(CSIPToHeader* aAor,
								  		   CSIPContactHeader* aContact,                                  
							  	  		   CSIPRefresh* aRefresh,
								  		   CSIPRouteHeader* aOutboundProxy,
								  		   CUri8* aRemoteUri,
                                  		   CSIPFromHeader* aFrom)
    {
    CheckConnectionL();

    iConnection->Implementation().AddRegistrationL(*this);
    iState = iConnection->Implementation().InitialRegisterStateL();
    iContact = aContact;
    iAor = aAor;
    iRefresh = aRefresh;
    if (iRefresh)
        {
        iRefresh->SetRefreshOwner(*this);
        iRefresh->SetRequestType(SIPStrings::StringF(SipStrConsts::ERegister));
        }

    iOutboundProxy = aOutboundProxy;
    iRemoteUri = aRemoteUri;
    iFrom = aFrom;
    iSendWithExpires = ETrue;
    iCacheOutboundProxyIP = EFalse;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::~CRegBindingImplementation
// -----------------------------------------------------------------------------
//
CRegBindingImplementation::~CRegBindingImplementation()
    {
    if (iConnection)
        {
        CSIPClientConnection* clientConn =
        	iConnection->Implementation().ClientConnection();
        if (clientConn)
        	{
        	clientConn->TerminateRegistration(iRegistrationId);
        	}
        iConnection->Implementation().RemoveRegistration(*this);
        }

    for (TInt i = 0; i < iTransactions.Count(); i++)
        {
        iTransactions[i]->Detach(*this);
        }
    iTransactions.Reset();

    delete iRefresh;
    delete iOutboundProxy;
    delete iContact;
    delete iAor;
    delete iRemoteUri;
    delete iFrom;
    delete iRegisteredContact;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::IsContextActive
// -----------------------------------------------------------------------------
//
TBool CRegBindingImplementation::IsContextActive() const
    {
    __TEST_INVARIANT;
    return iState && iState->IsContextActive();
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::ContextId
// -----------------------------------------------------------------------------
//
TUint32 CRegBindingImplementation::ContextId() const
    {
    __TEST_INVARIANT;
    return iRegistrationId;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::RegisterL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegBindingImplementation::RegisterL(CSIPMessageElements* aElements)
    {
    __TEST_INVARIANT;

    CheckElementsL(aElements);
    return StateL().RegisterL(*this, aElements);
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::DoRegisterL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegBindingImplementation::DoRegisterL(CSIPMessageElements* aElements)
    {
    __TEST_INVARIANT;
    CheckConnectionL();

	CSIPClientTransaction* ta = CreateClientTransactionL();
	CleanupStack::PushL(ta);

    CSIPMessageElements* elem = aElements;
    if (!aElements)
        {
        elem = CSIPMessageElements::NewLC();
        }

    //Put contact temporarily to elem. Once REGISTER is sent, clear it from elem
    CleanupStack::PushL(TCleanupItem(DetachContactHeader, elem));
    elem->AddHeaderL(iContact);

    CSIPFromHeader* from = iFrom;
    if (!iFrom)
        {
        from = CSIPFromHeader::NewLC(*iAor);
        }

    TUint32 requestId(0);
    iConnection->Implementation().ClientConnectionL().SendRegisterL(requestId,
                                                   *iAor,
                                                   *from,
                                                   *elem,
                                                   iRemoteUri,
                                                   iOutboundProxy,
                                                   (iRefresh != NULL),
                                                   iSendWithExpires,
                                                   iCacheOutboundProxyIP);
    if (!iFrom)
        {
        CleanupStack::PopAndDestroy(from);
        }
    CleanupStack::PopAndDestroy(); //TCleanupItem

    if (aElements)
        {
        delete aElements;
        }
    else
        {
        CleanupStack::PopAndDestroy(elem);
        }
    CleanupStack::Pop(ta);
	ta->SetRequestId(requestId);

    __TEST_INVARIANT;
    return ta;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::DeregisterL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegBindingImplementation::DeregisterL(CSIPMessageElements* aElements)
    {
    __TEST_INVARIANT;    
    return StateL().DeregisterL(*this, aElements);
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::DoDeregisterL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegBindingImplementation::DoDeregisterL(CSIPMessageElements* aElements)
    {
    __TEST_INVARIANT;
    CheckConnectionL();

    CSIPClientTransaction* ta = CSIPClientTransaction::NewLC(
    	SIPStrings::StringF(SipStrConsts::ERegister), *this);

    TUint32 requestId(0);
	iConnection->Implementation().ClientConnectionL().UnregisterL(
												iRegistrationId,
					                            requestId,
                                                aElements);
    CleanupStack::Pop(ta);
	ta->SetRequestId(requestId);    
    delete aElements;

    __TEST_INVARIANT;
    return ta;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::UpdateL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegBindingImplementation::UpdateL(CSIPMessageElements* aElements,
								   TUint* aExpirationValue)
    {
    __TEST_INVARIANT;
    return StateL().UpdateL(*this, aElements, aExpirationValue);
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::DoUpdateL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegBindingImplementation::DoUpdateL(CSIPMessageElements* aElements,
                            	     TUint* aExpirationValue)
    {
    __TEST_INVARIANT;    
    CheckConnectionL();

    CSIPClientTransaction* ta = CSIPClientTransaction::NewLC(
    					SIPStrings::StringF(SipStrConsts::ERegister),		                
		                *this);

	CSIPMessageElements* elem = aElements;
    if (!aElements)
        {
        elem = CSIPMessageElements::NewLC();
        }

    if (aExpirationValue)
        {
        iContact->SetExpiresParameterL(*aExpirationValue);
        }

    //Put contact temporarily to elem. Once REGISTER is sent, clear it from elem
	CleanupStack::PushL(TCleanupItem(DetachContactHeader, elem));
    elem->AddHeaderL(iContact);

    TUint32 requestId(0);
    iConnection->Implementation().ClientConnectionL().UpdateRegistrationL(
    													iRegistrationId,
                                                        requestId,
                                                        elem);
	CleanupStack::PopAndDestroy(); //TCleanupItem
    if (aElements)
        {
        delete aElements;
        }
    else
        {
        CleanupStack::PopAndDestroy(elem);
        }

    CleanupStack::Pop(ta);
	ta->SetRequestId(requestId);

    __TEST_INVARIANT;
    return ta;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::SetOutboundProxyL
// -----------------------------------------------------------------------------
//
void
CRegBindingImplementation::SetOutboundProxyL(CSIPRouteHeader* aOutboundProxy)
    {
    __TEST_INVARIANT;

	if (IsContextActive())
		{
    	CheckConnectionL();

		CSIPClientConnection& conn =
			iConnection->Implementation().ClientConnectionL();
	    if (aOutboundProxy)
    	    {
        	conn.SetOutboundProxyL(iRegistrationId,	*aOutboundProxy);
			}
		else
			{
        	conn.RemoveOutboundProxy(iRegistrationId);
			}
        }

    //Remove old proxy when leave can not occur
    delete iOutboundProxy;
    iOutboundProxy = aOutboundProxy;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::Aor
// -----------------------------------------------------------------------------
//
const CSIPToHeader& CRegBindingImplementation::Aor() const
    {
    __TEST_INVARIANT;
    return *iAor;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::ContactHeader
// -----------------------------------------------------------------------------
//
const CSIPContactHeader& CRegBindingImplementation::ContactHeader() const
    {
    __TEST_INVARIANT;
    return *iContact;
    }
    
// -----------------------------------------------------------------------------
// CRegBindingImplementation::RegisteredContact
// -----------------------------------------------------------------------------
//
const CSIPContactHeader* CRegBindingImplementation::RegisteredContact() const
    {
    __TEST_INVARIANT;
    return iRegisteredContact;
    }    

// -----------------------------------------------------------------------------
// CRegBindingImplementation::ContactHeader
// -----------------------------------------------------------------------------
//
CSIPContactHeader& CRegBindingImplementation::ContactHeader()
    {
    __TEST_INVARIANT;
    return *iContact;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::OutboundProxy
// -----------------------------------------------------------------------------
//
const CSIPRouteHeader* CRegBindingImplementation::OutboundProxy() const
    {
    __TEST_INVARIANT;
    return iOutboundProxy;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::SIPRefresh
// -----------------------------------------------------------------------------
//
const CSIPRefresh* CRegBindingImplementation::SIPRefresh() const
    {
    __TEST_INVARIANT;
    return iRefresh;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::SIPRefresh
// -----------------------------------------------------------------------------
//
CSIPRefresh* CRegBindingImplementation::SIPRefresh()
    {
    __TEST_INVARIANT;
    return iRefresh;
    }
    
// -----------------------------------------------------------------------------
// CRegBindingImplementation::Connection
// -----------------------------------------------------------------------------
//
CSIPConnection* CRegBindingImplementation::Connection()
    {
    __TEST_INVARIANT;
    return iConnection;    
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::Connection
// -----------------------------------------------------------------------------
//
const CSIPConnection* CRegBindingImplementation::Connection() const
    {
    __TEST_INVARIANT;
    return iConnection;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool
CRegBindingImplementation::IncomingResponseL(CSIPResponseElements* aElements,
				                             TUint32 aRequestId,
							                 TUint32 aRegistrationId,
							                 TUint32 aRefreshId,
		                                     CConnectionCallback& aCallback)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aElements != NULL, KErrArgument);

    if (iState)
        {
        return iState->IncomingResponseL(*this,
                                         aElements,
		                                 aRequestId,
					                     aRegistrationId,
					                     aRefreshId,
                                         aCallback);
        }
    delete aElements;    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CRegBindingImplementation::ErrorOccured(TInt aError,
                                     		  TUint32 aRequestId,
                                     		  CConnectionCallback& aCallback)
    {
    __TEST_INVARIANT;
	return iState && iState->ErrorOccured(*this, aError, aRequestId, aCallback);
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::SetRegistrationId
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::SetRegistrationId(TUint32 aRegistrationId)
    {
    __TEST_INVARIANT;

    iRegistrationId = aRegistrationId;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::UpdateRefreshState
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::UpdateRefreshState(TUint aStatusCode) const
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_RETURN(iRefresh, KErrNotFound);

    iRefresh->UpdateRefreshState(aStatusCode);
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::FindTransaction
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegBindingImplementation::FindTransaction(TUint32 aRequestId)
    {
    __TEST_INVARIANT;

    for (TInt i = 0; i < iTransactions.Count(); i++)
        {
        if (iTransactions[i]->RequestId() == aRequestId)
            {
            return iTransactions[i];
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::ConnectionDeleted
// As CSIPClientConnection is deleted, SIP client stops all registrations. So no
// need to use TerminateRegistration. Clear iState as this object won't know
// when the CSIP owning the state handlers is deleted.
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::ConnectionDeleted()
    {
    __TEST_INVARIANT;

    iConnection = NULL;
    iState = NULL;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::CheckConnectionL
// Don't use invariant here as CheckConnectionL is used from ConstructL.
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::CheckConnectionL() const
    {
    __ASSERT_ALWAYS(iConnection, User::Leave(KErrSIPResourceNotAvailable));    
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::StateL
// -----------------------------------------------------------------------------
//
const CRegistrationState& CRegBindingImplementation::StateL() const
    {
    __TEST_INVARIANT;
	__ASSERT_ALWAYS(iState != NULL, User::Leave(KErrSIPResourceNotAvailable));

    return *iState;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::ClientConnectionL
// -----------------------------------------------------------------------------
//
CSIPClientConnection& CRegBindingImplementation::ClientConnectionL()
    {
    __TEST_INVARIANT;

    CSIPClientConnection* clientConn = ClientConnection();
    __ASSERT_ALWAYS(clientConn, User::Leave(KErrSIPResourceNotAvailable));

    return *clientConn;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::ClientConnection
// -----------------------------------------------------------------------------
//
CSIPClientConnection* CRegBindingImplementation::ClientConnection()
	{
	__TEST_INVARIANT;

	if (iConnection)
		{
		return iConnection->Implementation().ClientConnection();
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CRegBindingImplementation::SIPConnectionL
// -----------------------------------------------------------------------------
//
CSIPConnection& CRegBindingImplementation::SIPConnectionL()
    {
    __TEST_INVARIANT;
    CheckConnectionL();    

    return *iConnection;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::ChangeState
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::ChangeState(const CRegistrationState* aNewState)
    {
    __TEST_INVARIANT;

	iState = aNewState;	

    if (iRefresh)
        {
        if (iState)
            {
            iRefresh->ChangeState(iState->RefreshState());
            }
        else
            {
            iRefresh->ChangeState(CSIPRefresh::ETerminated);
            }
        }

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CRegBindingImplementation::ConnectionLost
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::ConnectionLost()
    {
    __TEST_INVARIANT;

    if (iState)
        {
        iState->ConnectionLost(*this);
        }
    
    for (TInt i = 0; i < iTransactions.Count(); i++)
        {
        iTransactions[i]->ChangeState(CSIPTransactionBase::ETerminated);
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::operator==
// -----------------------------------------------------------------------------
//
TBool CRegBindingImplementation::operator==(
	const CRegBindingImplementation& aRegistration) const
    {
    __TEST_INVARIANT;

    return ( ContextId() && aRegistration.ContextId() &&
        	 (ContextId() == aRegistration.ContextId()) ) ||
    	   this == &aRegistration;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::Binding
// -----------------------------------------------------------------------------
//
CSIPRegistrationBinding& CRegBindingImplementation::Binding() const
	{
	__TEST_INVARIANT;
	return iRegistration;
	}

// -----------------------------------------------------------------------------
// CRegBindingImplementation::UpdateRegisteredContactL
// -----------------------------------------------------------------------------
//	
void CRegBindingImplementation::UpdateRegisteredContactL()
    {
    CSIPContactHeader* contact = ClientConnectionL().ContactL(iRegistrationId);
    delete iRegisteredContact;
    iRegisteredContact = contact;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::RemoveRegisteredContact
// -----------------------------------------------------------------------------
//	
void CRegBindingImplementation::RemoveRegisteredContact()
    {
    delete iRegisteredContact;
    iRegisteredContact = NULL;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::UpdateRefreshL
// Application may not update or terminate a registration refresh.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegBindingImplementation::UpdateRefreshL(CSIPRefresh& /*aRefresh*/,
                       		              CSIPMessageElements* /*aElements*/,
                       		              TBool /*aTerminate*/)
    {
    User::Leave(KErrSIPInvalidRegistrationState);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::DeletingRefresh
// No need to use CSIPClientConnection::TerminateRefresh, as destructor calls
// TerminateRegistration freeing possible refresh from the SIP stack.
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::DeletingRefresh(CSIPRefresh& /*aRefresh*/,
                                       		    TUint32 /*aRefreshId*/)
    {
    __TEST_INVARIANT;

    iRefresh = NULL;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::TransactionAssociation
// -----------------------------------------------------------------------------
//
MTransactionAssociation& CRegBindingImplementation::TransactionAssociation()
	{
	__TEST_INVARIANT;
	return *this;
	}

// -----------------------------------------------------------------------------
// CRegBindingImplementation::CheckIfStandAlone
// -----------------------------------------------------------------------------
//
TInt CRegBindingImplementation::CheckIfStandAlone()
	{
	__TEST_INVARIANT;
	return KErrSIPInvalidRegistrationState;
	}

// -----------------------------------------------------------------------------
// CRegBindingImplementation::AddTransactionL
// -----------------------------------------------------------------------------
//
void
CRegBindingImplementation::AddTransactionL(CSIPTransactionBase& aTransaction)
    {
    __TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aTransaction.IsSIPClientTransaction(), KErrArgument);

	/*lint -e826 */
	iTransactions.AppendL(static_cast<CSIPClientTransaction*>(&aTransaction));    

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::RemoveTransaction
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::RemoveTransaction(
									const CSIPTransactionBase& aTransaction)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_RETURN(aTransaction.IsSIPClientTransaction(), KErrArgument);

	/*lint -e826 */
    TInt pos = iTransactions.Find(
    	static_cast<const CSIPClientTransaction*>(&aTransaction));
    if (pos != KErrNotFound)
        {
        iTransactions.Remove(pos);
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::CreateClientTransactionL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CRegBindingImplementation::CreateClientTransactionL()
    {
    __TEST_INVARIANT;
    return CSIPClientTransaction::NewL(
    						SIPStrings::StringF(SipStrConsts::ERegister),
                            *this,
                            iRefresh);
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::HandleError
// -----------------------------------------------------------------------------
//
TBool
CRegBindingImplementation::HandleError(CConnectionCallback& aCallback,
                                       TInt aError,
                                       TUint32 aRequestId,
                                       const CRegistrationState& aUnregistered)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_RETURN_VALUE(aError != KErrNone, EFalse);

    RemoveRegisteredContact();
	ChangeState(&aUnregistered);

	CConnectionCallback::TCallbackMethod callback =
		CConnectionCallback::EErrorOccuredInRegistration;	
	CSIPClientTransaction* ta = FindTransaction(aRequestId);
    if (ta)
        {
        ta->ChangeState(CSIPTransactionBase::ETerminated);
        callback =
        	CConnectionCallback::EErrorOccuredInRegistrationWithTransaction;
        }

	aCallback.Set(callback, ta, &Binding(), NULL, NULL, aError);

	__TEST_INVARIANT;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::CheckElementsL
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::CheckElementsL(
								const CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;

    if (aElements)
        {
        RStringF contact = SIPStrings::StringF(SipStrConsts::EContactHeader);
        RStringF expires = SIPStrings::StringF(SipStrConsts::EExpiresHeader);

        const RPointerArray<CSIPHeaderBase>& headers = aElements->UserHeaders();
        for (TInt i = 0; i < headers.Count(); i++)
            {
            if (headers[i]->Name() == contact || headers[i]->Name() == expires)
                {
                User::Leave(KErrSIPMalformedMessage);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CRegBindingImplementation::DetachContactHeader
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::DetachContactHeader(TAny* aElements)
	{
    __SIP_ASSERT_RETURN(aElements, KErrArgument);
    DetachHeaders(aElements, SIPStrings::StringF(SipStrConsts::EContactHeader));
	}

// -----------------------------------------------------------------------------
// CRegBindingImplementation::DetachHeaders
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::DetachHeaders(TAny* aElements, RStringF aName)
	{
    __SIP_ASSERT_RETURN(aElements, KErrArgument);

    CSIPMessageElements* elem =
    	reinterpret_cast<CSIPMessageElements*>(aElements);
    const RPointerArray<CSIPHeaderBase>& headers = elem->UserHeaders();

	for (TInt i = headers.Count() - 1; i >= 0; i--)
        {
        if (headers[i]->Name() == aName)
            {
            elem->DetachUserHeader(headers[i]);
            }
        }
	}
// -----------------------------------------------------------------------------
// CRegBindingImplementation::SetPropertyL
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::SetPropertyL(TUint32 aProperty,TBool aValue)
	{
	__ASSERT_ALWAYS(!IsContextActive(), User::Leave(KErrSIPInvalidRegistrationState));
	switch(aProperty)
		{
		case KSIPSendWithExpires:
			{
			iSendWithExpires = aValue;
			break;
			}
		case KSIPCacheOutboundProxyIP:
			{
			iCacheOutboundProxyIP = aValue;
			break;
			}
		default:
			User::Leave(KErrNotFound);
		}
	}

// -----------------------------------------------------------------------------
// CRegBindingImplementation::__DbgTestInvariant
// -----------------------------------------------------------------------------
//
void CRegBindingImplementation::__DbgTestInvariant() const
	{
    if (!iContact || !iAor)
        {
		User::Invariant();
		}
	}
