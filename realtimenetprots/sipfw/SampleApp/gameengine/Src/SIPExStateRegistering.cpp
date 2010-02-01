
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



// INCLUDES
#include    "SIPExStateRegistering.h"
#include    "SIPExGameEngine.h"

// -----------------------------------------------------------------------------
// TSIPExStateRegistering::ProfileEnabled
// Sets info text to the UI and changes the active state to the Registered.
// -----------------------------------------------------------------------------
//
void TSIPExStateRegistering::ProfileEnabled( 
    CSIPExEngine* aContext, 
    TUint32 /*aSIPProfileId*/ )
    {
    Info( aContext, KProfileEnabled() );
    ChangeState( aContext, aContext->iStateRegistered );
    }

// -----------------------------------------------------------------------------
// TSIPExStateRegistering::ProfileError
// Sets info text to the UI and changes the active state to the Idle.
// -----------------------------------------------------------------------------
//
void TSIPExStateRegistering::ProfileError( 
    CSIPExEngine* aContext,
    TInt /*aError*/ )
    {
    Info( aContext, KProfEnabError() );
    ChangeState( aContext, aContext->iStateIdle );
    }

// End of file
