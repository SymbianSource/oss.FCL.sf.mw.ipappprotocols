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
#include "srtppacketrtp.h"
#include "srtputils.h"
#include "srtpcryptohandler.h"
#include "srtpcryptocontext.h"
#include "srtpmasterkey.h"
#include "srtpmastersalt.h"

// -----------------------------------------------------------------------------
// CSRTPPacketRTP::CSRTPPacketRTP
// -----------------------------------------------------------------------------
//
CSRTPPacketRTP::CSRTPPacketRTP(const TDesC8& aPacket,
                   CSRTPCryptoHandler& aHandler)
    : CSRTPPacket(aPacket, aHandler),
        iSequenceNumber(0),
        iPacketIndex(0)
            {
            
            }

// -----------------------------------------------------------------------------
// CSRTPPacketRTP::~CSRTPPacketRTP
// -----------------------------------------------------------------------------
//
CSRTPPacketRTP::~CSRTPPacketRTP()
            {
            
            }

// ---------------------------------------------------------------------------
// Two-phased constructor. 
// 
// ---------------------------------------------------------------------------
//
CSRTPPacketRTP* CSRTPPacketRTP::NewL(const TDesC8& aPacket,
                      CSRTPCryptoHandler& aHandler)
    {
    CSRTPPacketRTP* self = new( ELeave )CSRTPPacketRTP( aPacket, aHandler);
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );        
    return self;
    }

// -----------------------------------------------------------------------------
// void CSRTPPacketRTP::ConstructL
// -----------------------------------------------------------------------------
//
void CSRTPPacketRTP::ConstructL()
    {            
    CSRTPPacket::ConstructL(); 
    UpdateHeaderLengthL();
    
    SRTP_DEBUG_TINT_VALUE( "RTP header length", iHeaderLength );
        
    UpdateSequenceNumber();        
    UpdatePayloadLength();
    
    SRTP_DEBUG_TINT_VALUE( "RTP payload length", iPayloadLength );
     
    UpdatePayload();           
    
    }
// ---------------------------------------------------------------------------
// CSRTPPacketRTP::UpdateHeaderLengthL()
// ---------------------------------------------------------------------------
//
void CSRTPPacketRTP::UpdateHeaderLengthL()
    {    
    //TUint payloadOffset = KMinSizeRtpHeader;
    TInt size = iPacket.Length();
    if (size < KMinSizeRtpHeader)
    	{
    	User::Leave(KErrCorrupt);
    	}
	const TUint8 *pointer = iDataP;	

    //CC
    TUint8 numCSRC = static_cast<TUint8>( iDataP[0] & 0x0F );
    
    //header extension
    TUint8 headerExt = static_cast<TUint8>( ( iDataP[0] & 0x10 ) >> 4 );    
    
    //go to the end of SSRC
    pointer +=KMinSizeRtpHeader;
    
    if (numCSRC)
        {
        pointer += numCSRC * 4;  
        }
    
    if (headerExt)
        {        
        // Make sure there is a header extension to read
        if ( ( size - ( pointer - iDataP ) ) < 4 )
            {
            User::Leave(KErrCorrupt);
            }

        // bypass header extension type
        pointer += 2;

        // header extension length in number of 32-bit words
        TUint16 extLength = TSRTPUtils::Read16( pointer );
        pointer += 2;

		// jump the extension length
        pointer += extLength * 4;

        // Make sure the extension length is valid
        if ( (size - ( pointer - iDataP ) ) < (extLength * 4) )
            {
            User::Leave(KErrCorrupt);            
            }         
        }
    
    iHeaderLength = pointer - iDataP;
    if ( iHeaderLength > size )
    	{
    	User::Leave( KErrCorrupt );
    	}
    }

// ---------------------------------------------------------------------------
// CSRTPPacketRTP::UpdatePayloadLength()
// ---------------------------------------------------------------------------
//
void CSRTPPacketRTP::UpdatePayloadLength()
    {    
     // In RTP case the payload length is easy to calculate
     //This is only for sending the packe while payload is 512bits 
    TInt packetlen= iPacket.Length() - iHeaderLength;
   	iPayloadLength = packetlen;
   	if ( iPacket.Length() <= iHeaderLength )
     	{
     	iPayloadLength = 0;
     	}
    }

// ---------------------------------------------------------------------------
// CSRTPPacketRTP::UpdateSeqNumber()
// ---------------------------------------------------------------------------
//
void CSRTPPacketRTP::UpdateSequenceNumber( )
    {
//    search the Seq number and update iCurrentSeq here
	const TUint8 *pointer = iDataP;	
	pointer+=2;
    
    iSequenceNumber = static_cast<TUint16>( TSRTPUtils::Read16( pointer ) );
    }


