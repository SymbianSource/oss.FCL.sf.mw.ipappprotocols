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
* Description:    Contains SRTP master salt class.
*
*/




#ifndef __SRTP_MASTER_SALT_H__
#define __SRTP_MASTER_SALT_H__

// INCLUDES
#include <e32def.h>
#include <e32des8.h>
#include "srtpdef.h"


/**
* Contains a salt used in encryption and decryption.
*/
class CSRTPMasterSalt : public CBase
    {
    public: // constructor
    
        /**
        * Two-phased constructor.
        * CSRTPCryptoContext takes the ownership of MasterKey and MasterSalt 
        * @param aKey  The master salt value.
        * @param aN_S  The length of the session salt
        * @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C static CSRTPMasterSalt* NewL( const TDesC8& aKey, 
                        TUint aN_S = KSRTPDefSessionSaltingKeyLength );

        /**
        * Two-phased constructor.
        * CSRTPCryptoContext takes the ownership of MasterKey and MasterSalt 
        * @param aKey  The master salt value.
        * @param aN_S  The length of the session salt
        * @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C static CSRTPMasterSalt* NewLC( const TDesC8& aKey, 
                        TUint aN_S = KSRTPDefSessionSaltingKeyLength );
                        
        /**
        * Destructor
        */
        ~CSRTPMasterSalt();                
       
        /*
		* Gets master salt
        * @return master salt
        */                                
        IMPORT_C  const TDesC8& MasterSalt() const;
                            
        /*
		* Gets length of the session salt
        * @return length of the session salt
        */                                
        IMPORT_C  TUint SaltLength() const;
        
        /*
		* Check the validity of the whole CSRTPMasterSalt
        * @return validity of this CSRTPMasterSalt
        */
        IMPORT_C  TBool Valid() const;                 
        

    private: // methods
        
        CSRTPMasterSalt( TUint aN_S );
        
        void ConstructL( const TDesC8& aKey );
    
        /* assignment operator */
         const CSRTPMasterSalt& operator=(const CSRTPMasterSalt& aSalt);  

    private: // data
        /**
        * The master salt value.
        * The length of the salt is determined by the Size() function.
        */
        HBufC8 *iKey;

        /**
        * The length of the session salt
        */
        const TUint iN_S;

    friend class CSRTPCryptoContext;
    
	#ifdef EUNIT_TESTING
        friend class UT_CSRTPMasterSalt;
    #endif         
    };


#endif // __SRTP_MASTER_SALT_H__
