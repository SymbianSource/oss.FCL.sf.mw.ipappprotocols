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
* Description:    SRTP packet class (RTP)
*
*/




#ifndef __SRTP_PACKET_RTP_H__
#define __SRTP_PACKET_RTP_H__

// INCLUDES
#include <in_sock.h>
#include "srtppacket.h"
#include <e32math.h>

// FORWARD DECLARATIONS
class CSRTPCryptoHandler;

/**
* Contains general RTP-specific packet routines
*/
class CSRTPPacketRTP : public CSRTPPacket
    {
    public:
    	static CSRTPPacketRTP* NewL(const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler);
                      
        /**
        * Get sequence number.
        * @return 16 bit sequence number.
        */                              
        TUint16 SequenceNumber();         

        /**
        * Get packet index.
        * @return TUint64 packet index.
        */                              
        TUint64 PacketIndex();

        /**
        * Set packet index.
        * @param aPacketIndex  new packet index
        * @return void
        */                              
        void SetPacketIndex(TUint64 aPacketIndex);

        /**
        * Count sender packet index.
        * @return void
        */                              
        void CountSenderPacketIndex();  

        /**
        * Create encrypted packet.
        * @param aEncryptedPayloadPtr  encrypted payload to be copied
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return HBufC8* encrypted packet
        */                              
        HBufC8* CreateEncryptedPacketL(TUint8* aEncryptedPayloadPtr);
        
        /**
        * virtual function for creating decrypted packet.
        * @param aDecryptedPayloadPtr  decrypted payload to be copied
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return HBufC8* decrypted packet
        * implemented in CSRTPPacketSRTP.
        */                              
        virtual HBufC8* CreateDecryptedPacketL(TUint8* aDecryptedPayloadPtr);

        /**
        * virtual function for getting the authentication tag
        * implemented in CSRTPPacketSRTP.
        */                              
        virtual TPtrC8 AuthenticationTag();

        /**
        * virtual function for getting the master key identifier
        * implemented in CSRTPPacketSRTP.
        */                              
        virtual TPtrC8 MasterKeyIdentifier();
        
		/*
        * Update header length member
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return void
        */
        void UpdateHeaderLengthL();  
        
        /**
        * Get Sender Roll-Over Counter from RTP/SRTP packets defined by RCC mode
        * @return 32 bit unsigned integer 
        */                              
        TUint32 GetSenderROC(); 
                        
    protected: //methods        
        CSRTPPacketRTP (const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler);
        ~CSRTPPacketRTP(); 

        /**
        * Update payload length information
        * @return void
        */                              
        void UpdatePayloadLength();
        void ConstructL();   
        
        TBool TagWithROCLengthL();  
                
    private: //methods
    
       /**
        * Read sequence number and update class member
        * @return void
        */                              
        void UpdateSequenceNumber();    
                                    
       /**
        * Count size of the encrypted packet
        * @return TUint size of the encrypted packet
        */                              
        TUint CountEncryptedPacketSizeL();        

       /**
        * Count size of the decrypted packet
        * @return TUint size of the decrypted packet
        */                              
        TUint CountDecryptedPacketSize();                
            
    private: //data    
        TUint16                       iSequenceNumber;   // sequence number
        TUint64                       iPacketIndex;      // packet index   
        TUint32 					  iSenderROC; 		 //used in RCC modes       
     #ifdef EUNIT_TESTING
        friend class UT_CSRTPCryptoHandlerSRTP;
        friend class UT_CSRTPCryptoHandler;
    #endif     
              
    };

#endif // __SRTP_PACKET_RTP_H__

