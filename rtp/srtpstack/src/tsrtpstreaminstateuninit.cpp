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



#include "tsrtpstreaminstateuninit.h"
#include "msrtpstreamincontext.h"
#include "msrtpcryptohandlercontext.h"
#include "msrtpcryptohandlercontextrtp.h"
#include "msrtpcryptohandlercontextrtcp.h"
#include "srtputils.h"

// ----------------------------------------------------------------------------
// TSRTPStreamInStateUninit::TSRTPStreamInStateUninit
// ----------------------------------------------------------------------------
//
TSRTPStreamInStateUninit::TSRTPStreamInStateUninit(
    MSRTPStreamInContext& aStreamContext,
    MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
    MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext)
    : TSRTPStreamInStateBase(aStreamContext,
                            aCryptoHandlerRTPContext, 
                            aCryptoHandlerRTCPContext)
    {
    }

// ----------------------------------------------------------------------------
// TSRTPStreamInStateUninit::DoUnprotectSrtpL
// ----------------------------------------------------------------------------
//		
HBufC8* TSRTPStreamInStateUninit::DoUnprotectSrtpL(const TDesC8& aPacket)
    {    
    SRTP_DEBUG_DETAIL( "TSRTPStreamInStateUninit::DoUnprotectSrtpL entry" );
    // first initialize current encrypted RTP packet..
    iCryptoHandlerRTPContext.InitializeEncryptedPacketL(aPacket);    
   
    SRTP_DEBUG_TINT_VALUE("Encrypted PacketLength is", aPacket.Length() );                
             
    // for the first RTP packet, we need to set 
    // receiver s_l value to current sequence number, see RFC
    iCryptoHandlerRTPContext.SetReceiver_s_l();
       
    // Step 2 (in RFC 3711, section 3.3):
    // count the packet index i
    iCryptoHandlerRTPContext.CountReceiverPacketIndexL();
     
    // check MKI if it is used
    iCryptoHandlerRTPContext.CheckMasterKeyIdentifierL();
    SRTP_DEBUG_DETAIL( "DeriveSessionKeysL" );
               
    // derive session keys..
    iCryptoHandlerRTPContext.DeriveSessionKeysL();

    // do the replay protection 
    iCryptoHandlerRTPContext.ReplayProtectionL();	
    
	SRTP_DEBUG_DETAIL("DeriveSessionKeysL" );
    
    // do the authentication (integrity check)
    iCryptoHandlerRTPContext.AuthenticateL();
   	SRTP_DEBUG_DETAIL("Authenticated");
               
     // decrypt..
    HBufC8* decryptedPacket = iCryptoHandlerRTPContext.DecryptL();
   	SRTP_DEBUG_DETAIL( "StreamInStateUninit,Decrypted" );
     
    // Step 6, conditional update s_l and ROC values..
    // (in RFC 3711, section 3.3):         
    iCryptoHandlerRTPContext.Update_s_l_and_RocL();
    iCryptoHandlerRTPContext.AddReplayIndex(); 
    // first RTP packet successfully decrypted..
    // session keys derived etc..
    // we can go to normal state
    iStreamContext.ChangeRTPState(MSRTPStreamInContext::ESRTPStreamInNormal);
    
    
    SRTP_DEBUG_DETAIL( "StreamInStateUninit, UnprotectRTP Out" );
    
    // return decrypted RTP packet
    SRTP_DEBUG_DETAIL( "TSRTPStreamInStateUninit::DoUnprotectSrtpL exit" );
    
    return decryptedPacket;    
    }

// ----------------------------------------------------------------------------
// TSRTPStreamInStateUninit::DoUnprotectSrtcpL
// ----------------------------------------------------------------------------
//		
HBufC8* TSRTPStreamInStateUninit::DoUnprotectSrtcpL(const TDesC8& aPacket)
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
