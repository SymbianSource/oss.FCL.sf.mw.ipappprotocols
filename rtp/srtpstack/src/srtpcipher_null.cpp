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
* Description:    Provides class for NULL encryption/decryption.
*
*/




#include "srtpcipher_null.h"

// ---------------------------------------------------------------------------
// CSRTPCipherNULL::NewL
// ---------------------------------------------------------------------------
//
CSRTPCipherNULL* CSRTPCipherNULL::NewL()
    {
    CSRTPCipherNULL* self = new( ELeave ) CSRTPCipherNULL();
    return self;    
    }

// ---------------------------------------------------------------------------
// CSRTPCipherNULL::TransformL
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCipherNULL::TransformL(const TDesC8& /*aKey*/, 
                                const TDesC8& /*aIV*/, 
                                const TDesC8& aSrc )
    {    
    HBufC8* result = HBufC8::NewMaxL(aSrc.Length());

    //do nothing, but just copy the plain/decrypted text to destination
    *result = aSrc;
    return result;
    }

// ---------------------------------------------------------------------------
// CSRTPCipherNULL::~CSRTPCipherNULL
// ---------------------------------------------------------------------------
//
CSRTPCipherNULL::~CSRTPCipherNULL()
    {    
    }

// ---------------------------------------------------------------------------
// CSRTPCipherNULL::CSRTPCipherNULL
// ---------------------------------------------------------------------------
//
CSRTPCipherNULL::CSRTPCipherNULL()
    {    
        
    }
