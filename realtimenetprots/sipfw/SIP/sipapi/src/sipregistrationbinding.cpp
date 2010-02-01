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
// Name          : SIPRegistrationBinding.cpp
// Part of       : SIPAPI
// Version       : SIP/5.0 
//



#include <uri8.h>
#include "sipdefs.h"
#include "sipregistrationbinding.h"
#include "RegBindingImplementation.h"
#include "sipclienttransaction.h"
#include "sipmessageelements.h"
#include "sipcontactheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRegistrationBinding*
CSIPRegistrationBinding::NewL(CSIPConnection& aConnection,
					   		  CSIPToHeader* aAor,
					   		  CSIPContactHeader* aContact,
					   		  CSIPRefresh* aRefresh,
					   		  CSIPRouteHeader* aOutboundProxy,
					   		  CUri8* aRemoteUri,
                       		  CSIPFromHeader* aFrom)
    {
    CSIPRegistrationBinding* self =
    	CSIPRegistrationBinding::NewLC(aConnection,
					                   aAor,                                                     
                                       aContact,
                                       aRefresh,
					                   aOutboundProxy,
                                       aRemoteUri,
                                       aFrom);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRegistrationBinding*
CSIPRegistrationBinding::NewLC(CSIPConnection& aConnection,
							   CSIPToHeader* aAor,
							   CSIPContactHeader* aContact,
							   CSIPRefresh* aRefresh,
							   CSIPRouteHeader* aOutboundProxy,
							   CUri8* aRemoteUri,
                        	   CSIPFromHeader* aFrom)
    {
    __ASSERT_ALWAYS(aAor && aContact, User::Leave(KErrArgument));

    if (aContact->HasParam(SIPStrings::StringF(SipStrConsts::EExpires)))
        {        
        TUint expires = aContact->ExpiresParameter();
        __ASSERT_ALWAYS(expires >= KSIPMinExpirationValue,
        			    User::Leave(KErrArgument));
        }

    CSIPRegistrationBinding* self =
    	new (ELeave) CSIPRegistrationBinding();
    CleanupStack::PushL(self);
    self->ConstructL(aConnection,
    				 aAor,
                     aContact,
                     aRefresh,
                     aOutboundProxy,
                     aRemoteUri,
                     aFrom);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::CSIPRegistrationBinding
// -----------------------------------------------------------------------------
//
CSIPRegistrationBinding::CSIPRegistrationBinding()
    {
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::ConstructL
// aAor and aContact have been checked in NewLC so they aren't checked again
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBinding::ConstructL(CSIPConnection& aConnection,
										 CSIPToHeader* aAor,								
								  		 CSIPContactHeader* aContact,                                  
							  	  		 CSIPRefresh* aRefresh,
								  		 CSIPRouteHeader* aOutboundProxy,
								  		 CUri8* aRemoteUri,
                                  		 CSIPFromHeader* aFrom)
    {    
    iImplementation = CRegBindingImplementation::NewL(*this,
    												  aConnection,
    												  aAor,
    												  aContact,
    												  aRefresh,
    												  aOutboundProxy,
    												  aRemoteUri,
    												  aFrom);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::~CSIPRegistrationBinding
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRegistrationBinding::~CSIPRegistrationBinding()
    {
    delete iImplementation;    
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::IsContextActive
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPRegistrationBinding::IsContextActive() const
    {
    __TEST_INVARIANT;
	return iImplementation->IsContextActive();	
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::ContextId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CSIPRegistrationBinding::ContextId() const
    {
    __TEST_INVARIANT;
    return iImplementation->ContextId();    
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::RegisterL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPRegistrationBinding::RegisterL(CSIPMessageElements* aElements)
    {
    __TEST_INVARIANT;
    return iImplementation->RegisterL(aElements);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::DeregisterL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPRegistrationBinding::DeregisterL(CSIPMessageElements* aElements)
    {
    __TEST_INVARIANT;
    return iImplementation->DeregisterL(aElements);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::UpdateL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPRegistrationBinding::UpdateL(CSIPMessageElements* aElements)
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(aElements, User::Leave(KErrArgument));

	return iImplementation->UpdateL(aElements);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::UpdateL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPRegistrationBinding::UpdateL(TUint aExpirationValue,
                          		 CSIPMessageElements* aElements)
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(aExpirationValue >= KSIPMinExpirationValue,
    				User::Leave(KErrArgument));

    return iImplementation->UpdateL(aElements, &aExpirationValue);    
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::SetOutboundProxyL
// -----------------------------------------------------------------------------
//
EXPORT_C void
CSIPRegistrationBinding::SetOutboundProxyL(CSIPRouteHeader* aOutboundProxy)
    {
    __TEST_INVARIANT;

    iImplementation->SetOutboundProxyL(aOutboundProxy);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::Aor
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPToHeader& CSIPRegistrationBinding::Aor() const
    {
    __TEST_INVARIANT;
    return iImplementation->Aor();
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::ContactHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPContactHeader& CSIPRegistrationBinding::ContactHeader() const
    {
    __TEST_INVARIANT;
    return iImplementation->ContactHeader();    
    }
    
// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::ContactHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPContactHeader& CSIPRegistrationBinding::ContactHeader()
    {
    __TEST_INVARIANT;
    return iImplementation->ContactHeader();    
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::OutboundProxy
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPRouteHeader* CSIPRegistrationBinding::OutboundProxy() const
    {
    __TEST_INVARIANT;
    return iImplementation->OutboundProxy();    
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::SIPRefresh
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPRefresh* CSIPRegistrationBinding::SIPRefresh() const
    {
    __TEST_INVARIANT;
    return iImplementation->SIPRefresh();    
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::SIPRefresh
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRefresh* CSIPRegistrationBinding::SIPRefresh()
    {
    __TEST_INVARIANT;
    return iImplementation->SIPRefresh();    
    }
    
// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::Connection
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConnection* CSIPRegistrationBinding::Connection()
    {
    __TEST_INVARIANT;
    return iImplementation->Connection();    
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::Connection
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPConnection* CSIPRegistrationBinding::Connection() const
    {
    __TEST_INVARIANT;
    return iImplementation->Connection();    
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::operator==
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPRegistrationBinding::operator==(
    const CSIPRegistrationBinding& aRegistration) const
    {
    __TEST_INVARIANT;    
    return *iImplementation == *aRegistration.iImplementation;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::RegisteredContact
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPContactHeader* 
    CSIPRegistrationBinding::RegisteredContact() const
    {
    __TEST_INVARIANT;
    return iImplementation->RegisteredContact();
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::SetProperty
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPRegistrationBinding::SetProperty(TUint32 aProperty,TBool aValue)
	{
	__TEST_INVARIANT;
	TRAPD(err, iImplementation->SetPropertyL(aProperty,aValue));
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::Implementation
// -----------------------------------------------------------------------------
//
CRegBindingImplementation& CSIPRegistrationBinding::Implementation()
	{
	__TEST_INVARIANT;
	return *iImplementation;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CSIPRegistrationBinding::__DbgTestInvariant() const
	{
    if (!iImplementation)
        {
		User::Invariant();
		}
	}

