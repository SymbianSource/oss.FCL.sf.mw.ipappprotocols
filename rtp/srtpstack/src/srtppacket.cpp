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
* Description:    General packet handling routines.
*
*/




// INCLUDES
#include "srtppacket.h"
#include "srtpcryptohandler.h"
#include "srtpcryptocontext.h"
#include "srtputils.h"
#include "srtpmasterkey.h"
#include "srtpmastersalt.h"

// -----------------------------------------------------------------------------
// CSRTPPacket::CSRTPPacket
// -----------------------------------------------------------------------------
//
CSRTPPacket::CSRTPPacket(const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler)
    :   iPacket(aPacket),
        iDataP(aPacket.Ptr()),
        iHandler(aHandler),
        iPayload(TPtrC8()),
        iHeaderLength(0), 
        iPayloadLength(0)
    {
    
    // set the current packet    
    iPacket.Set(aPacket.Ptr(), aPacket.Length());
            
    }

// -----------------------------------------------------------------------------
// CSRTPPacket::~CSRTPPacket
// -----------------------------------------------------------------------------
//
CSRTPPacket::~CSRTPPacket()
            {
            
            }
            
// ---------------------------------------------------------------------------
// CSRTPPacket::Payload()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacket::Payload()
    {
    return iPayload;
    }


// ---------------------------------------------------------------------------
// CSRTPPacket::HeaderLength()
// ---------------------------------------------------------------------------
//
TUint CSRTPPacket::HeaderLength()
    {
    return iHeaderLength;
    }

// ---------------------------------------------------------------------------
// CSRTPPacket::PayloadLength()
// ---------------------------------------------------------------------------
//
TUint CSRTPPacket::PayloadLength()
    {
    return iPayloadLength;
    }


// ---------------------------------------------------------------------------
// CSRTPPacket::HeaderAndPayload()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacket::HeaderAndPayload()
    {
    return iPacket.Mid(0, iHeaderLength + iPayloadLength);    
    }

// ---------------------------------------------------------------------------
// CSRTPPacket::Get_Concatenated_MDataL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPPacket::Get_Concatenated_MDataL(TBool rtcp)
    {                      
    TInt size = iHeaderLength + iPayloadLength;
    if (rtcp)
    	{
    	//RFC 3711 3.4 SRCP authentication portion including index
    	size = size + KSRTCPPacketIndexLength4;
    	}
    HBufC8* result = HBufC8::NewMaxL(size);
    TUint8* ptr = const_cast<TUint8*>(result->Des().Ptr());
    
    Mem::FillZ( ptr, size);        
    Mem::Copy( ptr, iDataP, size );                    
   
    return result ;      
    }

// ---------------------------------------------------------------------------
// CSRTPPacket::ConstructL()
// ---------------------------------------------------------------------------
//
void CSRTPPacket::ConstructL()
    {      
    }


// -----------------------------------------------------------------------------
// void CSRTPPacket::UpdatePayload
// -----------------------------------------------------------------------------
//
void CSRTPPacket::UpdatePayload()
    {            
    TPtrC8 payload = iPacket.Mid(iHeaderLength, iPayloadLength);
    iPayload.Set(payload.Ptr(), iPayloadLength);
    }
        
// -----------------------------------------------------------------------------
// void CSRTPPacket::CopyHeaderAndPayloadL
// -----------------------------------------------------------------------------
//
HBufC8* CSRTPPacket::CopyHeaderAndPayloadL(TUint aPacketSize,
                                       TUint8* aPayloadPtr)
    {    
    HBufC8* packet = HBufC8::NewMaxL(aPacketSize);

    TUint8* packetPtr = const_cast<TUint8*>(packet->Des().Ptr());        
    Mem::FillZ( packetPtr, aPacketSize);        

    // First copy the header
    Mem::Copy( packetPtr, iPacket.Ptr(), iHeaderLength);
    // then copy the encrypted/decrypted payload
    packetPtr += iHeaderLength;
    Mem::Copy( packetPtr, aPayloadPtr, iPayloadLength);
    
    return packet;
    }
    


// VIRTUAL functions

// ---------------------------------------------------------------------------
// CSRTPPacket::CreateDecryptedPacketL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPPacket::CreateDecryptedPacketL(TUint8* /*aDecryptedPayloadPtr*/)
    { 
    User::Leave(KErrTotalLossOfPrecision);
    return  NULL;  
    }
    
// ---------------------------------------------------------------------------
// CSRTPPacket::CreateEncryptedPacketL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPPacket::CreateEncryptedPacketL(TUint8* /*aEncryptedPayloadPtr*/)
    {  
    User::Leave(KErrTotalLossOfPrecision);
    return  NULL;    
    }

// ---------------------------------------------------------------------------
// CSRTPPacket::AuthenticationTag()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacket::AuthenticationTag()
    {  
    return  NULL;      
    }

// ---------------------------------------------------------------------------
// CSRTPPacket::SequenceNumber()
// ---------------------------------------------------------------------------
//
TUint16 CSRTPPacket::SequenceNumber()
    {  
    return  NULL;      
    }

// ---------------------------------------------------------------------------
// CSRTPPacket::SetPacketIndex()
// ---------------------------------------------------------------------------
//
void CSRTPPacket::SetPacketIndex(TUint64 /*aPacketIndex*/)
    {  
    }

// ---------------------------------------------------------------------------
// CSRTPPacket::MasterKeyIdentifier()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacket::MasterKeyIdentifier()
    {  
    return  NULL;      
    }

// ---------------------------------------------------------------------------
// CSRTPPacket::CountSenderPacketIndex()
// ---------------------------------------------------------------------------
//
void CSRTPPacket::CountSenderPacketIndex()
    { 
    }
    
// ---------------------------------------------------------------------------
// CSRTPPacket::PacketIndex()
// ---------------------------------------------------------------------------
//
TUint64 CSRTPPacket::PacketIndex()
    {  
    return  NULL;      
    }
// ---------------------------------------------------------------------------
// CSRTPPacket::GetSenderROC
// ---------------------------------------------------------------------------
//    
TUint32 CSRTPPacket::GetSenderROC()    
	{
	return NULL;
	}
