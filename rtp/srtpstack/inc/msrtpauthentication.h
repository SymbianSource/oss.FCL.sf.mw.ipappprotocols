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
* Description:    Provides an abstract class for authentication (integrity protection)
*
*/




#ifndef __MSRTP_AUTHENTICATION_H__
#define __MSRTP_AUTHENTICATION_H__

#include <e32base.h>

class MSRTPAuthentication
    {
    public:
        /**
        * This function is called by SRTP for authentication/integrity protection
        * to create authentication tag regardless of the algorithm, 
        * the default algorith is HMAC-SHA1.
        * Produces authentication tag based on input key and data M.
        * result IS truncated to n_tag leftmost bits
        * @param aBitLength  authentication tag length, 32 or 80 bits
        * @param aKey  authentication key (n_a)
        * @param aAuthPortion data to be integrity protected 
        * @param aRoc packet ROC 
	    * Returns HBufC8* authentication tag (untruncated) 
        */
        virtual HBufC8* AuthenticateL(TUint aBitLength,
                                    const TDesC8& aKey, 
                                    const TDesC8& aAuthPortion,
                                    const TDesC8& aRoc) = 0;

        virtual ~MSRTPAuthentication() { };                                
    };

#endif // __MSRTP_AUTHENTICATION_H__
