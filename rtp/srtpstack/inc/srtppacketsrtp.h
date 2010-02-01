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
* Description:    SRTP packet class (SRTP)
*
*/




#ifndef __SRTP_PACKET_SRTP_H__
#define __SRTP_PACKET_SRTP_H__

// INCLUDES
#include <in_sock.h>
#include "srtppacket.h"
#include "srtppacketrtp.h"


// FORWARD DECLARATIONS
class CSRTPCryptoHandler;
/**
* Contains general SRTP-specific packet routines
*/
class CSRTPPacketSRTP : public CSRTPPacketRTP
    {
    public:
    
        /**
	    * Constructor.
	    * @param aPacket a SRTP packet
	    * @param aHandler the cryptohandler for this packet
	    * @leave KErrNone if success, system-wide error code otherwise
	    */
    	static CSRTPPacketSRTP* NewL(const TDesC8& aPacket,
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
        * @return HBufC8* decrypted packet
        */                                      
        HBufC8* CreateDecryptedPacketL(TUint8* aDecryptedPayloadPtr);

    private: //methods

        CSRTPPacketSRTP (const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler);
        ~CSRTPPacketSRTP();                
                        
        void ConstructL();          

        /**
        * Update payload length information
        * @return void
        */                              
        void UpdatePayloadLengthL();

        /**
        * Update authentication tag
        * @return void
        */                                      
        void UpdateAuthenticationTagL();        

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
        TPtrC8 GetAuthenticationTagL();   
        
        /*
        * Read Master Key Identifier (MKI) from packet
        * @return TPtrC8 pointer to MKI
        */                                            
        TPtrC8 GetMasterKeyIdentifier();        
                          
                    
    private: //data
        TPtrC8                 iAuthenticationTag;   // auth. tag
        TPtrC8                 iMasterKeyIdentifier; // MKI
    
    };

#endif // __SRTP_PACKET_SRTP_H__


