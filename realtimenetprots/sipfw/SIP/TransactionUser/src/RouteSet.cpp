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
// Name          : RouteSet.cpp
// Part of       : TransactionUser
// Version       : SIP/4.0 
//



#include "uricontainer.h"
#include "sipuri.h"
#include "sipaddress.h"
#include "siprouteheader.h"
#include "siprequest.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "MSipRegistrations.h"

#include "RouteSet.h"

#ifdef CPPUNIT_TEST
#include "TestCleanupStack.h"
#endif


// -----------------------------------------------------------------------------
// CRouteSet::NewL
// -----------------------------------------------------------------------------
//
CRouteSet* CRouteSet::NewL(CSIPRequest& aReq,
						   MSipRegistrations& aRegistrations,
						   TRegistrationId aRegisterId)
	{
	CRouteSet* self = new (ELeave) CRouteSet();
	CleanupStack::PushL(self);
	self->ConstructL(aReq, aRegistrations, aRegisterId);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CRouteSet::NewL
// -----------------------------------------------------------------------------
//
CRouteSet* CRouteSet::NewL(const CRouteSet& aRouteSet)
	{
	CRouteSet* self = new (ELeave) CRouteSet();
	CleanupStack::PushL(self);	
	self->ConstructL(aRouteSet);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CRouteSet::CRouteSet
// -----------------------------------------------------------------------------
//
CRouteSet::CRouteSet()
#ifdef CPPUNIT_TEST
    : iRoutes(1)
#endif
	{
	}

// -----------------------------------------------------------------------------
// CRouteSet::ConstructL
// If aReq contains Route-headers (=case of a pre-existing route set filled by
// upper subsystem), they are copied to route set. Otherwise the outbound
// proxy (if it exists) is put to the route set.
// -----------------------------------------------------------------------------
//
void CRouteSet::ConstructL(CSIPRequest& aReq,
						   MSipRegistrations& aRegistrations,
						   TRegistrationId aRegisterId)
	{
	iPreconfigRouteExists =
		aReq.HasHeader(SIPStrings::StringF(SipStrConsts::ERouteHeader));
	if (iPreconfigRouteExists)
		{
		TSglQueIter<CSIPHeaderBase> iter =
			aReq.Headers(SIPStrings::StringF(SipStrConsts::ERouteHeader));

	    for (CSIPHeaderBase* header = iter++; header; header = iter++)
			{
			AddRouteL(*header);
			}
		}
	else
		{
		const CSIPRouteHeader* proxy = 
		    aRegistrations.OutboundProxy(aRegisterId);
		if (proxy)
			{
			AddRouteL(*proxy);			
			}
		}
	}

// -----------------------------------------------------------------------------
// CRouteSet::ConstructL
// -----------------------------------------------------------------------------
//
void CRouteSet::ConstructL(const CRouteSet& aRouteSet)
	{
	iPreconfigRouteExists = aRouteSet.iPreconfigRouteExists;

	for (TInt i = 0; i < aRouteSet.iRoutes.Count(); ++i)
		{
		AddRouteL(*aRouteSet.iRoutes[i]);
		}
	}

// -----------------------------------------------------------------------------
// CRouteSet::~CRouteSet
// -----------------------------------------------------------------------------
//
CRouteSet::~CRouteSet()
	{
	iRoutes.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CRouteSet::AddRouteL
// -----------------------------------------------------------------------------
//
void CRouteSet::AddRouteL(const CSIPHeaderBase& aRouteHeader)
	{
    CSIPRouteHeader* route =
		static_cast<CSIPRouteHeader*>(aRouteHeader.CloneL());
	CleanupStack::PushL(route);
	iRoutes.AppendL(route);
	CleanupStack::Pop(route);
	}

// -----------------------------------------------------------------------------
// CRouteSet::CopyToRequestL
// -----------------------------------------------------------------------------
//
void CRouteSet::CopyToRequestL(CSIPRequest& aReq, TUint aIndex) const
	{
	for (TInt i = aIndex; i < iRoutes.Count(); ++i)
		{		
		aReq.AddHeaderL(*iRoutes[i]);
		}
	}

// -----------------------------------------------------------------------------
// CRouteSet::IsLrParamInTopRoute
// -----------------------------------------------------------------------------
//
TBool CRouteSet::IsLrParamInTopRoute() const
	{
	if (IsEmpty())
		{
		return EFalse;
		}
	
	CURIContainer& uri = iRoutes[0]->SIPAddress().URI();
	
    return (uri.IsSIPURI() &&
    		uri.SIPURI()->HasParam(SIPStrings::StringF(SipStrConsts::ELr)));
	}

// -----------------------------------------------------------------------------
// CRouteSet::IsEmpty
// -----------------------------------------------------------------------------
//
TBool CRouteSet::IsEmpty() const
	{
	return iRoutes.Count() == 0;
	}

// -----------------------------------------------------------------------------
// CRouteSet::PreconfigRouteExists
// -----------------------------------------------------------------------------
//
TBool CRouteSet::PreconfigRouteExists() const
	{
	return iPreconfigRouteExists;
	}
	
// -----------------------------------------------------------------------------
// CRouteSet::NextHop
// -----------------------------------------------------------------------------
//
CURIContainer& CRouteSet::NextHop(CURIContainer& aRemoteTarget) const
    {
    if (IsEmpty())
    	{
    	return aRemoteTarget;
    	}

    return iRoutes[0]->SIPAddress().URI();
    }

// -----------------------------------------------------------------------------
// CRouteSet::TopRoute
// -----------------------------------------------------------------------------
//
const CURIContainer* CRouteSet::TopUri() const
	{
	if (IsEmpty())
		{
		return NULL;
		}

    return &iRoutes[0]->SIPAddress().URI();
	}

// -----------------------------------------------------------------------------
// CRouteSet::AddToBeginningL
// -----------------------------------------------------------------------------
//
void CRouteSet::AddToBeginningL(const CSIPRouteHeader& aRoute,
								TBool aOverwrite)
	{
	CSIPRouteHeader* route = static_cast<CSIPRouteHeader*>(aRoute.CloneL());
	CleanupStack::PushL(route);

	if (aOverwrite && !IsEmpty())
		{
		CSIPRouteHeader* old = iRoutes[0];
		iRoutes.Remove(0);
		delete old;
		}

	iRoutes.InsertL(route, 0);
	CleanupStack::Pop(route);
	}

// -----------------------------------------------------------------------------
// CRouteSet::IsInSet
// -----------------------------------------------------------------------------
//
TBool CRouteSet::IsInSet(const CURIContainer& aUri) const
	{
	for (TInt i = 0; i < iRoutes.Count(); ++i)
		{
		if (aUri == iRoutes[0]->SIPAddress().URI())
			{
			return ETrue;
			}
		}

	return EFalse;
	}
