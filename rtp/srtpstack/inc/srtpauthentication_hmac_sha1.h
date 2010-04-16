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
* Description:    Provides class for authentication with HMAC-SHA1 algorithm.
*
*/




#ifndef __SRTP_AUTHENTICATION_HMAC_SHA1_H__
#define __SRTP_AUTHENTICATION_HMAC_SHA1_H__

#include "msrtpauthentication.h"

class CHMAC;

class CSRTPAuthentication_HMAC_SHA1 : public CBase, public MSRTPAuthentication
    {
    public:
        /**
        * Constructor.
        * @leave KErrNone if success, system-wide error code otherwise
        */
	    static CSRTPAuthentication_HMAC_SHA1* NewL();

        /**
        * Constructor.
        * @leave KErrNone if success, system-wide error code otherwise
        */
	    static CSRTPAuthentication_HMAC_SHA1* NewLC();
    
        /**
        * Destructor.
        */
	    ~CSRTPAuthentication_HMAC_SHA1();

        /**
        * This function is called by SRTP for authentication/integrity protection
        * with the default algorith, HMAC-SHA1.
        * Produces authentication tag based on input key and data M.
        * result IS truncated to n_tag leftmost bits
        * @param aBitLength  authentication tag length
        * @param aKey  authentication key (n_a)
        * @param aAuthPortion data to be integrity protected 
	    * Returns HBufC8* authentication tag (n_tag truncated) 
        */
        HBufC8* AuthenticateL(TUint aBitLength,
                            const TDesC8& aKey, 
                            const TDesC8& aAuthPortion,
                            const TDesC8& aRoc);
                 
    private:         // methods
        /**
        * Default constructor.
        */
        CSRTPAuthentication_HMAC_SHA1();  
                
        void ConstructL();  
        
        void CreateHmacL(const TDesC8& aKey);
        
    private: // data
        HBufC8* iKey;
        CHMAC* iHmac;
    };

#endif // __SRTP_AUTHENTICATION_HMAC_SHA1_H__

