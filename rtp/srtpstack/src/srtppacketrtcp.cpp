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
* Description:    General RTCP packet handling routines.
*
*/




// INCLUDES
#include "srtppacketrtcp.h"
#include "srtputils.h"
#include "srtpcryptohandler.h"
#include "srtpcryptocontext.h"
#include "srtpmasterkey.h"
#include "srtpmastersalt.h"

// -----------------------------------------------------------------------------
// CSRTPPacketRTCP::CSRTPPacketRTCP
// -----------------------------------------------------------------------------
//
CSRTPPacketRTCP::CSRTPPacketRTCP(const TDesC8& aPacket,
                   CSRTPCryptoHandler& aHandler)
    : CSRTPPacket(aPacket, aHandler)
            {
            
            }

// -----------------------------------------------------------------------------
// CSRTPPacketRTCP::~CSRTPPacketRTCP
// -----------------------------------------------------------------------------
//
CSRTPPacketRTCP::~CSRTPPacketRTCP()
            {
            
            }

// ---------------------------------------------------------------------------
// Two-phased constructor. 
// 
// ---------------------------------------------------------------------------
//
CSRTPPacketRTCP* CSRTPPacketRTCP::NewL(const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler)
    {
    CSRTPPacketRTCP* self = new( ELeave )CSRTPPacketRTCP( aPacket, aHandler);
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );        
    return self;
    }

// -----------------------------------------------------------------------------
// void CSRTPPacketRTCP::ConstructL
// -----------------------------------------------------------------------------
//
void CSRTPPacketRTCP::ConstructL()
    {            
    CSRTPPacket::ConstructL(); 
    UpdateHeaderLengthL(); 
    UpdatePayloadLength();
    UpdatePayload();           
    }

// ---------------------------------------------------------------------------
// CSRTPPacketRTCP::UpdatePayloadLength()
// ---------------------------------------------------------------------------
//
void CSRTPPacketRTCP::UpdatePayloadLength()
    {    
     // In RTP case the payload length is easy to calculate
     iPayloadLength = iPacket.Length() - iHeaderLength;     
     if ( iPacket.Length() <= iHeaderLength )
     	{
     	iPayloadLength = 0;
     	}
    }

// ---------------------------------------------------------------------------
// CSRTPPacketRTCP::UpdateHeaderLengthL()
// ---------------------------------------------------------------------------
//
void CSRTPPacketRTCP::UpdateHeaderLengthL()
    {    
    TInt size = iPacket.Length();
    
	const TUint8 *pointer = iDataP;	

     // version
    if ( ( ( pointer[0] & 0xC0 ) >> 6 ) != 2/*RTPVersion*/ )
        {
       	User::Leave(KErrCorrupt);
        }

    // padding
    if ( pointer[0] & 0x20 )
        {
    	size -= iDataP[size - 1];
    	}

    // determine RTCP packet type
    TInt pt = static_cast<TInt>( pointer[1] );

    if ( ( pt!= 200 ) && (pt != 201) &&
	     ( pt != 202) && ( pt != 203 ) &&
	     ( pt != 204 ) )
        {
        User::Leave(KErrCorrupt);
        }

    // source/report Count
    //TInt sourceCount = ( pointer[0] & 0x1F );

    pointer += 2;
    
    TInt length =static_cast<TInt>( TSRTPUtils::Read16( pointer ) );

    if ( length * 4 > size )
        {
        User::Leave(KErrCorrupt);
        }
    pointer += 2;

    // SSRC of sender in case of RR
    if ( pt == 201 || pt==200 )
        {
	    TUint32 SSRC = TSRTPUtils::Read32( pointer );
	    pointer += 4;
        }
                        
    

    //RFC 3711 3.4 - RR & SR
    iHeaderLength =pointer-iDataP;
    }
// ---------------------------------------------------------------------------
// CSRTPPacketRTCP::CountEncryptedPacketSizeL()
// ---------------------------------------------------------------------------
//
TUint CSRTPPacketRTCP::CountEncryptedPacketSizeL()
    {
   	TUint srtpPacketSize = iHeaderLength + iPayloadLength + KSRTCPPacketIndexLength4;
   	srtpPacketSize += iHandler.Context().MasterKey().MKI().Length();
    if ( iHandler.Context().CryptoParams().iSrtcpAuthAlg == EAuthHMAC_SHA1)
        {
        srtpPacketSize += iHandler.Context().CryptoParams().iSrtcpAuthTagLen/8;
        }
    
    return srtpPacketSize;
    }
        
// ---------------------------------------------------------------------------
// CSRTPPacketRTCP::CreateEncryptedPacketL()
// ---------------------------------------------------------------------------
//        
HBufC8* CSRTPPacketRTCP::CreateEncryptedPacketL(TUint8* aEncryptedPayloadPtr)
    {
     TUint srtpPacketSize = CountEncryptedPacketSizeL();
    
    return CopyHeaderAndPayloadL(srtpPacketSize, aEncryptedPayloadPtr);    
    }    


// ---------------------------------------------------------------------------
// CSRTPPacketRTCP::SetPacketIndex()
// ---------------------------------------------------------------------------
//
void CSRTPPacketRTCP::SetPacketIndex(TUint64 aPacketIndex)
    {
    iPacketIndex = aPacketIndex;
    }
// ---------------------------------------------------------------------------
// CSRTPPacketRTCP::PacketIndex()
// ---------------------------------------------------------------------------
//
TUint64 CSRTPPacketRTCP::PacketIndex()
    {
    return iPacketIndex;
    }

 // ---------------------------------------------------------------------------
// CSRTPPacketRTCP::CreateDecryptedPacketL()
// ---------------------------------------------------------------------------
//   
HBufC8* CSRTPPacketRTCP::CreateDecryptedPacketL(TUint8* /*aDecryptedPayloadPtr*/)
    { 
    User::Leave(KErrTotalLossOfPrecision);
    return  NULL;           
    }
// ---------------------------------------------------------------------------
// CSRTPPacketRTCP::AuthenticationTag()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacketRTCP::AuthenticationTag()
    {
    return  NULL;            
    }
// ---------------------------------------------------------------------------
// CSRTPPacketRTCP::MasterKeyIdentifier()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacketRTCP::MasterKeyIdentifier()
    {
    return  NULL;            
    }

