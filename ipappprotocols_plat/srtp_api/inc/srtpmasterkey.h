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
* Description:    SRTP Master key class.
*
*/




#ifndef __SRTP_MASTER_KEY_H__
#define __SRTP_MASTER_KEY_H__

// INCLUDES
#include <e32def.h>
#include <e32des8.h>
#include <e32base.h>
#include "srtpdef.h"


// STRUCT CLASSES

/**
* Contains an SRTP master key.
*/
class CSRTPMasterKey : public CBase
    {
    public: // constructor
    
        /**
        * Two-phased constructor.
        * CSRTPCryptoContext takes the ownership of MasterKey and MasterSalt 
        * @param aKey  The master key value. Local copy is made.
        * @param aMKI  The master key identifier value. Local copy is made.
        *   			MKI key is optional so if there is no MKI key then 
        *				gives empty string "".
        * @param aN_E  The bit length of the encryption key
        * @param aN_A  The bit length of the authentication key
        * @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C static CSRTPMasterKey* NewL( const TDesC8& aKey, 
                       const TDesC8& aMKI, 
                       TUint aN_E = KSRTPDefSessionEncryptionKeyLength, 
                       TUint aN_A = KSRTPDefSessionAuthenticationKeyLength );

        /**
        * Two-phased constructor.
        * CSRTPCryptoContext takes the ownership of MasterKey and MasterSalt 
        * @param aKey  The master key value. Local copy is made.
        * @param aMKI  The master key identifier value. Local copy is made.
        * @param aN_E  The bit length of the encryption key
        * @param aN_A  The bit length of the authentication key
        * @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C static CSRTPMasterKey* NewLC( const TDesC8& aKey, 
                       const TDesC8& aMKI, 
                       TUint aN_E = KSRTPDefSessionEncryptionKeyLength, 
                       TUint aN_A = KSRTPDefSessionAuthenticationKeyLength );
        
        /**
        * Destructor
        */
        ~CSRTPMasterKey();

        /*
		* Gets current master key value        
        * @return current master key value
        */                                
        IMPORT_C  const TDesC8& MasterKey() const;
        
        /*
		* Gets current MKI
        * @return current MKI
        */                                
        IMPORT_C  const TDesC8& MKI() const;
                            
        /*
		* Gets current encryption key length
        * @return encryption key length
        */                                
        IMPORT_C  TUint EncrKeyLength() const;

        /*
		* Gets current authorization key length
        * @return authorization key length
        */                                
        IMPORT_C  TUint AuthKeyLength() const;

   
        /*
		* Check the validity of the whole SRTPMasterKey
        * @return validity of this SRTPMasterKey
        */
        IMPORT_C  TBool Valid() const;                         
        
    private: //methods
    
        CSRTPMasterKey( TUint aN_E, TUint aN_A );	              
        
        void ConstructL (  const TDesC8& aKey, 
                           const TDesC8& aMKI );
    
      
        /* assignment operator */
         const CSRTPMasterKey& operator=(const CSRTPMasterKey& aKey);      
       
    private: //data
        
        /**
        * The master key value.
        * The length of the key is determined by the Size() function.
        */
        HBufC8 *iKey;

        /**
        * The master key identifier value
        */
        HBufC8* iMKI;

        /**
        * The bit length of the encryption key
        */
        TUint iN_E;

        /**
        * The bit length of the authentication key
        */
        TUint iN_A;

        friend class CSRTPCryptoContext;
 	#ifdef EUNIT_TESTING
        friend class UT_CSRTPMasterKey;
    #endif 
    };


#endif // __SRTP_MASTER_KEY_H__
