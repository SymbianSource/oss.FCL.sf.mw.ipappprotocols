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
* Description:    Provides class for AES CM 128 bit encryption/decryption.
*
*/




#include "srtpcipher_aescm128.h"
#include "srtpaesctrcrypto.h"

// ---------------------------------------------------------------------------
// CSRTPCipherAESCM128::NewL
// ---------------------------------------------------------------------------
//
CSRTPCipherAESCM128* CSRTPCipherAESCM128::NewL()
    {
    CSRTPCipherAESCM128* self = CSRTPCipherAESCM128::NewLC();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// CSRTPCipherAESCM128::NewLC
// ---------------------------------------------------------------------------
//
CSRTPCipherAESCM128* CSRTPCipherAESCM128::NewLC()
    {
    CSRTPCipherAESCM128* self = new( ELeave ) CSRTPCipherAESCM128();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CSRTPCipherAESCM128::TransformL
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCipherAESCM128::TransformL(const TDesC8& aKey, 
                                const TDesC8& aIV, 
                                const TDesC8& aSrc )
    {    
    // encrypt/decrypt using CSrtpAESCTRCrypto
    return iCipher->EncryptL(aKey, aIV, aSrc);
    }

// ---------------------------------------------------------------------------
// CSRTPCipherAESCM128::ConstructL
// ---------------------------------------------------------------------------
//
void CSRTPCipherAESCM128::ConstructL()
    {    
    // Create AES-CTR Wrapper
    iCipher = CSrtpAESCTRCrypto::NewL();        
        
    }

// ---------------------------------------------------------------------------
// CSRTPCipherAESCM128::~CSRTPCipherAESCM128
// ---------------------------------------------------------------------------
//
CSRTPCipherAESCM128::~CSRTPCipherAESCM128()
    {    
    // delete AES-CTR Wrapper
    delete iCipher;
    }

// ---------------------------------------------------------------------------
// CSRTPCipherAESCM128::CSRTPCipherAESCM128
// ---------------------------------------------------------------------------
//
CSRTPCipherAESCM128::CSRTPCipherAESCM128()
    {    
        
    }
