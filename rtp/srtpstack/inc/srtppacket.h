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
* Description:    General SRTP packet class (RTP/SRTP/RTCP/SRTCP)
*
*/




#ifndef __SRTP_PACKET_H__
#define __SRTP_PACKET_H__

// INCLUDES
#include <in_sock.h>
// FORWARD DECLARATIONS
class CSRTPCryptoHandler;

/**
* Contains general packet routines
*/
class CSRTPPacket : public CBase
    {
    public:
        /**
        * Get payload entry. Here the "payload" denotes the
        * encrypted portion (incl. RTCP/SRTCP).
        * @return pointer reference to the encrypted portion. 
        */        
        TPtrC8 Payload();    
        
        /**
        * Get header length.
        * @return length of the header.
        */        
        TUint HeaderLength();

        /**
        * Get payload length (== length of the encrypted portion).
        * @return length of the payload.
        */        
        TUint PayloadLength();
                    
        /**
        * Get header and encrypted portion, excluding possible 
        * MKI and authentication tag.
        * @return header and encrypted portion.
        */                            
        TPtrC8 HeaderAndPayload();      
                
        /**
        * Get the authentication portion for recv stream data
        * For RTP this gets header and payload as authentication portion
        * For RTCP it gets payload + SRTCP index
        * @param rtcp if it is rtcp packet
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return payload concatenated with ROC.
        */                            
        HBufC8* Get_Concatenated_MDataL(TBool rtcp); 
        
        /**
        * Create a new packet by allocating given size
        * and by copying header and given payload.
        * This can be any packet: RTP/SRTP/RTCP/SRTCP
        * @param aPacketSize  new packet size.
        * @param aPayloadPtr   pointer to the payload to be copied.
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return new packet with header and payload.
        */                            
        HBufC8* CopyHeaderAndPayloadL(TUint aPacketSize,
                                       TUint8* aPayloadPtr);
               
        ~CSRTPPacket();                


        /**
        * virtual function for creating decrypted packet,
        * implemented in CSRTPPacketSRTP/CSRTPPacketSRTCP,
        */                            
        virtual HBufC8* CreateDecryptedPacketL(TUint8* aDecryptedPayloadPtr);
        
        /**
        * virtual function for creating encrypted packet,
        * implemented in CSRTPPacketRTP/CSRTPPacketRTCP,
        */                            
        virtual HBufC8* CreateEncryptedPacketL(TUint8* aEncryptedPayloadPtr);        
        
        /**
        * virtual function for getting the authentication tag,
        * implemented in CSRTPPacketSRTP/CSRTPPacketSRTCP
        */                                    
        virtual TPtrC8 AuthenticationTag();
        
        /**
        * virtual function for getting the sequence number,
        * implemented in CSRTPPacketRTP
        */                                    
        virtual TUint16 SequenceNumber(); 
        
        /**
        * virtual function for setting the packet index value,
        * implemented in CSRTPPacketRTP
        */                                    
        virtual void SetPacketIndex(TUint64 aPacketIndex);
        
        /**
        * virtual function for getting the MKI value,
        * implemented in CSRTPPacketSRTP/CSRTPPacketSRTCP
        */                                            
        virtual TPtrC8 MasterKeyIdentifier();
        
        /**
        * virtual function for counting the sender-side packet index
        * implemented in CSRTPPacketRTP
        */                                            
        virtual void CountSenderPacketIndex(); 
         
        /**
        * virtual function for getting the packet index value,
        * implemented in CSRTPPacketRTP
        */                                    
        virtual TUint64 PacketIndex();        
                

		/**
        * Get Sender Roll-Over Counter from RTP/SRTP packets defined by RCC mode
        * @return 32 bit unsigned integer 
        */                              
        virtual TUint32 GetSenderROC(); 
    protected: //methods

        
        /*
        * Update header length member
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return void
        */
        void UpdatePayloadLengthL();
        
        /*
        * Update payload member in this class
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return void
        */
        void UpdatePayload();        

        /*
        * Read sequence number. 
        * @return 16 bit sequence number.
        */
        TUint16 GetSequenceNumber();

        void ConstructL();

        CSRTPPacket (const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler);
                                            
            
    protected: //data
        TPtrC8                        iPacket;  // packet
    	const TUint8                  *iDataP;  // pointer to packet
        CSRTPCryptoHandler&           iHandler; // crypto handler
        TPtrC8                        iPayload; // payload (encrypted portion)
        TUint                         iHeaderLength; // header length
        TUint                         iPayloadLength; // payload length
	
	private:
    #ifdef EUNIT_TESTING
        friend class UT_CSRTPCryptoHandlerSRTP;
        
    #endif 
    
        
    };

#endif // __SRTP_PACKET_H__
