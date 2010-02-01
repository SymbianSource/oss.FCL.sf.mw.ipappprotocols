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




#include "tsrtpstreamoutstatenormal.h"
#include "msrtpstreamoutcontext.h"
#include "msrtpcryptohandlercontext.h"
#include "msrtpcryptohandlercontextrtp.h"
#include "msrtpcryptohandlercontextrtcp.h"
#include "srtputils.h"

// ----------------------------------------------------------------------------
// TSRTPStreamOutStateNormal::TSRTPStreamOutStateNormal
// ----------------------------------------------------------------------------
//
TSRTPStreamOutStateNormal::TSRTPStreamOutStateNormal(
    MSRTPStreamOutContext& aStreamContext,
    MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
    MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext)
    : TSRTPStreamOutStateBase(aStreamContext,
                            aCryptoHandlerRTPContext, 
                            aCryptoHandlerRTCPContext)
    {
    }

// ----------------------------------------------------------------------------
// TSRTPStreamOutStateNormal::DoProtectRtpL
// ----------------------------------------------------------------------------
//		
HBufC8* TSRTPStreamOutStateNormal::DoProtectRtpL(const TDesC8& aPacket)
    {
    // first initialize current RTP packet..
    SRTP_DEBUG_DETAIL("TSRTPStreamOutStateNormal::DoProtectRtpL Entry");
    
    SRTP_DEBUG_TINT_VALUE( "and packet before encrypted length is", aPacket.Length() );
                   
    iCryptoHandlerRTPContext.InitializePlainPacketL(aPacket);    
    
    // Step 2 (in RFC 3711, section 3.3):
    // count the packet index i
    iCryptoHandlerRTPContext.CountSenderPacketIndexL();
     
     // derive session keys..
    if (iCryptoHandlerRTPContext.MasterKeysUpdated())
       {
       iCryptoHandlerRTPContext.DeriveSessionKeysL();        
       }
               
     // encrypt..
	SRTP_DEBUG_DETAIL( "EncrypL" );
     
    HBufC8* protectedPacket = iCryptoHandlerRTPContext.EncryptL();
     
     // finally, conditional update of roll-over counter..
     //Should check the next Packet Index and decide if need to be re-key
    iCryptoHandlerRTPContext.UpdateROC();
                  
    

	SRTP_DEBUG_DETAIL( "TSRTPStreamOutStateNormal::DoProtectRtpL Exit");
    
    // return encrypted RTP packet
    return protectedPacket;    
    
    }

// ----------------------------------------------------------------------------
// TSRTPStreamOutStateNormal::DoProtectRtcpL
// ----------------------------------------------------------------------------
//		
HBufC8* TSRTPStreamOutStateNormal::DoProtectRtcpL(const TDesC8& aPacket)
    {
     // first initialize current RTP packet..
    iCryptoHandlerRTCPContext.InitializePlainPacketL(aPacket);    
    
    // Step 2 (in RFC 3711, section 3.3):
    // count the packet index i
     iCryptoHandlerRTCPContext.CountSenderPacketIndexL();
     
     // derive session keys..
     if (iCryptoHandlerRTCPContext.MasterKeysUpdated())
        {
        iCryptoHandlerRTCPContext.DeriveSessionKeysL();        
        }
               
     // encrypt..
     HBufC8* protectedPacket = iCryptoHandlerRTCPContext.EncryptL();
     
    //Should check the next Packet Index and decide if need to be re-key
    // return encrypted RTP packet
    return protectedPacket;    
    }
