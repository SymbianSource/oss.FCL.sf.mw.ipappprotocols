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
* Description:    Contains an SRTCP specific cryptographic context.
*
*/




#ifndef __SRTP_CRYPTOHANDLER_SRTCP_H__
#define __SRTP_CRYPTOHANDLER_SRTCP_H__

// INCLUDES
#include <e32base.h>
#include "srtpcryptocontext.h"
#include "srtpcryptohandler.h"
#include "msrtpcryptohandlercontextrtcp.h"

// FORWARD DECLARATIONS
class CSRTPStream;

/**
* Contains RTCP specific handling of cryptographic context.
*/
class CSRTPCryptoHandlerSRTCP : public CSRTPCryptoHandler, 
                                public MSRTPCryptoHandlerContextRTCP
    {    
    public: // Construction
        /**
        * Two-phased constructor.
        * @param aStream  RTCP stream
        * @returns CSRTPCryptoHandlerSRTCP
        */
        static CSRTPCryptoHandlerSRTCP* NewL( CSRTPStream& aStream);

        /**
        * Two-phased constructor.
        * @param aStream  RTCP stream
        * @returns CSRTPCryptoHandlerSRTCP
        */
        static CSRTPCryptoHandlerSRTCP* NewLC( CSRTPStream& aStream );
        
 
        ~CSRTPCryptoHandlerSRTCP( );    
                
        /**
        * Calculate authentication tag for an RTP packet.
        * Authentication is based on the current 
        * TSRTPAuthAlg algorithm setting in CryptoContext
        * If the operation fails, the function leaves
        * with a system error code.
        * @leave KErrNone if success, system-wide error code otherwise,
        * KErrNotFound means that authentication tags dont match
        */
        HBufC8* CalculateAuthTagL();
        
        
   private: // from MSRTPCryptoHandlerContext

        /*
        * Initialize plain packet class (CSRTPPacketRTCP) 
        * @param aPacket   RTCP packet.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        void InitializePlainPacketL(const TDesC8& aPacket);

        /*
        * Initialize encrypted packet class (CSRTPPacketSRTCP) 
        * @param aPacket   SRTCP packet.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        void InitializeEncryptedPacketL(const TDesC8& aPacket);
                        
        /*
        * Encrypt plain RTCP packet
        * @leave KErrNone if success, system-wide error code otherwise
        * @return encrypted packet
        */        
        HBufC8* EncryptL();
        
        /*
        * Decrypt ciphered SRTCP packet
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
        * Derive session encryption, salting and authentication keys
        * for RTCP
        * @return void
        */
        void DeriveSessionKeysL();
        
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
 
  	private: // from MSRTPCryptoHandlerContextRTCP       
       
       /*
        * Determines the index i
        * of an SRTCP packet by incrementing one on sender's side.         
        * @return void        
        */
        void CountSenderPacketIndexL();        
 
        /*
        * Initial the index i
        * of an SRTCP packet with initial packet index on sender's side.         
        * @return void        
        */		       
        void InitialPacketIndex();

       /*
        * Implements pseudocode on RFC 3711 Appendix A
        * for determining the index i of an SRTP packet with receiving index number
        * and current s_l value in receiver side
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void                
        */		
        void ReceiverPacketIndexL();
                
    private: // methods   
            
        void ConstructL();
                
        CSRTPCryptoHandlerSRTCP(  CSRTPStream& aStream );
        
        /*
        * Strip the E flag away from the 32 bit SRTCP index
        * @return void
        */
        void StripEBit();      
        
        /*
        * Write E flag and Srtcp Index number into Packet
        * E flag in SRTP always as True
        * @param aSrtcpPacketPtr   SRTCP packet.        
        * @return void
        */        
		void AddEflagAndSrtcpIndex(TUint8* aSrtcpPacketPtr);
		
        /*
        * Count IV and transform payload
        * @return HBufC8*, transformed encrypted / decrypted paylod
        */                                                                                         
        HBufC8* CountIV_And_TransformL();
        
        /*
        * Add auth tag to the given packet.
        * @param aSrtpPacketPtr  packet pointer
        * @leave KErrNone if success, system-wide error code otherwise,
        * @return void
        */
        void AddAuthTagToPacketL(TUint8* aSrtpPacketPtr);      
        
       /*
        * Check if Re-Key needed for receving and sending packet index
        * If Re-Key is needed will call Re-Key interface, and SRTP will reject
        * any packet if MasterKey is not updated      
        * @return void
        */        
		void IndexReKeyCheckL();
		
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
      
        /*
        * Add MKI to the given packet.
        * Common for RTP and RTCP.
        * @param aSrtpPacketPtr  packet pointer
        * @return void
        */
        void AddMKIToPacket(TUint8* aSrtpPacketPtr);   						 
		                     
 
        
    private: // data
        TUint64           iPcktsProcessed; // counter for packets processed
        TUint32           iSRTCPIndex;     // SRTCP index
        TUint32           iSRTCPIndexEStripped;  // SRTCP index with "E bit" removed
	    TInt			  iReplayIndexDelta;
        TUint64           iReplayPktIndex;       //packet index
        TUint64			  iSRTCPMasterKeyLifeTime;
        
	#ifdef EUNIT_TESTING
        friend class UT_CSRTPCryptoHandlerSRTCP;
        friend class UT_CSRTPCryptoHandlerSRTP;
    #endif 
    };

#endif // __SRTP_CRYPTOHANDLER_SRTCP_H__
