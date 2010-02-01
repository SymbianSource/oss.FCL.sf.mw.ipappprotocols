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
* Description:    SRTP packet class (SRTCP)
*
*/




#ifndef __SRTP_PACKET_SRTCP_H__
#define __SRTP_PACKET_SRTCP_H__

// INCLUDES
#include <in_sock.h>
#include "srtppacket.h"
#include "srtppacketrtcp.h"


// FORWARD DECLARATIONS
class CSRTPCryptoHandler;
/**
* Contains general SRTCP-specific packet routines
*/
class CSRTPPacketSRTCP : public CSRTPPacketRTCP
    {
    public:
    
        /**
	    * Constructor.
	    * @param aPacket a SRTCP packet
	    * @param aHandler the cryptohandler for this packet
	    * @leave KErrNone if success, system-wide error code otherwise
	    */
    	static CSRTPPacketSRTCP* NewL(const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler);

        /**
        * Get authentication tag
        * @return authentication tag
        */                                      
        TPtrC8 AuthenticationTag();   
        
        /**
        * Get master key identifier
        * @return TPtrC8 master key identifier
        */                                                                 
        TPtrC8 MasterKeyIdentifier();

        
        /**
        * Allocate size for decrypted packet and copy payload + header
        * @param aDecryptedPayloadPtr  decrypted payload to be copied
        * @return HBufC8* decrypted packet
        */                                      
        HBufC8* CreateDecryptedPacketL(TUint8* aDecryptedPayloadPtr);

    private: //methods

        CSRTPPacketSRTCP (const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler);
        ~CSRTPPacketSRTCP();                
                        
        void ConstructL();          
        
       /**
        * Read Index number and update class member
        * @return void
        */                              
        void UpdatePacketIndex();                          

        /**
        * Update payload length information
        * @return void
        */                              
        void UpdatePayloadLength();
        
        /**
        * Update authentication tag
        * @return void
        */                                      
        void UpdateAuthenticationTag();    
            
        /**
        * Update master key identifier
        * @return void
        */                                      
        void UpdateMasterKeyIdentifier(); 
                   
        /**
        * Count size of the decrypted packet
        * @return TUint size of the decrypted packet
        */                                      
        TUint CountDecryptedPacketSize();  
        
                
        /*
        * Read Master Key Identifier (MKI) from packet
        * @return TPtrC8 pointer to MKI
        */                                            
        TPtrC8 GetAuthenticationTag();   
        
        /*
        * Read Master Key Identifier (MKI) from packet
        * @return TPtrC8 pointer to MKI
        */                                            
        TPtrC8 GetMasterKeyIdentifier();        
                            
                    
    private: //data
        TPtrC8                iAuthenticationTag;   // auth. tag
        TPtrC8                iMasterKeyIdentifier; // MKI
    
    };

#endif // __SRTP_PACKET_SRTCP_H__


