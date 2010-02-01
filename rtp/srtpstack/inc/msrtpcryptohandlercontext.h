/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    implementation
*
*/



#ifndef MSRTPCRYPTOHANDLERCONTEXT_H
#define MSRTPCRYPTOHANDLERCONTEXT_H

#include <e32std.h>


// CLASS DECLARATION
/*
* A context for stream classes for using common crypto handling operations.
* These are common for both RTP and RTCP
*
*  @lib n/a
*/
class MSRTPCryptoHandlerContext
	{	
    public: 
        /*
        * virtual function for initializing 
        * plain packet class (CSRTPPacketRTP or CSRTPPacketRTCP) 
        * @param aPacket   RTP/RTCP packet.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void InitializePlainPacketL(const TDesC8& aPacket) = 0;

        /*
        * virtual function for initializing 
        * encrypted packet class (CSRTPPacketSRTP or CSRTPPacketSRTCP) 
        * @param aPacket   SRTP/SRTCP packet.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void InitializeEncryptedPacketL(const TDesC8& aPacket) = 0;
                                
        /*
        * virtual function for deriving session keys
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void DeriveSessionKeysL() = 0;        
        
        /*
        * virtual function for checking that crypto context MKI and 
        * packet MKI match.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void CheckMasterKeyIdentifierL() = 0;
        
        /*
        * virtual function for performing the authentication step
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void AuthenticateL() = 0;
        
        /*
        * virtual function for performing the replay protection step
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        virtual void ReplayProtectionL() = 0;        

        /*
        * virtual function for encrypting plain packet
        * @leave KErrNone if success, system-wide error code otherwise
        * @return encrypted packet
        */        
        virtual HBufC8* EncryptL() = 0;

        /*
        * virtual function for decrypting ciphered packet
        * @leave KErrNone if success, system-wide error code otherwise
        * @return plain packet
        */        
        virtual HBufC8* DecryptL() = 0;

        /*
        * virtual function for querying whether master key or salt has been updated
        * @return master keys update status
        */        
        virtual TBool MasterKeysUpdated() = 0;

        /*
        * virtual function for incrementing Replay Protection Index after 
        * the packet has been authenticated
        * @return void
        */                
        virtual void AddReplayIndex() =0;
	};
#endif  //MSRTPCryptoHandlerContext_H
