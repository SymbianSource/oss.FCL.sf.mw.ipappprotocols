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




#include "tsrtpstreaminstatenormal.h"
#include "msrtpstreamincontext.h"
#include "msrtpcryptohandlercontext.h"
#include "msrtpcryptohandlercontextrtp.h"
#include "msrtpcryptohandlercontextrtcp.h"
#include "srtputils.h"

// ----------------------------------------------------------------------------
// TSRTPStreamInStateNormal::TSRTPStreamInStateNormal
// ----------------------------------------------------------------------------
//
TSRTPStreamInStateNormal::TSRTPStreamInStateNormal(
    MSRTPStreamInContext& aStreamContext,
    MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
    MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext)
    : TSRTPStreamInStateBase(aStreamContext,
                            aCryptoHandlerRTPContext, 
                            aCryptoHandlerRTCPContext)
    {
    }

// ----------------------------------------------------------------------------
// TSRTPStreamInStateNormal::DoUnprotectSrtpL
// ----------------------------------------------------------------------------
//		
HBufC8* TSRTPStreamInStateNormal::DoUnprotectSrtpL(const TDesC8& aPacket)
    {
	SRTP_DEBUG_DETAIL( "TSRTPStreamInStateNormal::DoUnprotectSrtpL ENTRY" );
   	SRTP_DEBUG_TINT_VALUE( "Encrypted PacketLength is", aPacket.Length() );            
          
    // first initialize current encrypted RTP packet..
    iCryptoHandlerRTPContext.InitializeEncryptedPacketL(aPacket);    
           
    // Step 2 (in RFC 3711, section 3.3):
    // count the packet index i
    iCryptoHandlerRTPContext.CountReceiverPacketIndexL();
     
    // check MKI if it is used
    iCryptoHandlerRTPContext.CheckMasterKeyIdentifierL();
     
    // derive session keys..
    if (iCryptoHandlerRTPContext.MasterKeysUpdated())
        {
        iCryptoHandlerRTPContext.DeriveSessionKeysL();        
        }

    // do the replay protection 
    iCryptoHandlerRTPContext.ReplayProtectionL();

    // do the authentication (integrity check)
    iCryptoHandlerRTPContext.AuthenticateL();
               
     // decrypt..
    HBufC8* decryptedPacket = iCryptoHandlerRTPContext.DecryptL();
     
    // Step 7, conditional update s_l and ROC values..
    // (in RFC 3711, section 3.3):         
    iCryptoHandlerRTPContext.Update_s_l_and_RocL();
    iCryptoHandlerRTPContext.AddReplayIndex();
    ///Should check the next Packet Index and decide if need to be re-key
    
    SRTP_DEBUG_TINT_VALUE( "the decrypted packet Length is", decryptedPacket->Length());
    SRTP_DEBUG_DETAIL( "TSRTPStreamInStateNormal::DoUnprotectSrtpL EXIT" );
	
    return decryptedPacket;    
    
    }

// ----------------------------------------------------------------------------
// TSRTPStreamInStateNormal::DoUnprotectSrtcpL
// ----------------------------------------------------------------------------
//		
HBufC8* TSRTPStreamInStateNormal::DoUnprotectSrtcpL(const TDesC8& aPacket)
    {
    
    // first initialize current encrypted RTP packet..
    iCryptoHandlerRTCPContext.InitializeEncryptedPacketL(aPacket);    
           
  //(in RFC 3711, section 3.4):
    // count the packet index i
    iCryptoHandlerRTCPContext.ReceiverPacketIndexL();
     
    // check MKI if it is used
    iCryptoHandlerRTCPContext.CheckMasterKeyIdentifierL();
     
    // derive session keys..
    if (iCryptoHandlerRTCPContext.MasterKeysUpdated())
        {
        iCryptoHandlerRTCPContext.DeriveSessionKeysL();        
        }

    // do the replay protection 
    iCryptoHandlerRTCPContext.ReplayProtectionL();

    // do the authentication (integrity check)
    iCryptoHandlerRTCPContext.AuthenticateL();
               
     // decrypt..
    HBufC8* decryptedPacket = iCryptoHandlerRTCPContext.DecryptL();
    iCryptoHandlerRTCPContext.AddReplayIndex();  
    ///Should check the next Packet Index and decide if need to be re-key
    //Set updateKey needed and then lateron compared the key if the key is the same then ignore packet
    //implemented later
    // return decrypted RTCP packet
    return decryptedPacket;    
    
    }
