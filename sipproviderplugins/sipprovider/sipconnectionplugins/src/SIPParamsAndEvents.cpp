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
// SIPSCPR_subconparams.cpp
// SIP extension parameters definition
// 
//

/**
 @file
 @publishedAll
*/

#include <ecom/ecom.h>
#include <comms-infras/metatypearray.h>
#include <comms-infras/metatypevariablelen.h>
//#include <ecom/implementationproxy.h>
#include <comms-infras/metabuffer.h>
#include <cs_subconevents.h>

#include "SIP_subconparams.h"
#include "SIP_subconevents.h"

//-=========================================================
// Data/functions required for instantiating ECOM Plugin
//-=========================================================
  /* const TImplementationProxy ImplementationTable[] =
   	{
  	IMPLEMENTATION_PROXY_ENTRY(KSubConSIPParametersUid, CSIPSubConnExtensionParamsFactory::NewL),
  	IMPLEMENTATION_PROXY_ENTRY(KSubConSIPEventsUid, CSIPSubConnExtensionEventsFactory::NewL)
   	};
   
   
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
   {
   aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

   return ImplementationTable;
   }
   */

//-=========================================================
// SIP Extension Events & Paramemeters follow
//-=========================================================

START_ATTRIBUTE_TABLE( CSubConSIPResponseEvent, KSubConSIPEventsUid, KSubConSIPResponseEventType )
	REGISTER_ATTRIBUTE( CSubConSIPResponseEvent, iResponse, TMetaNumber)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CSubConSIPAuthenticationRequiredEvent, KSubConSIPEventsUid, KSubConSIPAuthenticationRequiredEventType )
	REGISTER_ATTRIBUTE( CSubConSIPAuthenticationRequiredEvent, iRealm, TMetaBuf8)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CSubConSIPNotificationEvent, KSubConSIPEventsUid, KSubConSIPNotificationEventType )
	REGISTER_ATTRIBUTE( CSubConSIPNotificationEvent, iNotification, TMetaBuf8)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CSubConSIPInviteParamSet, KSubConSIPParametersUid, KSubConSIPInviteParamsType )
	REGISTER_ATTRIBUTE( CSubConSIPInviteParamSet, iReqUri, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPInviteParamSet, iFrom, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPInviteParamSet, iTo, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPInviteParamSet, iContact, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPInviteParamSet, iContentType, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPInviteParamSet, iContentSubType, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPInviteParamSet, iContent, TMetaBuf8 )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CSubConSIPSubscribeParamSet, KSubConSIPParametersUid, KSubConSIPSubscribeParamsType )
	REGISTER_ATTRIBUTE( CSubConSIPSubscribeParamSet, iReqUri, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPSubscribeParamSet, iFrom, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPSubscribeParamSet, iTo, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPSubscribeParamSet, iContact, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPSubscribeParamSet, iEventType, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPSubscribeParamSet, iAcceptType, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPSubscribeParamSet, iAcceptSubType, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPSubscribeParamSet, iExpires, TMeta<TInt> )
	REGISTER_ATTRIBUTE( CSubConSIPSubscribeParamSet, iAutoRefresh, TMeta<TBool> )		
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CSubConSIPAuthenticateParamSet, KSubConSIPParametersUid, KSubConSIPAuthenticateParamsType )
	REGISTER_ATTRIBUTE( CSubConSIPAuthenticateParamSet, iUserName, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPAuthenticateParamSet, iPassword, TMetaBuf8 )
	REGISTER_ATTRIBUTE( CSubConSIPAuthenticateParamSet, iRealm, TMetaBuf8 )	
END_ATTRIBUTE_TABLE()


CSubConExtensionParameterSet* CSIPSubConnExtensionParamsFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
	case KSubConSIPInviteParamsType:
		return new (ELeave) CSubConSIPInviteParamSet;
	case KSubConSIPSubscribeParamsType:
		return new (ELeave) CSubConSIPSubscribeParamSet;
	case KSubConSIPAuthenticateParamsType:
		return new (ELeave) CSubConSIPAuthenticateParamSet;
	default:
		User::Leave(KErrNotFound);
		}
	return NULL;
	}

CSubConSIPInviteParamSet::~CSubConSIPInviteParamSet()	
	{
	// Cleanup the Memory associated with it
	iReqUri.Close();
	iFrom.Close();
	iTo.Close();
	iContact.Close();
	iContentType.Close();
	iContentSubType.Close();
	iContent.Close();
	}
	
CSubConSIPSubscribeParamSet::~CSubConSIPSubscribeParamSet()
	{
	// Cleanup the Memory associated with it
	iReqUri.Close();
	iFrom.Close();
	iTo.Close();
	iContact.Close();
	iEventType.Close();
	iAcceptType.Close();
	iAcceptSubType.Close();
	}
	
CSubConSIPAuthenticateParamSet::~CSubConSIPAuthenticateParamSet()
    {
	iUserName.Close();
	iPassword.Close();
	iRealm.Close();	        
    }


CSubConNotificationEvent* CSIPSubConnExtensionEventsFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
		case KSubConSIPResponseEventType:
		    return new (ELeave) CSubConSIPResponseEvent;
		case KSubConSIPAuthenticationRequiredEventType:
		    return new (ELeave) CSubConSIPAuthenticationRequiredEvent;
		case KSubConSIPNotificationEventType:
			return new (ELeave) CSubConSIPNotificationEvent;
	default:
		User::Leave(KErrNotFound);
		}
	return NULL;
	}

CSubConSIPResponseEvent::~CSubConSIPResponseEvent()
	{
	}

CSubConSIPAuthenticationRequiredEvent::~CSubConSIPAuthenticationRequiredEvent()
    {
    iRealm.Close();
    }

CSubConSIPNotificationEvent::~CSubConSIPNotificationEvent()
	{
	iNotification.Close();
	}
