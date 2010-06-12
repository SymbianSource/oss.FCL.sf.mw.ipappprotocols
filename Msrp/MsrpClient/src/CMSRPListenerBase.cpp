/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/


//  Include Files
#include <e32base.h>
#include "CMSRPListenerBase.h"


//  Member Functions

CMSRPListenerBase::CMSRPListenerBase(
    CMSRPSessionImplementation& aSessionImpl,
    MMSRPServerInterface& aServerInterface,
    TInt aPriority )
    : CActive( aPriority ),
    iSessionImpl( aSessionImpl ),
    iServerInterface( aServerInterface )
    {
    CActiveScheduler::Add( this ); // add to active scheduler
    }
        
        
CMSRPListenerBase::~CMSRPListenerBase()
    {
    }

