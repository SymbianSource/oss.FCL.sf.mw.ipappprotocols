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
* Description:    Provides an abstract class for key derivation operations
*
*/




#ifndef __MSRTP_KEYDERIVATION_H__
#define __MSRTP_KEYDERIVATION_H__

#include <e32base.h>

class MSRTPKeyDerivation
    {
    public:
        /**
        * This function is called by SRTP for key derivation operation (PRF_n)
        * regardless of the algorithm, the default is AESCM-128
        * Produces key stream based on input key and IV value.
        * PRF_n function is used to derive session key from desired master key.
        * @param aBitLength Encryption key length
        * @param aKey  Input key.
        * @param aIV  Integer Value (IV).
	    * Returns HBufC8* keystream 
        */        
        virtual HBufC8* PRF_128BitL(TUint aBitLength,
                                const TDesC8& aKey, 
                                const TDesC8& aIV) = 0;
                                
        virtual ~MSRTPKeyDerivation() { };                                
    };

#endif // __MSRTP_KEYDERIVATION_H__
