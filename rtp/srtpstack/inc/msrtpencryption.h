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
* Description:    Provides an abstract class for ecryption operations
*
*/




#ifndef __MSRTP_ENCRYPTION_H__
#define __MSRTP_ENCRYPTION_H__

#include <e32base.h>

class MSRTPEncryption
    {
    public:
        /**
        * This function is called by SRTP for cipher operation 
        * regardless of the algorithm, the default is AESCM-128
        * Produces key stream and performs an XOR operation for the plain text
        * @param aKey  Input key.
        * @param aIV  Integer Value (IV).
	    * Returns HBufC8* decrypted/encrypted source        
        */
        virtual HBufC8* EncryptL(const TDesC8& aKey, 
                                const TDesC8& aIV, 
                                const TDesC8& aSrc ) = 0;
        virtual ~MSRTPEncryption() { };
    };

#endif // __MSRTP_ENCRYPTION_H__
