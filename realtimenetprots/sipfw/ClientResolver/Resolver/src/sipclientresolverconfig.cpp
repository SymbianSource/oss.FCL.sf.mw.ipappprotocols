// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : sipclientresolverconfig.cpp
// Part of       : SIP / SIP Client Resolver
// Version       : SIP/6.0
//



#include "sipclientresolverconfig.h"
#include "sipclientresolverconfigcrkeys.h" 


// -----------------------------------------------------------------------------
// CSipClientResolverConfig::NewL
// -----------------------------------------------------------------------------
//
CSipClientResolverConfig* CSipClientResolverConfig::NewL()
	{   
    CSipClientResolverConfig* self = CSipClientResolverConfig::NewLC();
    CleanupStack::Pop(self);
    return self;    
	}
	
// -----------------------------------------------------------------------------
// CSipClientResolverConfig::NewLC
// -----------------------------------------------------------------------------
//
CSipClientResolverConfig* CSipClientResolverConfig::NewLC()
	{   
    CSipClientResolverConfig* self = new(ELeave)CSipClientResolverConfig();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;    
	}

// -----------------------------------------------------------------------------
// CSipClientResolverConfig::CSipClientResolverConfig
// -----------------------------------------------------------------------------
//
CSipClientResolverConfig::CSipClientResolverConfig()
	{
	}

// -----------------------------------------------------------------------------
// CSipClientResolverConfig::ConstructL
// -----------------------------------------------------------------------------
//	
void CSipClientResolverConfig::ConstructL()
    {
    iCenRep = CRepository::NewL(KCRUidSIPClientResolverConfig);
    }

// -----------------------------------------------------------------------------
// CSipClientResolverConfig::~CSipClientResolverConfig
// -----------------------------------------------------------------------------
//
CSipClientResolverConfig::~CSipClientResolverConfig()
	{
	delete iCenRep;
	}

// -----------------------------------------------------------------------------
// CSipClientResolverConfig::GetUIDsForUserNameL
// -----------------------------------------------------------------------------
//	
void CSipClientResolverConfig::GetUIDsForUserNameL(
    const TDesC8& aUserName,
    RArray<TUid>& aUids)
    {
    RArray<TUint32> keys;
    CleanupClosePushL(keys);
    TInt err = iCenRep->FindEqL(KSIPClientResolverUserNameMask,
                                KSIPClientResolverFieldTypeMask,
                                aUserName,
                                keys);

    for (TInt i=0; !err && i < keys.Count(); i++)
        {
        TUint32 key = (KSIPClientResolverUserNameMask^(keys[i]));       
        // Read the related plug-in UID
        TUint32 uidKey = KSIPClientResolverPluginUIDMask|key;
        TInt uidValue(0);
        User::LeaveIfError(iCenRep->Get(uidKey,uidValue));
        TUid uid;
        uid.iUid = uidValue;
        // Duplicate plug-in UIDs not added, but are allowed
        err = aUids.InsertInUnsignedKeyOrder(uid);
        if (err != KErrNone && err != KErrAlreadyExists)
            {
            User::Leave(err);
            }
        err = KErrNone;
        }
        
    CleanupStack::PopAndDestroy(&keys);
    }
