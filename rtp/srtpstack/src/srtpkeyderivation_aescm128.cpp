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
* Description:    Provides a class for AES CM 128 key derivation
*
*/




#include "srtpkeyderivation_aescm128.h"
#include "srtpaesctrcrypto.h"
#include "srtpdef.h"
#include "srtputils.h"

// ---------------------------------------------------------------------------
// CSRTPEncryptionAESCM128::NewL
// ---------------------------------------------------------------------------
//
CSRTPKeyDerivation_AESCM128* CSRTPKeyDerivation_AESCM128::NewL()
    {
    CSRTPKeyDerivation_AESCM128* self = CSRTPKeyDerivation_AESCM128::NewLC();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// CSRTPKeyDerivation_AESCM128::NewLC
// ---------------------------------------------------------------------------
//
CSRTPKeyDerivation_AESCM128* CSRTPKeyDerivation_AESCM128::NewLC()
    {
    CSRTPKeyDerivation_AESCM128* self = new( ELeave ) CSRTPKeyDerivation_AESCM128();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CSRTPKeyDerivation_AESCM128::PRF_128BitL
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPKeyDerivation_AESCM128::PRF_128BitL(TUint aBitLength, 
                                              const TDesC8& aKey, 
                                              const TDesC8& aIV)
    {    
    HBufC8* paddedIV = NULL;
    HBufC8* keystream = NULL;
    TBuf8<1> padChar;

    // if the IV value is shorter than the AES block size (128 bits, 16 octets),
    // --> do the padding       
    if (aIV.Length() < KAESCM128BlockSizeInBytes)
        {
        paddedIV = HBufC8::NewMaxL(KAESCM128BlockSizeInBytes);
        CleanupStack::PushL(paddedIV);
        TUint8* ptr = const_cast<TUint8*>(paddedIV->Des().Ptr());
        
        Mem::FillZ( ptr, KAESCM128BlockSizeInBytes);        
        Mem::Copy( ptr, aIV.Ptr(), aIV.Length() );                    
        }
        
     if (!paddedIV)
        {
        // get the AES keystream with the original IV value       
        keystream = iEncryptor->KeystreamL(aBitLength, aKey, aIV);            
        }
     else
        {        
        // get the AES keystream with the padded IV value       
        keystream = iEncryptor->KeystreamL(aBitLength, aKey, *paddedIV);            
        }

    if (paddedIV)
        {
        CleanupStack::Pop(paddedIV);
        delete paddedIV; paddedIV=NULL;
        } 
        
    return keystream;
    }


// ---------------------------------------------------------------------------
// CSRTPKeyDerivation_AESCM128::ConstructL
// ---------------------------------------------------------------------------
//
void CSRTPKeyDerivation_AESCM128::ConstructL()
    {    
    // Create AES-CTR Wrapper
    iEncryptor = CSrtpAESCTRCrypto::NewL();        
        
    }

// ---------------------------------------------------------------------------
// CSRTPKeyDerivation_AESCM128::~CSRTPKeyDerivation_AESCM128
// ---------------------------------------------------------------------------
//
CSRTPKeyDerivation_AESCM128::~CSRTPKeyDerivation_AESCM128()
    {    
    // delete AES-CTR Wrapper
    delete iEncryptor;
        
    }

// ---------------------------------------------------------------------------
// CSRTPKeyDerivation_AESCM128::CSRTPKeyDerivation_AESCM128
// ---------------------------------------------------------------------------
//
CSRTPKeyDerivation_AESCM128::CSRTPKeyDerivation_AESCM128()
    {    
        
    }
