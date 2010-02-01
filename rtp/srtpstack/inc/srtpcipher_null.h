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




#ifndef __SRTP_CIPHER_NULL_H__
#define __SRTP_CIPHER_NULL_H__

#include "msrtpcipher.h"

//FORWARD declarations
class CSrtpAESCTRCrypto;

class CSRTPCipherNULL: public CBase, public MSRTPCipher
    {
    public:
        /**
        * Constructor.
        * @leave KErrNone if success, system-wide error code otherwise
        */
	    static CSRTPCipherNULL* NewL();

        /**
        * Destructor.
        */
	    ~CSRTPCipherNULL();

        /**
        * Inherited from MSRTPCipher.
        * This function is called by SRTP for NULL encryption/decryption
        * Just copies the input text to output text.
        * @param aKey  Input key.
        * @param aIV  Integer Value (IV).
        * @param aSrc source data which will be transformed.
	    * Returns HBufC8* decrypted/encrypted source        
        */
        HBufC8* TransformL(const TDesC8& aKey, 
                                const TDesC8& aIV, 
                                const TDesC8& aSrc );
                                
    private:         // Data        
                 
    private:         // methods
        /**
        * Default constructor.
        */
        CSRTPCipherNULL();  
    };

#endif // __SRTP_CIPHER_NULL_H__
