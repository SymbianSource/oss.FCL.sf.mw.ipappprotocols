/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Provides class for authentication with NULL algorithm.
*
*/




#include "srtpauthentication_null.h" 
#include "srtpaesctrcrypto.h"
#include "hash.h"
#include "srtpdef.h"

    
// ---------------------------------------------------------------------------
// CSRTPAuthentication_NULL::NewL
// ---------------------------------------------------------------------------
//
CSRTPAuthentication_NULL* CSRTPAuthentication_NULL::NewL()
    {
    CSRTPAuthentication_NULL* self = new( ELeave ) CSRTPAuthentication_NULL();
    return self;    
    }


// -----------------------------------------------------------------------------
// SRTPAuthentication_NULL::AuthenticateL
// -----------------------------------------------------------------------------
//
HBufC8* CSRTPAuthentication_NULL::AuthenticateL(TUint /*aBitLength*/,
                                                    const TDesC8& /*aKey*/, 
                                                    const TDesC8& /*aM*/,
                                                    const TDesC8& /*aRoc*/)
    {
    return NULL;
    }
    
    
// ---------------------------------------------------------------------------
// CSRTPAuthentication_NULL::~CSRTPAuthentication_NULL
// ---------------------------------------------------------------------------
//
CSRTPAuthentication_NULL::~CSRTPAuthentication_NULL()
    {    
    }

// ---------------------------------------------------------------------------
// CSRTPAuthentication_NULL::CSRTPAuthentication_NULL
// ---------------------------------------------------------------------------
//
CSRTPAuthentication_NULL::CSRTPAuthentication_NULL()
    {    
        
    }

