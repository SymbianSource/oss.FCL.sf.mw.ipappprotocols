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




#ifndef __SRTP_CIPHER_AESCM128_H__
#define __SRTP_CIPHER_AESCM128_H__

#include "msrtpcipher.h"

//FORWARD declarations
class CSrtpAESCTRCrypto;

class CSRTPCipherAESCM128: public CBase, public MSRTPCipher
    {
    public:
        /**
        * Constructor.
        * @leave KErrNone if success, system-wide error code otherwise
        */
	    static CSRTPCipherAESCM128* NewL();

        /**
        * Constructor.
        * @leave KErrNone if success, system-wide error code otherwise
        */
	    static CSRTPCipherAESCM128* NewLC();
    
        /**
        * Destructor.
        */
	    ~CSRTPCipherAESCM128();

        /**
        * Inherited from MSRTPCipher.
        * This function is called by SRTP for encryption/decryption with AESCM-128 
        * Produces key stream and performs an XOR operation for the plain text
        * @param aKey  Input key.
        * @param aIV  Integer Value (IV).
        * @param aSrc source data which will be transformed.
	    * Returns HBufC8* decrypted/encrypted source        
        */
        HBufC8* TransformL(const TDesC8& aKey, 
                                const TDesC8& aIV, 
                                const TDesC8& aSrc );
                                
    private:         // Data        
	    CSrtpAESCTRCrypto* iCipher;	    
                 
    private:         // methods
        /**
        * Default constructor.
        */
        CSRTPCipherAESCM128();  
                
        void ConstructL();  
    };

#endif // __SRTP_CIPHER_AESCM128_H__