// ---------------------------------------------------------------------------
// CSRTPPacketRTP::CountEncryptedPacketSizeL()
// ---------------------------------------------------------------------------
//
TUint CSRTPPacketRTP::CountEncryptedPacketSizeL()
    {
    TUint srtpPacketSize = iHeaderLength + iPayloadLength;
    	srtpPacketSize += iHandler.Context().MasterKey().MKI().Length();  
	//in Normal case
    if ( iHandler.Context().CryptoParams().iSrtpAuthAlg != EAuthNull )
        {
        srtpPacketSize += iHandler.Context().CryptoParams().iSrtpAuthTagLen/8;
        }
    if (!TagWithROCLengthL() &&(
    	iHandler.Context().CryptoParams().iSrtpAuthAlg != EAuthNull &&
    	iHandler.Context().CryptoParams().iSrtpAuthAlg != EAuthHMAC_SHA1))
    	{
    	srtpPacketSize -= KSRTPROCLength4;
    	}
    if (!TagWithROCLengthL() &&
    	iHandler.Context().CryptoParams().iSrtpAuthAlg == EAuthRCCm1)
    	{
    	srtpPacketSize -= (KSRTPAuthTagLength80/8);
    	}	
    return srtpPacketSize;
    }
        
        
// ---------------------------------------------------------------------------
// CSRTPPacketRTP::CreateEncryptedPacketL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPPacketRTP::CreateEncryptedPacketL(TUint8* aEncryptedPayloadPtr)
    {
    // create encrypted SRTP packet
    // first count needed packet size
    TUint srtpPacketSize = CountEncryptedPacketSizeL();
    
    return CopyHeaderAndPayloadL(srtpPacketSize, aEncryptedPayloadPtr);    
    }

    
// ---------------------------------------------------------------------------
// CSRTPPacketRTP::CountSenderPacketIndex()
// ---------------------------------------------------------------------------
//
void CSRTPPacketRTP::CountSenderPacketIndex()
    {   
    TUint32 roc= iHandler.ROC();//note it uses local ROC
    
    iPacketIndex = ((65536/*K2EXP16*/* roc) + iSequenceNumber);
    
    }

    
// ---------------------------------------------------------------------------
// CSRTPPacketRTP::SequenceNumber()
// ---------------------------------------------------------------------------
//
TUint16 CSRTPPacketRTP::SequenceNumber()
    {
    return iSequenceNumber;
    }

// ---------------------------------------------------------------------------
// CSRTPPacketRTP::PacketIndex()
// ---------------------------------------------------------------------------
//
TUint64 CSRTPPacketRTP::PacketIndex()
    {
    return iPacketIndex;
    }

// ---------------------------------------------------------------------------
// CSRTPPacketRTP::SetPacketIndex()
// ---------------------------------------------------------------------------
//
void CSRTPPacketRTP::SetPacketIndex(TUint64 aPacketIndex)
    {
    iPacketIndex = aPacketIndex;
    }

// ---------------------------------------------------------------------------
// CSRTPPacketRTP::GetSenderROC
// ---------------------------------------------------------------------------
//
TUint32 CSRTPPacketRTP::GetSenderROC()    
	{
	//Note that SenderROC is now only attached with RTP/SRTP packet
	/*RCCm1 and RCCm2
	Only when SEQ % R (ROC transmission rate) =0
	Then there is attached sender ROC in the packet 4 octets before MAC
	so after payload there is 14 (4+ 10) octets if HMAC-SHA1-80 is used
	*/
	/*RCCm3
	Only when SEQ % R (ROC transmission rate )=0
	Then there is attached sender ROC in the packet 4 octets after packet payload
	but no MAC
	*/
	const TUint8 *pointer = iDataP;	
	pointer+= iHeaderLength;
	pointer+= iPayloadLength;
	pointer+= iHandler.Context().MasterKey().MKI().Length();  
    iSenderROC = static_cast<TUint16>( TSRTPUtils::Read32( pointer ) );
	
	//return
	return iSenderROC;
	}

// ---------------------------------------------------------------------------
// CSRTPPacketRTP::TagWithROCLengthL()
// ---------------------------------------------------------------------------
//
TBool CSRTPPacketRTP::TagWithROCLengthL()
	{
	//mode 1 and 2 add ROC when SEQ%TransRate =0
	//mode 1 will not have any tag if SEQ%TransRate !=0
	TReal remainder;
	TUint16 rate = iHandler.Context().CryptoParams().iROCTransRate;
	User::LeaveIfError( Math::Mod(remainder ,iSequenceNumber,rate));
	if (remainder==0 && 
		((iHandler.Context().CryptoParams().iSrtpAuthAlg == EAuthRCCm1 )||
		(iHandler.Context().CryptoParams().iSrtpAuthAlg  == EAuthRCCm2) ||
		(iHandler.Context().CryptoParams().iSrtpAuthAlg  == EAuthRCCm3)))
		{
		return ETrue;
		}
	return EFalse;
	}


    
// VIRTUAL functions    
HBufC8* CSRTPPacketRTP::CreateDecryptedPacketL(TUint8* /*aDecryptedPayloadPtr*/)
    {    
    ////should call inherited class
    User::Leave(KErrTotalLossOfPrecision);
    return  NULL;    
    }

TPtrC8 CSRTPPacketRTP::AuthenticationTag()
    {   
    //should call inherited class
    return  NULL;     
    }

TPtrC8 CSRTPPacketRTP::MasterKeyIdentifier()
    {
    ////should call inherited class
    return  NULL;        
    }

