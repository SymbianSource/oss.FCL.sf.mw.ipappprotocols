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
// Name          : CSigCompMgr.cpp
// Part of       : Dialogs
// Version       : SIP/4.0 
//



#include "CSigCompMgr.h"
#include "MSigComp.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "siprouteheader.h"
#include "sipaddress.h"
#include "siphostport.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSigCompMgr::NewL
// -----------------------------------------------------------------------------
//
CSigCompMgr* CSigCompMgr::NewL (TUint32 aIapId, MSigComp& aSigComp)
	{
	CSigCompMgr* self = CSigCompMgr::NewLC (aIapId, aSigComp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSigCompMgr::NewLC
// -----------------------------------------------------------------------------
//
CSigCompMgr* CSigCompMgr::NewLC (TUint32 aIapId, MSigComp& aSigComp)
	{
	CSigCompMgr* self = new(ELeave)CSigCompMgr(aIapId, aSigComp);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSigCompMgr::CSigCompMgr
// -----------------------------------------------------------------------------
//
CSigCompMgr::CSigCompMgr(TUint32 aIapId, MSigComp& aSigComp)
 : iIapId (aIapId),
   iSigComp (aSigComp)
	{
    iCompParamName = SIPStrings::StringF(SipStrConsts::EComp);
    iSigCompValue = SIPStrings::StringF(SipStrConsts::ESigComp);
	}

// -----------------------------------------------------------------------------
// CSigCompMgr::~CSigCompMgr
// -----------------------------------------------------------------------------
//
CSigCompMgr::~CSigCompMgr()
	{
    if (iCompartmentId)
        {
        iSigComp.RemoveCompartment(iCompartmentId);
        }
	}

// -----------------------------------------------------------------------------
// CSigCompMgr::UpdateL
// -----------------------------------------------------------------------------
//
void CSigCompMgr::UpdateL (const CURIContainer& aRemoteTarget,
                           const RPointerArray<CSIPRouteHeader>& aRouteSet,
                           CSIPURI& aLocalTarget,
						   TBool aUpdateLocalTarget)
    {
	// Update the compartment usage
    if (aRouteSet.Count() > 0 && aRouteSet[0] != 0)
        {
        const CSIPAddress& routeAddr = aRouteSet[0]->SIPAddress();
        if (routeAddr.URI().IsSIPURI())
            {
            UpdateL(*(routeAddr.URI().SIPURI()));           
            }
        }
    else 
        {
        if (aRemoteTarget.IsSIPURI())
            {
            UpdateL(*aRemoteTarget.SIPURI());
            }
        }
	// Update local target
	if (aUpdateLocalTarget)
		{
		if (iCompartmentId && !HasSigCompParameter(aLocalTarget)) 
			{
			// SigComp used: Add comp=sigcomp to local target
			aLocalTarget.SetParamL(iCompParamName,iSigCompValue);
			}
		else
			{
			aLocalTarget.DeleteParam(iCompParamName);
			}
		}
    }

// -----------------------------------------------------------------------------
// CSigCompMgr::CompartmentId
// -----------------------------------------------------------------------------
//    
TUint32 CSigCompMgr::CompartmentId() const
    {
    return iCompartmentId;
    }

// -----------------------------------------------------------------------------
// CSigCompMgr::HasSigCompParameter
// -----------------------------------------------------------------------------
//
TBool CSigCompMgr::HasSigCompParameter (const CSIPURI& aURI) const
    {
    return (aURI.ParamValue(iCompParamName) == iSigCompValue);
    }

// -----------------------------------------------------------------------------
// CSigCompMgr::UpdateL
// -----------------------------------------------------------------------------
//
void CSigCompMgr::UpdateL (const CSIPURI& aURI)
    {
    if (HasSigCompParameter(aURI))
        {
        if (!iSigComp.IsSupported())
            {
            User::Leave(KErrNotSupported);
            }
    	if (!iCompartmentId)
        	{
        	iCompartmentId = iSigComp.CreateCompartmentL(iIapId);
        	}
        }
    else
        {
        if (iCompartmentId)
            {
            iSigComp.RemoveCompartment(iCompartmentId);
            iCompartmentId = 0;
            }
        }
    }
