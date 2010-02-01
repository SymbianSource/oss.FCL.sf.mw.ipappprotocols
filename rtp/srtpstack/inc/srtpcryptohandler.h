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
* Description:    Contains a cryptographic handler for SRTP/SRTCP streams.
*
*/




#ifndef __SRTP_CRYPTOHANDLER_H__
#define __SRTP_CRYPTOHANDLER_H__

// INCLUDES
#include <e32base.h>
#include "srtpdef.h"
#include "msrtpcryptohandlercontext.h"
#include "srtpcryptocontext.h"
#include "srtpcryptoparams.h"

// FORWARD DECLARATIONS
class CSRTPMasterKey;
class CSRTPMasterSalt;
class CSRTPCryptoContext;
class CSRTPStream;
class MSRTPCipher;
class MSRTPKeyDerivation;
class MSRTPAuthentication;
class CSRTPPacket;

/**
* Contains RTP/RTCP common handling of cryptographic context.
*/
class CSRTPCryptoHandler : public CBase, public MSRTPCryptoHandlerContext
    {
        
   public: // from MSRTPCryptoHandlerContext   
      
        /*
        * virtual function for initializing 
        * plain packet class (CSRTPPacketRTP or CSRTPPacketRTCP) 
        * @param aPacket   RTP/RTCP packet.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void InitializePlainPacketL(const TDesC8& aPacket)=0;

        /*
        * virtual function for initializing 
        * encrypted packet class (CSRTPPacketSRTP or CSRTPPacketSRTCP) 
        * @param aPacket   SRTP/SRTCP packet.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void InitializeEncryptedPacketL(const TDesC8& aPacket)=0;
        /* 
        * Virtual function for deriving session 
        * encryption, salting and authentication keys
        * for RTP or RTCP 
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void                
        */
        virtual void DeriveSessionKeysL()=0; 
               
        /*
        * virtual function for checking that crypto context MKI and 
        * packet MKI match.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void CheckMasterKeyIdentifierL()=0;   
             
        /*
        * virtual function for performing the authentication step
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void AuthenticateL()=0;
        
        /*
        * virtual function for performing the replay protection step
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void ReplayProtectionL()=0;

        /*
        * virtual function for encrypting plain packet
        * @leave KErrNone if success, system-wide error code otherwise
        * @return encrypted packet
        */        
        virtual HBufC8* EncryptL()=0;
        
        /*
        * virtual function for decrypting ciphered packet
        * @leave KErrNone if success, system-wide error code otherwise
        * @return plain packet
        */        
        virtual HBufC8* DecryptL()=0;       
        
        /*
        * function for querying whether master key or salt has been updated
        * @return master keys update status
        */        
        virtual TBool MasterKeysUpdated()=0;
 
   public:  // from CSRTPCryptoContext           
        /**
        * Callback from CSRTPCryptoContext when a master key is changed.
        */
        void SRTPMasterKeyChanged();        
        /**
        * Callback from CSRTPCryptoContext when a master salt is changed.
        */
        void SRTPMasterSaltChanged();
        
        
        
   public:                    
        /**
        * Get roll-over counter value.
        * @return current ROC value. 
        */
        TUint32 ROC( ) const;  
        /*
        * Get corresponding CSRTPCryptoContext
        * (either session or stream specific)
        * @return CSRTPCryptoContext
        */                                            
        CSRTPCryptoContext& Context();
        
        const TSrtpCryptoParams& CryptoParams();
   
        virtual ~CSRTPCryptoHandler( );                
                                                 
    protected: // methods   
    
        CSRTPCryptoHandler( CSRTPStream& aStream);
                
        void ConstructL( );
                
                        
        /*
        * Deletes session keys (encryption, 
        * salting and authentication) and sets them NULL
        * @return void
        */                                            
        void DeleteSessionKeys();    

        /**
        * Set roll-over counter value.
        * @param aROC The new ROC value. 
        * @return void
        */
        void SetROC( TUint32 aROC );

        /**
        * Get 32 bit SSRC value
        * @return SSRC 
        */
        TUint SSRC() const;
             
        /*
        * Count X value, 
        * which  used in session key derivation
        * Common to RTP and RTCP.
        * The length of X is the same as master key length
        * (padding will be performed later)
        * @param a_R, r, index DIV key_derivation_rate
        * @param aLabel, label 8 bit RTP/RTCP label 
        * @param aIndexLength, 
        * @param &aRes  resulting n bit x value
        * @return void
        */
        void Count_X(TUint64 a_R,
                    TInt8 aLabel, 
                    const TUint64 aIndexLength, 
                    TDes8 &aRes);                
        

        
        /*
        * Check Packet Index with Replay window
        * Common for RTP and RTCP.
        * @param aDelta  the difference between the stored packet Index and 
        *  current Packet Index
        * @return KErrNone if checks Ok, if not, return KErrArgument
        */
        TInt ReplayCheck(TInt aDelta) ;
        
        /*
        * Set Encryptor and Auth
        * Common for RTP and RTCP.
        * @param aEngAlg  Encrption Alg using for cryptor handler
        * @param aAuthAlg Authentication Algorithem using for crypto handler
        * @return void
        */
        void SetEncAndAuthL (TSRTPEncAlg aEngAlg, 
										TSRTPAuthAlg aAuthAlg);

    public: 
        /**
        * iHandlerOffset, List offset.
        * Owned by CCryptoContext object.
        */
        static const TInt iHandlerOffset;

        /**
        * iHandlerLink, Link to list.
        * Owned by CSRTPCryptoHandler object.
        */
        TSglQueLink iHandlerLink;
        
    protected: // data
    
        CSRTPStream&            iStream;         // stream
        
        CSRTPPacket*            iCurrentPacket;   // packet class        
        // algorithms: 
        MSRTPKeyDerivation*     iKeyDeriver;     // key divider algorithm
        MSRTPAuthentication*    iAuthenticator;  // authentication algorithm
        MSRTPCipher*            iCipher;         // encryption/decryption alg.
        TBool                   iMasterDataUpdated; // is master key or salt updated   
                    
        // session keys:
        HBufC8*                 iSessionEncrKey;  // session encryption/decryption key (k_e)
        HBufC8*                 iSessionAuthKey;  // session authentication key (k_a)
        HBufC8*                 iSessionSaltKey;  // session salting key (k_s)
		TBool                   iReKey; // is master key or salt updated    
		TUint64			  		iBitmap;		//Replay List
		TUint64					iNumPackets; //Number of packets has been recived / sending
    	
    private:
    #ifdef EUNIT_TESTING
        friend class UT_CSRTPCryptoHandler;
        friend class UT_CSRTPCryptoHandlerSRTP;
        friend class UT_CSRTPStreamIn;
        friend class UT_CSRTPStreamOut;
    #endif 
                   
    };
#endif // __SRTP_CRYPTOHANDLER_H__
