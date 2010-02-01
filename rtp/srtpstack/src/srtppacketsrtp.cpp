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
* Description:    SRTP-specific packet handling routines.
*
*/




// INCLUDES
#include "srtppacketsrtp.h"
#include "srtpcryptohandler.h"
#include "srtpcryptocontext.h"
#include "srtpmasterkey.h"
#include "srtpmastersalt.h"
#include "srtputils.h"

_LIT8( KMKINULL, "NULL");

// -----------------------------------------------------------------------------
// CSRTPPacketSRTP::CSRTPPacketSRTP
// -----------------------------------------------------------------------------
//
CSRTPPacketSRTP::CSRTPPacketSRTP(const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler)
    : CSRTPPacketRTP(aPacket, aHandler),
        iAuthenticationTag(TPtrC8()),       
        iMasterKeyIdentifier(TPtrC8())
            {
            
            }

// -----------------------------------------------------------------------------
// CSRTPPacketSRTP::~CSRTPPacketSRTP
// -----------------------------------------------------------------------------
//
CSRTPPacketSRTP::~CSRTPPacketSRTP()
            {
            
            }

// ---------------------------------------------------------------------------
// Two-phased constructor. 
// 
// ---------------------------------------------------------------------------
//
CSRTPPacketSRTP* CSRTPPacketSRTP::NewL(const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler)
    {
    CSRTPPacketSRTP* self = new( ELeave )CSRTPPacketSRTP( aPacket, aHandler);
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );        
    return self;
    }

// -----------------------------------------------------------------------------
// void CSRTPPacketSRTP::ConstructL
// -----------------------------------------------------------------------------
//
void CSRTPPacketSRTP::ConstructL()
    {            
    CSRTPPacketRTP::ConstructL();
    UpdatePayloadLengthL();
    SRTP_DEBUG_TINT_VALUE( "SRTP payload length", iPayloadLength );
    
    UpdatePayload();               
    UpdateAuthenticationTagL();    
    UpdateMasterKeyIdentifier();        
    }

// ---------------------------------------------------------------------------
// CSRTPPacketSRTP::AuthenticationTag()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacketSRTP::AuthenticationTag()
    {
    SRTP_DEBUG_DETAIL( "SRTP HMAC authen tag and the tag in the packet" );
    SRTP_DEBUG_PACKET( iAuthenticationTag );    
    		
    return iAuthenticationTag;
    }


// ---------------------------------------------------------------------------
// CSRTPPacketSRTP::MasterKeyIdentifier()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacketSRTP::MasterKeyIdentifier()
    {
    return iMasterKeyIdentifier;
    }

// ---------------------------------------------------------------------------
// CSRTPPacketSRTP::UpdatePayloadLengthL()
// ---------------------------------------------------------------------------
//
void CSRTPPacketSRTP::UpdatePayloadLengthL()
    {    
    SRTP_DEBUG_DETAIL( "CSRTPPacketSRTP::UpdatePayloadLengthL ENTRY" );

     // the real RTP packet length is 
     // the packet size - header - MKI - Auth tag
     // Need implementation of caculating RCC
     // when RCC is not included but the tag length is 14 or 8
     TUint rocLength=KSRTPROCLength4;
     TUint authTagLen=0;
     authTagLen = iHandler.Context().CryptoParams().iSrtpAuthTagLen/8;	
     if (!TagWithROCLengthL() && (
     	iHandler.Context().CryptoParams().iSrtpAuthAlg  == EAuthRCCm2 ||
     	iHandler.Context().CryptoParams().iSrtpAuthAlg  == EAuthRCCm3))
     	{
     	authTagLen -= rocLength;
     	}
     if (!TagWithROCLengthL() && (
     	iHandler.Context().CryptoParams().iSrtpAuthAlg == EAuthRCCm1))
     	{
     	authTagLen=0;
     	}	
     TUint deductedLen = HeaderLength() + iHandler.Context().MasterKey().MKI().Length() + authTagLen;     
     
     iPayloadLength = iPacket.Length() - deductedLen;    
     if ( iPacket.Length() <= HeaderLength() )
     	{
     	iPayloadLength = 0;
     	}	
    SRTP_DEBUG_TINT_VALUE( "SRTP payload length", iPayloadLength);
    SRTP_DEBUG_DETAIL( "CSRTPPacketSRTP::UpdatePayloadLengthL EXIT" );
    }
    
    
