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
* Description:    Represents an SRTP stream.
*
*/




#ifndef __SRTP_STREAM_H__
#define __SRTP_STREAM_H__

// INCLUDES
#include <e32def.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CSRTPSession;
class CSRTPCryptoContext;
class MSRTPReKeyingObserver;
class CSRTPCryptoHandlerSRTP;
class CSRTPCryptoHandlerSRTCP;

/**
* Represents an SRTP stream.
*/
class CSRTPStream : public CBase
    {
    public: 
    
        /**
        * iStreamOffset, List offset.
        */
        static const TInt iStreamOffset;

        /**
        * iStreamLink, Link to list.
        */
        TSglQueLink iStreamLink;
    
    public:  
        
        /**
        * Set roll-over counter value.
        * RFC 3711 3.3.1 ROC() can not be reset to 0 after Re-Key
        * @param aROC The new ROC value. 
        * @return void
        */
        IMPORT_C void SetROC( TUint32 aROC );

        /**
        * Get roll-over counter value.
        * @return current ROC value. 
        */
        IMPORT_C TUint32 ROC( ) const;
        
        /**
        * Get SSRC
        * @return SSRC.
        */
        IMPORT_C TUint SSRC( ) const;
        
        /**
        * Set SSRC value if the stream's SSRC is 0 as late binding
        * @param aSSRC The new SSRC value. 
        * @return void
        */
 		void SetSSRC( const TUint aSSRC );
 		
        /**
        * Get cryptographic context associated for this stream
        * or the one associated for the session
        * @return CSRTPCryptoContext
        */
        CSRTPCryptoContext& GetCryptoContext();
        
        /**
        * Call for Rekey 
        * @return void
        */
        void ReKeyNeeded();        
   

		/**
        * Call for KeyExpired
        * @return void
        */
        void KeyExpired();    
          
        /**
        * Destructor
        */
        ~CSRTPStream();

        /**
        * Tell stream type is InStream or OutStream
        * @return TBool
        */
		IMPORT_C TBool StreamType();
		
		/**
        * Check if crypto context has been set
        * @return ETrue if crypto context is existed, otherwise EFalse
        */
        TBool IsContextSet();
        
        /**
        * Delete the existed crypto context and re-initial the states
        * @param None
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
		virtual void UpdateCryptoAndStatesL()=0;
                                
    protected:  //methods
        CSRTPStream (CSRTPSession& aSession, 
        				const TUint aSSRC, 
        				TBool aStreamType );

        CSRTPStream (CSRTPSession& aSession,
                        TUint aSSRC,
                       CSRTPCryptoContext* aCon,
                       MSRTPReKeyingObserver& aObs,
                       TBool aStreamType );

        CSRTPStream (CSRTPSession& aSession,
        			TBool aStreamType);

        CSRTPStream (CSRTPSession& aSession,
                       CSRTPCryptoContext* aCon,
                       MSRTPReKeyingObserver& aObs,
                       TBool aStreamType );
        
        CSRTPStream (CSRTPSession& aSession,
                        TUint aSSRC,
                       MSRTPReKeyingObserver& aObs,
                       TBool aStreamType );
        CSRTPStream (CSRTPSession& aSession,
                       MSRTPReKeyingObserver& aObs,
                       TBool aStreamType );       
                       
        /**
        * Create RTP crypto handler
        * @return void
        */
        void CreateCryptoHandlerSRTPL();        

        /**
        * Create RTCP crypto handler
        * @return void
        */
        void CreateCryptoHandlerSRTCPL();
        
        
        /**
        * Delete the existed crypto handler and create a new one
        * @return void
        */
        void UpdateCryptoHandlerL(); 
        

    protected:  //data
        CSRTPSession& iSession;             // access to session
        CSRTPCryptoContext* iContext;       // crypto context
        MSRTPReKeyingObserver* iObserver;         // observer (application)    
        TBool   iStrmSpecificCrypto;              // do we have stream or session-
                                                  // spesific context
        CSRTPCryptoHandlerSRTP* iHandlerRTP;          // crypto handling class for RTP
        CSRTPCryptoHandlerSRTCP* iHandlerRTCP;         // crypto handling class for RTCP
        TUint32 iROC;                             // unsigned 32 bit ROC
        TUint iSSRC;                        // SSRC     
        TBool iIsStreamInType;    
        TBool iRekey;                   
 		    
    private:  
    #ifdef EUNIT_TESTING
        friend class UT_CSRTPCryptoHandlerSRTP;
        friend class UT_CSRTPCryptoHandlerSRTCP;
        friend class UT_CSRTPCryptoHandler;
        friend class UT_CSRTPStreamIn;        
        friend class UT_CSRTPStreamOut;
        friend class UT_CSRTPStream;      
        friend class UT_CSRTPCryptoContext; 
    #endif 
    };

#endif // __SRTP_STREAM_H__
