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



#include "tsrtpstreamoutstateuninit.h"
#include "msrtpstreamoutcontext.h"
#include "msrtpcryptohandlercontext.h"
#include "msrtpcryptohandlercontextrtp.h"
#include "msrtpcryptohandlercontextrtcp.h"
#include "srtputils.h"

// ----------------------------------------------------------------------------
// TSRTPStreamOutStateUninit::TSRTPStreamOutStateUninit
// ----------------------------------------------------------------------------
//
TSRTPStreamOutStateUninit::TSRTPStreamOutStateUninit(
    MSRTPStreamOutContext& aStreamContext,
    MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
    MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext)
    : TSRTPStreamOutStateBase(aStreamContext,
                            aCryptoHandlerRTPContext, 
                            aCryptoHandlerRTCPContext)
    {
    }

// ----------------------------------------------------------------------------
// TSRTPStreamOutStateUninit::DoProtectRtpL
// ----------------------------------------------------------------------------
//		
HBufC8* TSRTPStreamOutStateUninit::DoProtectRtpL(const TDesC8& aPacket)
    {
    SRTP_DEBUG_DETAIL( "TSRTPStreamOutStateUninit::DoProtectRtpL entry" );
    SRTP_DEBUG_TINT_VALUE( "PacketLength is", aPacket.Length());
                  
    // first initialize current RTP packet..
    iCryptoHandlerRTPContext.InitializePlainPacketL(aPacket);    
       
    // Step 2 (in RFC 3711, section 3.3):
    // count the packet index i
     iCryptoHandlerRTPContext.CountSenderPacketIndexL();
    
     SRTP_DEBUG_DETAIL( "StreamOutStateUninit, Protect, DeriveSessionKeysL" );
    
     // derive session keys..
     iCryptoHandlerRTPContext.DeriveSessionKeysL();
     SRTP_DEBUG_DETAIL( "StreamOutStateUninit, Protect, Encrypt" );
     // encrypt..
     HBufC8* protectedPacket = iCryptoHandlerRTPContext.EncryptL();
     
     // finally, conditional update of roll-over counter..
     iCryptoHandlerRTPContext.UpdateROC();
     
         
    // first RTP packet successfully encrypted..
    // session keys derived etc..
    // we can go to normal state
    SRTP_DEBUG_DETAIL( "ESRTPStreamOutNormal" );
    
    iStreamContext.ChangeRTPState(MSRTPStreamOutContext::ESRTPStreamOutNormal);
    
    
    SRTP_DEBUG_TINT_VALUE("length of protectPacket", protectedPacket->Length() );
    SRTP_DEBUG_DETAIL( "TSRTPStreamOutStateUninit::DoProtectRtpL exit" );
    
    // return encrypted RTP packet
    return protectedPacket;    
    }

// ----------------------------------------------------------------------------
// TSRTPStreamOutStateUninit::DoProtectRtcpL
// ----------------------------------------------------------------------------
//		
HBufC8* TSRTPStreamOutStateUninit::DoProtectRtcpL(const TDesC8& aPacket)
    {
     // first initialize current RTP packet..
    iCryptoHandlerRTCPContext.InitializePlainPacketL(aPacket);    
       
    // Step 2 (in RFC 3711, section 3.4):
    //Srtcp packet index must be set to zero before the first srtcp packet is sent
    iCryptoHandlerRTCPContext.InitialPacketIndex();
	
     // derive session keys..
     iCryptoHandlerRTCPContext.DeriveSessionKeysL();
               
     // encrypt..
     HBufC8* protectedPacket = iCryptoHandlerRTCPContext.EncryptL();
     
        
    // first RTP packet successfully encrypted..
    // session keys derived etc..
    // we can go to normal state
    iStreamContext.ChangeRTCPState(MSRTPStreamOutContext::ESRTPStreamOutNormal);
    
    SRTP_DEBUG_TINT_VALUE("length of protect RTCP Packet", protectedPacket->Length() );
     
    // return encrypted RTP packet
    return protectedPacket;    
    }

