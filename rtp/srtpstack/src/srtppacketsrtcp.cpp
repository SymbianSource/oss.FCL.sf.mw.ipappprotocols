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
* Description:    SRTCP-specific packet handling routines.
*
*/




// INCLUDES
#include "srtppacketsrtcp.h"
#include "srtpcryptohandler.h"
#include "srtpcryptocontext.h"
#include "srtputils.h"
#include "srtpmasterkey.h"
#include "srtpmastersalt.h"

_LIT8( KMKINULL, "NULL");
// -----------------------------------------------------------------------------
// CSRTPPacketSRTCP::CSRTPPacketSRTCP
// -----------------------------------------------------------------------------
//
CSRTPPacketSRTCP::CSRTPPacketSRTCP(const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler)
    : CSRTPPacketRTCP(aPacket, aHandler),
        iAuthenticationTag(TPtrC8()),       
        iMasterKeyIdentifier(TPtrC8())
            {
            
            }

// -----------------------------------------------------------------------------
// CSRTPPacketSRTCP::~CSRTPPacketSRTCP
// -----------------------------------------------------------------------------
//
CSRTPPacketSRTCP::~CSRTPPacketSRTCP()
            {
            
            }

// ---------------------------------------------------------------------------
// Two-phased constructor. 
// 
// ---------------------------------------------------------------------------
//
CSRTPPacketSRTCP* CSRTPPacketSRTCP::NewL(const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler)
    {
    CSRTPPacketSRTCP* self = new( ELeave )CSRTPPacketSRTCP( aPacket, aHandler);
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );        
    return self;
    }

// -----------------------------------------------------------------------------
// void CSRTPPacketSRTCP::ConstructL
// -----------------------------------------------------------------------------
//
void CSRTPPacketSRTCP::ConstructL()
    {            
    CSRTPPacketRTCP::ConstructL();
    UpdatePayloadLength();
    UpdatePayload();               
    UpdateAuthenticationTag();    
    UpdateMasterKeyIdentifier();        
    UpdatePacketIndex();     
    }

// ---------------------------------------------------------------------------
// CSRTPPacketSRTCP::AuthenticationTag()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacketSRTCP::AuthenticationTag()
    {
    return iAuthenticationTag;
    }


// ---------------------------------------------------------------------------
// CSRTPPacketSRTCP::MasterKeyIdentifier()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacketSRTCP::MasterKeyIdentifier()
    {
    return iMasterKeyIdentifier;
    }

// ---------------------------------------------------------------------------
// CSRTPPacketSRTCP::UpdatePayloadLength()
// ---------------------------------------------------------------------------
//
void CSRTPPacketSRTCP::UpdatePayloadLength()
    {    
     // the real RTCP packet length is 
     // the packet size - header - MKI - Auth tag- packetIndex
     TUint authTagLen = iHandler.Context().CryptoParams().iSrtcpAuthTagLen/8;
     TUint mkiLen=0;
     mkiLen=  iHandler.Context().MasterKey().MKI().Length();  
     TUint deductedLength = iHeaderLength + mkiLen + authTagLen + KSRTCPPacketIndexLength4; 
     iPayloadLength = iPacket.Length() - deductedLength;  
     
     if ( iPacket.Length() <= iHeaderLength )
     	{
     	iPayloadLength = 0;
     	}
    // iPayloadLength should be bigger than 0 
    }
    
    
// ---------------------------------------------------------------------------
// CSRTPPacketSRTCP::CountDecryptedPacketSize()
// ---------------------------------------------------------------------------
//
TUint CSRTPPacketSRTCP::CountDecryptedPacketSize()
    {
    return iHeaderLength + iPayloadLength;    
    }    
    
// ---------------------------------------------------------------------------
// CSRTPPacketSRTCP::CreateDecryptedPacketL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPPacketSRTCP::CreateDecryptedPacketL(TUint8* aDecryptedPayloadPtr)
    {
    // create decrypted RTCP packet
    // first count needed packet size
    TUint rtpPacketSize = CountDecryptedPacketSize();
    
    return CopyHeaderAndPayloadL(rtpPacketSize, aDecryptedPayloadPtr);
    
    }

// ---------------------------------------------------------------------------
// CSRTPPacketSRTCP::UpdateAuthenticationTag()
// ---------------------------------------------------------------------------
//
void CSRTPPacketSRTCP::UpdateAuthenticationTag()
    {       
       TPtrC8 authTag = GetAuthenticationTag();    
       if (authTag.Compare(KNullDesC8))
       	{
       	iAuthenticationTag.Set(authTag.Ptr(), authTag.Length());	
       	}
       
    }    
// ---------------------------------------------------------------------------
// CSRTPPacketSRTCP::GetAuthenticationTagL()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacketSRTCP::GetAuthenticationTag()
    { 
    // for RTCP, authTagLength existed only with HMAC_SHA1     
    if ((iHandler.Context().CryptoParams().iSrtcpAuthTagLen > 0) && 
        (iHandler.Context().CryptoParams().iSrtcpAuthAlg != EAuthNull))
        {
         TUint authTagLength = iHandler.Context().CryptoParams().iSrtcpAuthTagLen;
         
         TUint authTagLenInBytes = authTagLength/8;
         
         if (iHandler.Context().CryptoParams().iSrtcpAuthAlg != EAuthHMAC_SHA1)
         	{
         	authTagLenInBytes = KSRTPAuthTagLength80/8;
         	}
         		
         TUint packetLength = iPacket.Length();
         TUint position = packetLength - authTagLenInBytes;
         
         return iPacket.Mid(position,authTagLenInBytes);
        }
    //In case of the other Authentication mode
    TPtrC8 authTagNull(KNullDesC8);
    return  authTagNull;    
    }

// ---------------------------------------------------------------------------
// CSRTPPacketSRTCP::GetMasterKeyIdentifier()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacketSRTCP::GetMasterKeyIdentifier()
    {
    if(iHandler.Context().MasterKey().MKI()!= KNullDesC8)
    	{    
	    TInt mkiLength = iHandler.Context().MasterKey().MKI().Length();
	    if (iHandler.Context().MasterKey().MKI().Length() > 0)
	        {
	         TUint authTagLength = iHandler.Context().CryptoParams().iSrtcpAuthTagLen;
	         TUint authTagLenInBytes = authTagLength/8;
	         TUint position = iPacket.Length() - authTagLenInBytes - mkiLength;
	                                
	         return iPacket.Mid(position,mkiLength);
	        }
    	}
    TPtrC8 mkiNull( KMKINULL );
    return  mkiNull; 
    }    
            
// ---------------------------------------------------------------------------
// CSRTPPacketSRTCP::UpdateMasterKeyIdentifier()
// ---------------------------------------------------------------------------
//
void CSRTPPacketSRTCP::UpdateMasterKeyIdentifier()
    {    
     TPtrC8 mki = GetMasterKeyIdentifier();
     if (mki.Compare( KMKINULL ))
     	{
     	iMasterKeyIdentifier.Set(mki.Ptr(),mki.Length());	
     	}
    }    
    
// ---------------------------------------------------------------------------
// CSRTPPacketRTP::UpdatePacketIndex()
// ---------------------------------------------------------------------------
//
void CSRTPPacketSRTCP::UpdatePacketIndex()
    {
    const TUint8 *pointer = iDataP;	
	pointer+=iHeaderLength;
	pointer+= iPayloadLength;//payload length as SRTCP report length
    
     
    iPacketIndex=static_cast<TUint32>(TSRTPUtils::Read32(pointer));
    }
