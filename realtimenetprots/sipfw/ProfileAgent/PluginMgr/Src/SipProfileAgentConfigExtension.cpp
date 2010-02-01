// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SipProfileAgentConfigExtension.cpp
// Part of       : SIP / SIP Profile Server
//

#include <sipprofileagentextensionparams.h>
#include "SipProfileAgentConfigRetriever.h"

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigExtension::NewL
// -----------------------------------------------------------------------------
//
CSipProfileAgentConfigExtension* CSipProfileAgentConfigExtension::NewL()
    {
    CSipProfileAgentConfigExtension* self = CSipProfileAgentConfigExtension::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigExtension::NewLC
// -----------------------------------------------------------------------------
//
CSipProfileAgentConfigExtension* CSipProfileAgentConfigExtension::NewLC()
    {
    CSipProfileAgentConfigExtension* self = new (ELeave) CSipProfileAgentConfigExtension();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigExtension::CSipProfileAgentConfigExtension
// -----------------------------------------------------------------------------
//
CSipProfileAgentConfigExtension::CSipProfileAgentConfigExtension()
    {
    
    }

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigExtension::~CSipProfileAgentConfigExtension
// -----------------------------------------------------------------------------
//
CSipProfileAgentConfigExtension::~CSipProfileAgentConfigExtension()
    {
    delete iConfigRetriever;
    }

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigExtension::ConstructL
// -----------------------------------------------------------------------------
//
void CSipProfileAgentConfigExtension::ConstructL()
    {
    iConfigRetriever = CSipProfileAgentConfigRetriever::NewL();
    }

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigExtension::ExpiryValueL
// -----------------------------------------------------------------------------
//
TUint CSipProfileAgentConfigExtension::ExpiryValueL(TSIPProfileTypeInfo::TSIPProfileClass aClass,TProfileProperty aProperty)
    {
    return iConfigRetriever->ExpiryValueL(aClass,aProperty);
    }