// ---------------------------------------------------------------------------
// CSRTPPacketSRTP::CountDecryptedPacketSize()
// ---------------------------------------------------------------------------
//
TUint CSRTPPacketSRTP::CountDecryptedPacketSize()
    {
    return iHeaderLength + iPayloadLength;    
    }    
    
// ---------------------------------------------------------------------------
// CSRTPPacketSRTP::CreateDecryptedPacketL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPPacketSRTP::CreateDecryptedPacketL(TUint8* aDecryptedPayloadPtr)
    {
    // create decrypted RTP packet
    // first count needed packet size
    TUint rtpPacketSize = CountDecryptedPacketSize();
    
    return CopyHeaderAndPayloadL(rtpPacketSize, aDecryptedPayloadPtr);
    
    }

// ---------------------------------------------------------------------------
// CSRTPPacketSRTP::UpdateAuthenticationTag()
// ---------------------------------------------------------------------------
//
void CSRTPPacketSRTP::UpdateAuthenticationTagL()
    {       
    //Here means MAC part
    TPtrC8 authTag = GetAuthenticationTagL();  
    if (authTag.Compare(KNullDesC8))
       	{     
    	iAuthenticationTag.Set(authTag.Ptr(), authTag.Length());
       	}
    }    

// ---------------------------------------------------------------------------
// CSRTPPacketSRTP::GetAuthenticationTag()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacketSRTP::GetAuthenticationTagL()
    {       
    if ((iHandler.Context().CryptoParams().iSrtpAuthTagLen > 0) && 
        iHandler.Context().CryptoParams().iSrtpAuthAlg == EAuthHMAC_SHA1)
        {
         TUint authTagLength = iHandler.Context().CryptoParams().iSrtpAuthTagLen;
         TUint authTagLenInBytes = authTagLength/8;
         TUint packetLength = iPacket.Length();
         TUint position = packetLength - authTagLenInBytes;
         
         return iPacket.Mid(position,authTagLenInBytes);
        }
     if ((iHandler.Context().CryptoParams().iSrtpAuthTagLen > 0) &&
     	((iHandler.Context().CryptoParams().iSrtpAuthAlg == EAuthRCCm1 && 
     		TagWithROCLengthL()) ||
        iHandler.Context().CryptoParams().iSrtpAuthAlg == EAuthRCCm2))
     	{
     	TUint authTagLength = iHandler.Context().CryptoParams().iSrtpAuthTagLen;
     	TUint authTagLenInBytes = authTagLength/8;//14
     	//Here we only need MAC part
     	authTagLenInBytes-= 4;
     	TUint packetLength = iPacket.Length();
        TUint position = packetLength - authTagLenInBytes;
         
        return iPacket.Mid(position,authTagLenInBytes);
     	}  
    // in case of EAuthRCCm3 and EAuthNULL  	 
    TPtrC8 authTagNull(KNullDesC8);
    return  authTagNull;    
    } 

// ---------------------------------------------------------------------------
// CSRTPPacketSRTP::GetMasterKeyIdentifier()
// ---------------------------------------------------------------------------
//
TPtrC8 CSRTPPacketSRTP::GetMasterKeyIdentifier()
    {
    if(iHandler.Context().MasterKey().MKI()!= KNullDesC8)
    	{    
	    TInt mkiLength = iHandler.Context().MasterKey().MKI().Length();
	    if (iHandler.Context().MasterKey().MKI().Length() > 0)
	        {
	         TUint authTagLength = iHandler.Context().CryptoParams().iSrtpAuthTagLen;
	         TUint authTagLenInBytes = authTagLength/8;
	         TUint position = iPacket.Length() - authTagLenInBytes - mkiLength;
	                                
	         return iPacket.Mid(position,mkiLength);
	        }
    	}
    TPtrC8 mkiNull( KMKINULL );
    return  mkiNull; 
    }    
           
// ---------------------------------------------------------------------------
// CSRTPPacketSRTP::UpdateMasterKeyIdentifierL()
// ---------------------------------------------------------------------------
//
void CSRTPPacketSRTP::UpdateMasterKeyIdentifier()
    {    
     TPtrC8 mki = GetMasterKeyIdentifier();
     if (mki.Compare( KMKINULL ))
     	{
     	iMasterKeyIdentifier.Set(mki.Ptr(),mki.Length());	
     	}
    }    
    
