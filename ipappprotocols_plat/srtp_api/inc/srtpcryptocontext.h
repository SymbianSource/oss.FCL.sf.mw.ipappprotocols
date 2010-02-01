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
* Description:    Contains a default cryptographic context for SRTP/SRTCP streams.
*
*/




#ifndef __SRTP_CRYPTOCONTEXT_H__
#define __SRTP_CRYPTOCONTEXT_H__

// INCLUDES
#include <e32base.h>
#include "srtpdef.h"
#include <srtpcryptoparams.h>

// FORWARD DECLARATIONS
class CSRTPMasterKey;
class CSRTPMasterSalt;
class CSRTPCryptoHandler;
class TSrtpCryptoParams;
/**
* Contains a cryptographic context.
*/
class CSRTPCryptoContext : public CBase
    {
    //public: // enumerations


    public: // Construction
    
        /**
        * Use this function if the stream will use the SRTP session's default
        * cryptographic context. CSrtpSession take the ownership of
        * CSRTPCryptoContext if CSrtpSession is created with CSRTPCryptoContext
        * as input parameter.
        * @param aKey  The master key.
        * @param aSalt  The master salt.
		* @param aCryptoParams. Crypto Context parameters.
		* @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C static CSRTPCryptoContext* NewL( const CSRTPMasterKey* aKey,
                                            const CSRTPMasterSalt* aSalt, 
                                            const TSrtpCryptoParams& aCryptoParams);
        
         /**
        * Use this function if the stream will use the SRTP session's default
        * cryptographic context.
        * @param aKey  The master key.
        * @param aSalt  The master salt.
		* @param aCryptoParams. Crypto Context parameters.
		* @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C static CSRTPCryptoContext* NewLC( const CSRTPMasterKey* aKey,
                                            const CSRTPMasterSalt* aSalt, 
                                            const TSrtpCryptoParams& aCryptoParams );
        
        /**
        * Destructor
        * 
        */
        ~CSRTPCryptoContext();          

        /*
		* Gets current master key        
        * @return CSRTPMasterKey
        */
        IMPORT_C  const CSRTPMasterKey& MasterKey() const; 

        /*
		* Sets master key, the new master key will be used in encryptiuon 
		* and decryption. The previous master key will be deleted.        
        * @param aKey new master key
        */
        IMPORT_C void SetMasterKey( const CSRTPMasterKey* aKey); 
        
        /*
		* Gets current master salt             
        * @return CSRTPMasterSalt
        */
        IMPORT_C  const CSRTPMasterSalt& MasterSalt() const; 

        /*
		* Sets master salt, the new master salt key will be used in encryptiuon 
		* and decryption. The previous master salt key will be deleted.   
        * @param aSalt new master salt
        */
        IMPORT_C void SetMasterSalt(const CSRTPMasterSalt* aSalt); 
              

        /*
		* Check the validity of the whole TCryptoContext
        * @return validity of this CryptoContext
        */
        IMPORT_C TBool Valid() const;                 

        /*
		* Gets CryptoParams             
        * @return TSrtpCryptoParams Cryptocontext parameters
        */
        IMPORT_C const TSrtpCryptoParams& CryptoParams() ;

        /*
		* Feeback from Applicaiton to indicate that if the local ROC is
		* synchronized with the sender in RCC mode3.If it is not set, 
		* "Not-Synchronized" will be default. If it is synchronized, 
		* the packet would hence be processed using the local ROC.
		* Defined in RFC srtp-rcc-04 section 4.
		* @param aSync True if it is synchronized, False otherwise.
        */
        IMPORT_C void SetRCCm3Sync(TBool aSync) ;    
   
        /*
		* Check if the crypto context is equal
		* @param aContext True if it is equal, False otherwise.
        */
        TBool IsEqual(const CSRTPCryptoContext& aContext); 
          
        /*
		* Add cryptocontext user  
        * @param aObserver new CSRTPCryptoHandler instance
        */
        void AddCryptoChangeObserver(CSRTPCryptoHandler *aObserver);

        /*
		* Remove cryptocontext user  
        * @param aObserver CSRTPCryptoHandler instance
        */
        void RemoveCryptoChangeObserver(CSRTPCryptoHandler *aObserver);
        
    protected:
    	       
        /*
		* Update crypto parameters, internal use only 
        * @param aCryptoParams a new crypto parameters
        */
    	void UpdateCryptoParams(const  TSrtpCryptoParams& aCryptoParams );     

    private:
    
        /* copy constructor */
        CSRTPCryptoContext(const  TSrtpCryptoParams& aCryptoParams );
                            
        void ConstructL (  const CSRTPMasterKey* aKey,
                           const CSRTPMasterSalt* aSalt);
                           
        
        /* assignment operator */
        const CSRTPCryptoContext& operator=(const CSRTPCryptoContext& aContext);                     
    
        TBool FindHandler( CSRTPCryptoHandler *aObserver );

    private: // from Array


        /**
        * list of CSRTPCryptoHandler objects
        * owned by CSRTPCryptoContext object.
        */
        TSglQue<CSRTPCryptoHandler> iHandlerList;

        /**
        * CSRTPCryptoHandler iterator.
        * Owned by CSRTPCryptoContext object.
        */
        TSglQueIter<CSRTPCryptoHandler> iHandlerIter;


    
    private: // data
    	const CSRTPMasterKey *iKey;               // TSRTPMasterKey definitions
        const CSRTPMasterSalt *iSalt;             // TSRTPMasterSalt definitions
        TSrtpCryptoParams iCryptoParams;
        static const TInt iObserverOffset;   
        										  

    private:
    #ifdef EUNIT_TESTING
        friend class UT_CSRTPCryptoContext;
        friend class UT_CSRTPCryptoHandlerSRTCP;
    #endif              
    };


#endif // __SRTP_CRYPTOCONTEXT_H__
