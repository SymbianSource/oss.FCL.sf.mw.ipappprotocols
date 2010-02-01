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



#include "tsrtpstreaminstatelatebind.h"
#include "msrtpstreamincontext.h"
#include "msrtpcryptohandlercontext.h"
#include "msrtpcryptohandlercontextrtp.h"
#include "msrtpcryptohandlercontextrtcp.h"
#include "srtputils.h"

// ----------------------------------------------------------------------------
// TSRTPStreamInStateLateBind::TSRTPStreamInStateLateBind
// ----------------------------------------------------------------------------
//
TSRTPStreamInStateLateBind::TSRTPStreamInStateLateBind(
    MSRTPStreamInContext& aStreamContext,
    MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
    MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext)
    : TSRTPStreamInStateBase(aStreamContext,
                            aCryptoHandlerRTPContext, 
                            aCryptoHandlerRTCPContext)
    {
    }

// ----------------------------------------------------------------------------
// TSRTPStreamInStateLateBind::DoUnprotectSrtpL
// ----------------------------------------------------------------------------
//		
HBufC8* TSRTPStreamInStateLateBind::DoUnprotectSrtpL(const TDesC8& aPacket)
    {
    SRTP_DEBUG_DETAIL( "TSRTPStreamInStateLateBind::DoUnprotectSrtpL ENTRY");
   	SRTP_DEBUG_TINT_VALUE( "Encrypted PacketLength is", aPacket.Length() );                 
             
    // first initialize current encrypted RTP packet..
    iCryptoHandlerRTPContext.InitializeEncryptedPacketL(aPacket);    
    
    
    // for the first RTP packet, we need to set 
    // receiver s_l value to current sequence number, see RFC
    iCryptoHandlerRTPContext.SetReceiver_s_l();
       
    // Step 2 (in RFC 3711, section 3.3):
    // count the packet index i
    iCryptoHandlerRTPContext.CountReceiverPacketIndexL();
    
  	SRTP_DEBUG_DETAIL( "CheckMasterKeyIdentifier" );
    
    // check MKI if it is used
    iCryptoHandlerRTPContext.CheckMasterKeyIdentifierL();
   	
   	SRTP_DEBUG_DETAIL( "DeriveSessionKeysL" );
                  
    // derive session keys..
    iCryptoHandlerRTPContext.DeriveSessionKeysL();

    // do the replay protection, based on sender packetIndex 
    //note that in RCC mode, it set packet index as sender one
    iCryptoHandlerRTPContext.ReplayProtectionL();
    
    SRTP_DEBUG_DETAIL( "AuthenticateL" );
      
    // do the authentication (integrity check)
    iCryptoHandlerRTPContext.AuthenticateL();
               
    /*
    if "error in authentication"
        "clear Crypto Handler"
        go to the uninitialized state
    iStreamContext.ChangeRTPState(MSRTPStreamInContext::ESRTPStreamInUninitialized);    
    */
  	SRTP_DEBUG_DETAIL( "DecryptL" );
                    
     // step 6 decrypt..
    HBufC8* decryptedPacket = iCryptoHandlerRTPContext.DecryptL();
     
    // Step 7, conditional update s_l and ROC values..
    // (in RFC 3711, section 3.3):         
    iCryptoHandlerRTPContext.Update_s_l_and_RocL();
    iCryptoHandlerRTPContext.AddReplayIndex(); 
 

    // first RTP packet successfully decrypted..
    // session keys derived etc..
    // we can go to normal state
    SRTP_DEBUG_DETAIL( "ChangeStreamIn State to Normal" );
           
    
    iStreamContext.ChangeRTPState(MSRTPStreamInContext::ESRTPStreamInNormal);
    
    SRTP_DEBUG_TINT_VALUE( "the decrypted packet Length is", decryptedPacket->Length());            
        
    SRTP_DEBUG_DETAIL( "TSRTPStreamInStateLateBind::DoUnprotectSrtpL EXIT");
   	
    // return decrypted RTP packet
    return decryptedPacket;    
    
    }

// ----------------------------------------------------------------------------
// TSRTPStreamInStateLateBind::DoUnprotectSrtcpL
// ----------------------------------------------------------------------------
//		
HBufC8* TSRTPStreamInStateLateBind::DoUnprotectSrtcpL(const TDesC8& aPacket)
    {
     // first initialize current encrypted RTP packet..
    iCryptoHandlerRTCPContext.InitializeEncryptedPacketL(aPacket);    
    
    
    //(in RFC 3711, section 3.4):
    // count the packet index i
    iCryptoHandlerRTCPContext.ReceiverPacketIndexL();
     
    // check MKI if it is used
    iCryptoHandlerRTCPContext.CheckMasterKeyIdentifierL();
     
    // derive session keys..
    iCryptoHandlerRTCPContext.DeriveSessionKeysL();

    // do the replay protection 
    iCryptoHandlerRTCPContext.ReplayProtectionL();

    // do the authentication (integrity check)
    iCryptoHandlerRTCPContext.AuthenticateL();
               
     // decrypt..
    HBufC8* decryptedPacket = iCryptoHandlerRTCPContext.DecryptL();
     
    iCryptoHandlerRTCPContext.AddReplayIndex(); 
    // first RTP packet successfully decrypted..
    // session keys derived etc..
    // we can go to normal state
    iStreamContext.ChangeRTCPState(MSRTPStreamInContext::ESRTPStreamInNormal);
    
    
    // return decrypted RTP packet
    return decryptedPacket;    
    }
