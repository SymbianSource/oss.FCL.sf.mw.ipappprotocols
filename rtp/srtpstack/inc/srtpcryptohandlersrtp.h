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
* Description:    Contains an SRTP specific cryptographic context.
*
*/




#ifndef __SRTP_CRYPTOHANDLER_SRTP_H__
#define __SRTP_CRYPTOHANDLER_SRTP_H__

// INCLUDES
#include <e32base.h>
#include "srtpcryptocontext.h"
#include "srtpcryptohandler.h"
#include "msrtpcryptohandlercontextrtp.h"


// FORWARD DECLARATIONS
class CSRTPStream;

/**
* Contains RTP specific handling of cryptographic context.
*/
class CSRTPCryptoHandlerSRTP : public CSRTPCryptoHandler,
                                public MSRTPCryptoHandlerContextRTP
    {
    public: // Construction
        /**
        * Two-phased constructor.
        * @param aStream  aStream for this cryptohandler.
        * @leave KErrNone if success, system-wide error code otherwise
        */
        static CSRTPCryptoHandlerSRTP* NewL( CSRTPStream& aStream );
        /**
        * Two-phased constructor.
        * @param aStream  aStream for this cryptohandler.
        * @leave KErrNone if success, system-wide error code otherwise  
        */
        static CSRTPCryptoHandlerSRTP* NewLC( CSRTPStream& aStream);
               
 

        /**
        * Calculate authentication tag for an RTP packet.
        * Authentication is based on the current 
        * TSRTPAuthAlg algorithm setting in CryptoContext
        * If the operation fails, the function leaves
        * with a system error code.
        * @leave KErrNone if success, system-wide error code otherwise,
        */
        HBufC8* CalculateAuthTagL();

        ~CSRTPCryptoHandlerSRTP( );
        
               
   private: // from MSRTPCryptoHandlerContext
        /*
        * Initialize plain packet class (CSRTPPacketRTP) 
        * @param aPacket   RTP packet.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        void InitializePlainPacketL(const TDesC8& aPacket);

        /*
        * Initialize encrypted packet class (CSRTPPacketSRTP) 
        * @param aPacket   SRTP packet.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        void InitializeEncryptedPacketL(const TDesC8& aPacket);

        /* 
        * Derive session encryption, salting and authentication keys
        * for RTP        
        * @return void                
        */
        void DeriveSessionKeysL();
        
        /*
        * Encrypt plain RTP packet
        * @leave KErrNone if success, system-wide error code otherwise
        * @return encrypted packet
        */        
        HBufC8* EncryptL();
        
        /*
        * Decrypt ciphered SRTP packet
        * @leave KErrNone if success, system-wide error code otherwise
        * @return plain packet
        */        
        HBufC8* DecryptL();        
        
        /*
        * Check that crypto context MKI and 
        * packet MKI match.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        void CheckMasterKeyIdentifierL();

        /*
        * Perform the authentication step
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        void AuthenticateL();
        
        /*
        * Perform the replay protection step
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        void ReplayProtectionL();
       
        /*
        * function for querying whether master key or salt has been updated
        * @return master keys update status
        */        
        TBool MasterKeysUpdated();

        /*
        * function for updating Replay index after ReplayProtection Check
        * @return void
        */            
        void AddReplayIndex() ;
        
        
    private: // from MSRTPCryptoHandlerContextRTP
        /*
        * Determines the index i
        * of an SRTP packet with given sequence number SEQ on sender's side.         
        * @return void        
        */
        void CountSenderPacketIndexL();        

        /*
        * Implements pseudocode on RFC 3711 Appendix A
        * for determining the index i of an SRTP packet with given sequence number SEQ
        * and current s_l value in receiver side
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void                
        */
        void CountReceiverPacketIndexL();        

        /*
        * Assign receiver counter s_l to the value of first sequence number. 
        * @return void                
        */
        void SetReceiver_s_l();        

        /*
        * Conditionally update ROC in sender's side:
        * Each time SEQ wraps MOD 2^16, the sender side must 
        * increment ROC by one MOD 2^32.
        * @return void                
        */
        void UpdateROC();        

        /*
        * Conditionally update s_l and ROC based on v value. 
        * See RFC 3711 section 3.3.1 for more information
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void                
        */
        void Update_s_l_and_RocL();
        
       /*
        * Add auth tag to the given packet.
        * @param aSrtpPacketPtr  packet pointer
        * @leave KErrNone if success, system-wide error code otherwise,
        * @return void
        */
        void AddAuthTagToPacketL(TUint8* aSrtpPacketPtr); 
        
        /*
        * Check if packets is carrying Roll-Over Counter
        * @return TBool return True if it should carry, otherwise return False.
        */
        TBool CarryROC();    
        
        /*
        * Call AuthenticateL in MSRTPAuthentication -interface
        * to get authentication tag based on algorithm
        * that is currently set in crypto context
        * @param aAuthenticatedArea  portion of data to be authenticated
        * @param aRoc Roll-Over counter 
        * @leave KErrNone if success, system-wide error code otherwise,
        * @return authentication tag 
        */
        HBufC8* DoAuthenticationL(const TDesC8& aAuthenticatedArea,
        						 const TDesC8& aRoc);
      private: // methods   

        /*
        * Count IV and transform payload
        * @return HBufC8*, transformed encrypted / decrypted paylod
        */    
        HBufC8* CountIV_And_TransformL();

        /*
        * Add MKI to the given packet.
        * Common for RTP and RTCP.
        * @param aSrtpPacketPtr  packet pointer
        * @return void
        */
        void AddMKIToPacket(TUint8* aSrtpPacketPtr); 
         
        
        /*
        * Count sender packet index when receiving the packet under RCC mode 1
        * based on the attached sender ROC
        * @return TUint64 sender packet index.
        */              
        TInt64 CountSndIndxbySndROC();
                                               
        void ConstructL();

        void ReKeyCheck();
        
        TBool CSRTPCryptoHandlerSRTP::IfAddAuthTag();
        
        CSRTPCryptoHandlerSRTP( CSRTPStream& aStream );
                     
        
    private: // data
        TUint32                     iROC;            // //only using when RCC mode as sender Roc
        TUint16                     iS_L;           // s_l, highest received SEQ   
        TUint16                     iCurrentSeq;     // current sequence number  
        TUint64                     iPktIndex;       //packet index
        TInt32                      iV;             // local v value
        TInt						iReplayIndexDelta;
		TUint64			  			iSRTPMasterKeyLifeTime;
    private:
    #ifdef EUNIT_TESTING
        friend class UT_CSRTPCryptoHandlerSRTP;
        friend class UT_CSRTPCryptoHandler;
    #endif 
        
    };

#endif // __SRTP_CRYPTOHANDLER_SRTP_H__
