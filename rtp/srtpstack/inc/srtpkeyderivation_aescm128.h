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
* Description:    Provides class for AES CM 128 bit key derivation
*
*/




#ifndef __SRTP_KEYDERIVATION_AESCM128_H__
#define __SRTP_KEYDERIVATION_AESCM128_H__

#include "msrtpkeyderivation.h"

//FORWARD declarations
class CSrtpAESCTRCrypto;

class CSRTPKeyDerivation_AESCM128 : public CBase, public MSRTPKeyDerivation
    {
    public:
        /**
        * Constructor.
        * @leave KErrNone if success, system-wide error code otherwise
        */
	    static CSRTPKeyDerivation_AESCM128* NewL();

        /**
        * Constructor.
        * @leave KErrNone if success, system-wide error code otherwise
        */
	    static CSRTPKeyDerivation_AESCM128* NewLC();

        /**
        * Destructor.
        */
	    ~CSRTPKeyDerivation_AESCM128();

        /**
        * This function is called by SRTP for key derivation operation (PRF_n)
        * with the default algorithm AESCM-128
        * Produces keystream based on input key and IV value.
        * PRF_n function is used to derive session key from desired master key.
        * @param aBitLength  bit length of the key.
        * @param aKey  Input key.
        * @param aIV  Integer Value (IV).
	    * Returns HBufC8* keystream 
        */
        HBufC8* PRF_128BitL(TUint aBitLength, 
                         const TDesC8& aKey, 
                         const TDesC8& aIV);
                                
                                
    private:         // Data        
	    CSrtpAESCTRCrypto* iEncryptor;	    
                 
    private:         // methods
        /**
        * Default constructor.
        */
        CSRTPKeyDerivation_AESCM128();  
                
        void ConstructL();  
    };

#endif // __SRTP_KEYDERIVATION_AESCM128_H__


    

                                
