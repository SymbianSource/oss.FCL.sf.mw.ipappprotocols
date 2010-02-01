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
* Description:    SRTP packet class (RTCP)
*
*/




#ifndef __SRTP_PACKET_RTCP_H__
#define __SRTP_PACKET_RTCP_H__

// INCLUDES
#include <in_sock.h>
#include "srtppacket.h"

// FORWARD DECLARATIONS
class CSRTPCryptoHandler;

/**
* Contains general RTCP-specific packet routines
*/
class CSRTPPacketRTCP : public CSRTPPacket
    {
    public:
    	static CSRTPPacketRTCP* NewL(const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler);
                              
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
        * implemented in CSRTPPacketSRTCP.
        */                              
        virtual HBufC8* CreateDecryptedPacketL(TUint8* aDecryptedPayloadPtr);
        
        /**
        * virtual function for getting the authentication tag
        * implemented in CSRTPPacketSRTCP.
        */                                      
        virtual TPtrC8 AuthenticationTag();
        
        /**
        * virtual function for getting the master key identifier
        * implemented in CSRTPPacketSRTP.
        */                              
        virtual TPtrC8 MasterKeyIdentifier();
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
        
		/*
        * Update header length member
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return void
        */
        void UpdateHeaderLengthL();        
                
    protected: //methods        
        CSRTPPacketRTCP (const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler);
        ~CSRTPPacketRTCP(); 

        /**
        * Update payload length information
        * @return void
        */                              
        void UpdatePayloadLength();
        void ConstructL();     
                
    private: //methods

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
            
    protected: //data    
        TUint32                     iPacketIndex;       // packet index                      
    };

#endif // __SRTP_PACKET_RTCP_H__

