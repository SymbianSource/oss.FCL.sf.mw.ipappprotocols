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
* Description:    .
*
*/




// INCLUDES
#include <e32std.h>
#include <e32math.h>
#include "srtpcryptohandler.h"
#include "srtpcryptohandlersrtcp.h"
#include "srtppacketrtcp.h"
#include "srtppacketsrtcp.h"
#include "msrtpcryptohandlercontext.h"

#include "srtputils.h"
#include "srtpmasterkey.h"
#include "srtpmastersalt.h"
#include "msrtpkeyderivation.h"
#include "msrtpcipher.h"
#include "srtpstream.h"
#include "srtpcryptoparams.h"
#include "srtpcipher_aescm128.h"
#include "srtpcipher_null.h"
#include "srtpauthentication_hmac_sha1.h"
#include "srtpauthentication_null.h"
#include "srtpauthentication_rcc.h"

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::CSRTPCryptoHandlerSRTCP
// ---------------------------------------------------------------------------
//
CSRTPCryptoHandlerSRTCP::CSRTPCryptoHandlerSRTCP( CSRTPStream& aStream)
    : CSRTPCryptoHandler(aStream)
            {}

// ---------------------------------------------------------------------------
// Two-phased constructor. 
// 
// ---------------------------------------------------------------------------
//
CSRTPCryptoHandlerSRTCP* CSRTPCryptoHandlerSRTCP::NewL(CSRTPStream& aStream)
    {
    CSRTPCryptoHandlerSRTCP* self = CSRTPCryptoHandlerSRTCP::NewLC( aStream);
    CleanupStack::Pop( self );
    return self;    
    }


// -----------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSRTPCryptoHandlerSRTCP* CSRTPCryptoHandlerSRTCP::NewLC( CSRTPStream& aStream)
    {
    CSRTPCryptoHandlerSRTCP* self = new( ELeave )CSRTPCryptoHandlerSRTCP( aStream);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::~CSRTPCryptoHandlerSRTCP
// ---------------------------------------------------------------------------
//
CSRTPCryptoHandlerSRTCP::~CSRTPCryptoHandlerSRTCP( )
    {

    }


// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::ConstructL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::ConstructL() 
    {
    //construct the upper class
    CSRTPCryptoHandler::ConstructL();   
    SetEncAndAuthL(CryptoParams().iSrtcpEncAlg,
    				CryptoParams().iSrtcpAuthAlg);
    iReplayPktIndex=0;
    iReplayIndexDelta=0; 
    iSRTCPMasterKeyLifeTime = CryptoParams().iMasterKeysLifeTime;
    
    if ( !iSRTCPMasterKeyLifeTime || 
    	iSRTCPMasterKeyLifeTime > KSRTCPMaterKeysLifeTime )
    	{
    	iSRTCPMasterKeyLifeTime = KSRTCPMaterKeysLifeTime;								
    	}    
    }

// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTCP::MasterKeysUpdated()
// ---------------------------------------------------------------------------
//
TBool CSRTPCryptoHandlerSRTCP::MasterKeysUpdated()
    {
    return iMasterDataUpdated;
    
    }  


// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTCP::CheckMasterKeyIdentifierL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::CheckMasterKeyIdentifierL()
    {
    // RFC 3711, section 3.4):
    // verify that this packet uses the same MKI
    // than we have in CryptoContext    
    if(Context().MasterKey().MKI()!= KNullDesC8)
    	{
	    if (Context().MasterKey().MKI().Length() > 0)
	        {
	        TPtrC8 mki = iCurrentPacket->MasterKeyIdentifier();
	        if (Context().MasterKey().MKI().Compare(mki) != 0)
	            {
	            // we can not process this packet, since MKI does not match
	             User::Leave(KErrNotFound);
	            }
	        }       
    	}
    }  
// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTCP::AuthenticateL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::AuthenticateL()
    {
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTCP::AuthenticateL() entry" );
    if ( CryptoParams().iSrtcpAuthAlg != EAuthNull &&
        iCurrentPacket->AuthenticationTag().Length()==0)
        {
        User::Leave(KErrNotFound);
        }
    
    if (iCurrentPacket->AuthenticationTag().Length()>0)
        {        
        // calculate auth tag from the authenticated area..
        HBufC8* calculatedAuthTag = CalculateAuthTagL();                
        CleanupStack::PushL(calculatedAuthTag);
       	SRTP_DEBUG_DETAIL( "SRTCP HMAC caculated Authentication tag" );
        SRTP_DEBUG_PACKET( *calculatedAuthTag );    
    	                                                 
        // finally, compare the two auth tags..
        if (iCurrentPacket->AuthenticationTag().Compare(
                                    *calculatedAuthTag) != 0)
            {
            // authentication failed, since tags dont match
            SRTP_DEBUG_DETAIL( "authenticate tag is not match to caculated one" );
            User::Leave(KErrNotFound);
            }
        
        // then, lose the auth tag we just calculated 
        CleanupStack::PopAndDestroy(calculatedAuthTag);       
                       
        }
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTCP::AuthenticateL() exit" );    
    }  
        
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::CountIV_And_TransformL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCryptoHandlerSRTCP::CountIV_And_TransformL()
    {        
    /*    
    RTCP case is different. 
    */ 
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTCP::CountIV_And_TransformL entry" );       
    //IV is 128 bit value
    TBuf8<16> iv;           
    // set IV to be 128 bit, 16 octets
    iv.SetLength(16);       
    
    // count the IV for decryption   
    //RFC 3711 4.1.1 
    TInt ret = TSRTPUtils::CountIV(iv, *iSessionSaltKey, SSRC(), iSRTCPIndexEStripped);
    
    SRTP_DEBUG_TUINT_VALUE( "SRTCP in ssrc" , SSRC());
                
    if (ret!=KErrNone)
        {
        User::Leave(ret);
        }
   	SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTCP::CountIV_And_TransformL exit" );       

    // Step 5, encryption (in RFC 3711, section 3.3):    
    return iCipher->TransformL( *iSessionEncrKey, 
                                iv, iCurrentPacket->Payload() );  
    }
    
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::EncryptL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCryptoHandlerSRTCP::EncryptL()
    {
   	SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTCP::EncryptL() entry" );       

   	HBufC8* encryptedPayload = CountIV_And_TransformL();      
    CleanupStack::PushL(encryptedPayload);         
    
    TUint8* encryptedPayloadPtr = const_cast<TUint8*>(encryptedPayload->Des().Ptr());        

    HBufC8* srtpPacket = iCurrentPacket->CreateEncryptedPacketL(encryptedPayloadPtr);
    CleanupStack::PushL(srtpPacket);     
    
    TUint8* srtpPacketPtr = const_cast<TUint8*>(srtpPacket->Des().Ptr());                
  	  
    //Step 6, Add E flag and SRTCP Index (RFC 3711 section 3.4)
    AddEflagAndSrtcpIndex(srtpPacketPtr);
    //Step 7, Add MKI (in RFC 3711, section 3.3):    
    AddMKIToPacket(srtpPacketPtr);

    //Step 8, Add authentication tag (in RFC 3711, section 3.3):
    AddAuthTagToPacketL(srtpPacketPtr);     
    

    CleanupStack::Pop(srtpPacket); 
    SRTP_DEBUG_DETAIL( "Encrypted Payload" );
    SRTP_DEBUG_PACKET( *encryptedPayload );    
                                                
    CleanupStack::PopAndDestroy(encryptedPayload);   
    iNumPackets++;     
    
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTCP::EncryptL() exit" );         
    
    return srtpPacket;        
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::DecryptL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCryptoHandlerSRTCP::DecryptL()
    {
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTCP::DecryptL() entry" ); 
    // Step 6, decryption (in RFC 3711, section 3.3):    
    HBufC8* decryptedPayload = CountIV_And_TransformL();      
    CleanupStack::PushL(decryptedPayload);                                               
                                                                 
    TUint8* decryptedPayloadPtr = const_cast<TUint8*>(decryptedPayload->Des().Ptr());        
    HBufC8* rtpPacket = iCurrentPacket->CreateDecryptedPacketL(decryptedPayloadPtr);
    TUint8* rtpPacketPtr = const_cast<TUint8*>(rtpPacket->Des().Ptr());        
    SRTP_DEBUG_DETAIL( "SRTCP Decrypt Payload" );
    SRTP_DEBUG_PACKET( *decryptedPayload );    
                                             
    CleanupStack::PopAndDestroy(decryptedPayload);    
    
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTCP::DecryptL() exit" ); 
                
    return rtpPacket;                              
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::AddMKIToPacket
// 
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::AddMKIToPacket(TUint8* aSrtpPacketPtr)
    {
    TUint8* ptr = aSrtpPacketPtr;        
    if( Context().MasterKey().MKI()!= KNullDesC8)
    	{
	    //Step 6, Add MKI (in RFC 3711, section 3.3):    
	    if (Context().MasterKey().MKI().Length()>0  )
	        {
	        ptr += iCurrentPacket->HeaderLength();
	        ptr += iCurrentPacket->PayloadLength();
	        ptr += KSRTCPPacketIndexLength4;
	        TUint8* mkiPtr = const_cast<TUint8*>(Context().MasterKey().MKI().Ptr());        
	        Mem::Copy( ptr, mkiPtr, Context().MasterKey().MKI().Length());                
	        }
    	}
    }
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::AddAuthTagToPacketL
// 
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::AddAuthTagToPacketL(TUint8* aSrtpPacketPtr)
    {
    TUint8* ptr = aSrtpPacketPtr;        

    //Step 7, Add authentication tag (in RFC 3711, section 3.3):
    if ( CryptoParams().iSrtcpAuthTagLen>0)
        {
        ptr += iCurrentPacket->HeaderLength();
        ptr += iCurrentPacket->PayloadLength();
        ptr += KSRTCPPacketIndexLength4 ; /*srtcp index length*/
        if(Context().MasterKey().MKI()!= KNullDesC8)
    		{
        	ptr += Context().MasterKey().MKI().Length(); 
    		}
        //Copy the authenticatin portion only not with MKI key part
		//RFC 3711 3.4 SRCP authentication portion including index
  		TInt len= iCurrentPacket->HeaderLength() + 	iCurrentPacket->PayloadLength()
  				+ KSRTCPPacketIndexLength4 /*SRTCP index length*/;
   		HBufC8* authPortion = HBufC8::NewMaxLC(len);
        TUint8* authptr = const_cast<TUint8*>(authPortion->Des().Ptr());
        Mem::Copy( authptr, aSrtpPacketPtr, len);
    	authPortion->Des().SetLength(len);    
                 
    	HBufC8* authTag = DoAuthenticationL(*authPortion, KNullDesC8);
                  
       
    	CleanupStack::PopAndDestroy( authPortion );        
    	authPortion=NULL;       
        
        
        Mem::Copy( ptr, authTag->Ptr(), authTag->Length());
        delete authTag;
        authTag=NULL;
        }    
    }
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::CalculateAuthTagL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCryptoHandlerSRTCP::CalculateAuthTagL()
    {  
    //RFC 3711 3.4 SRCP authentication portion including index                           
    // that is, M = header + payload (Report block)  + SRTCP indexp     
    
    // calculate auth tag from the authenticated area..
   
 	TInt len= iCurrentPacket->HeaderLength() + 	
 		iCurrentPacket->PayloadLength()	+4; /*SRTCP index length*/
   	HBufC8* authPortion = iCurrentPacket->Get_Concatenated_MDataL(ETrue);
   	CleanupStack::PushL( authPortion );
    authPortion->Des().SetLength(len);    
                 
    HBufC8* calculatedAuthTag = DoAuthenticationL(*authPortion, KNullDesC8);
                  
    // then, lose authPortion buffer
         
    CleanupStack::PopAndDestroy( authPortion );  
    authPortion= NULL; 
    
    return calculatedAuthTag;
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::DoAuthenticationL
// 
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCryptoHandlerSRTCP::DoAuthenticationL(const TDesC8& aAuthenticatedArea,
											const TDesC8& aRoc)
    {                                     
    // calculate auth tag from the authenticated area..
    TUint authtaglen= CryptoParams().iSrtcpAuthTagLen;
    // Note that RCCm3 work as Auth Null so taglen does not matter to RTP/RTCP
    if (aRoc.Length() && 
    	(CryptoParams().iSrtcpAuthAlg == EAuthRCCm1 ||
		CryptoParams().iSrtcpAuthAlg == EAuthRCCm2 ))
    	{
    	authtaglen-= KSRTPAuthTagLength32 ;
    	}
    if 	((!aRoc.Length()) && 
    		( CryptoParams().iSrtcpAuthAlg != EAuthNull &&
    		CryptoParams().iSrtcpAuthAlg != EAuthHMAC_SHA1))
    	{
    	//This is for RTCP if using RCC mode because currently handler sharing 
    	//the same context
		authtaglen=KSRTPAuthTagDefaultLength;
    	}
    return iAuthenticator->AuthenticateL(authtaglen,
                                    *iSessionAuthKey, 
                                    aAuthenticatedArea,
                                    aRoc);                  
    }
    
// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTCP::InitializePlainPacketL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::InitializePlainPacketL(const TDesC8& aPacket)
    {
    if(iCurrentPacket)
    	delete iCurrentPacket;iCurrentPacket = NULL;   
    iCurrentPacket = CSRTPPacketRTCP::NewL(aPacket, *this);  
    }  

// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTCP::InitializeEncryptedPacketL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::InitializeEncryptedPacketL(const TDesC8& aPacket)
    {
    if(iCurrentPacket)
    	delete iCurrentPacket;iCurrentPacket = NULL;       
    iCurrentPacket = CSRTPPacketSRTCP::NewL(aPacket, *this);        
    }  

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::DeriveSessionKeysL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::DeriveSessionKeysL()
    {
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTCP::DeriveSessionKeysL() Entry" );

    TBuf8<100> sessionKey_X;
    DeleteSessionKeys();        

    TUint64 r = TSRTPUtils::Cnt_r(iSRTCPIndexEStripped, 
    				CryptoParams().iSrtcpKeyDervRate);
    
    // session encryption key
    //sessionKey_X.SetLength(Context()->MasterKey().EncrKeyLength());
    sessionKey_X.SetLength(100);
    // session encryption key derivation :
    sessionKey_X.FillZ();    
    Count_X(r, KSRTCPEncryptionLabel, KSRTPIndexLength, sessionKey_X);
    iSessionEncrKey = iKeyDeriver->PRF_128BitL(Context().MasterKey().EncrKeyLength(),
                                            Context().MasterKey().MasterKey(), 
                                            sessionKey_X);
    SRTP_DEBUG_DETAIL( "SRTCP iSessionEncrKey as CipherKey " );
    SRTP_DEBUG_PACKET( *iSessionEncrKey );     
                                    
    // session authentication key
    sessionKey_X.FillZ();
    //sessionKey_X.SetLength(Context()->MasterKey().AuthKeyLength());
    Count_X(r, KSRTCPAuthenticationLabel, KSRTPIndexLength, sessionKey_X);        
    iSessionAuthKey = iKeyDeriver->PRF_128BitL(Context().MasterKey().AuthKeyLength(),
                                            Context().MasterKey().MasterKey(), 
                                            sessionKey_X);

	SRTP_DEBUG_DETAIL( "SRTCP iSessionAuthKey" );
    SRTP_DEBUG_PACKET( *iSessionAuthKey);    
       
    // session salt key
    sessionKey_X.FillZ();
    //sessionKey_X.SetLength(Context()->MasterSalt().SaltLength());
    Count_X(r, KSRTCPSaltingLabel, KSRTPIndexLength, sessionKey_X);        
    iSessionSaltKey = iKeyDeriver->PRF_128BitL(Context().MasterSalt().SaltLength(),
                                            Context().MasterKey().MasterKey(), 
                                            sessionKey_X);
    SRTP_DEBUG_DETAIL( "SRTCP   iSessionSaltKey" );
    SRTP_DEBUG_PACKET( *iSessionSaltKey ); 
               
    
   	iMasterDataUpdated = EFalse;  
   	SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTCP::DeriveSessionKeysL() Exit" );
                                                                             
 	}
    

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::StripEBit()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::StripEBit()
    {    
    // strip the Ebit away
    iSRTCPIndexEStripped = iSRTCPIndex ^ 0x80000000;
    } 
    

// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTCP::ReplayProtectionL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::ReplayProtectionL()
    {
    if ( CryptoParams().iSrtcpReplayProtection)
    	{
	    TInt delta =0;
	  	delta= iSRTCPIndexEStripped/*updated one*/ - iReplayPktIndex; //set as 0
		      
		//Index check
		if ( CryptoParams().iSrtcpReplayProtection)
			{
			User::LeaveIfError (ReplayCheck(delta));
			}
		//if the index is inside the replay window and index does not appear in rdb
		//then Index add after authentication passed  
	    iReplayIndexDelta= delta;
    	}
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::CountSenderPacketIndex()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::CountSenderPacketIndexL()
    {  
   	//Re-Key was needed earlier but key has not yet been updated
    //iReKey is false
    //Call it after each packet has been sent
    //TReal modulus(0x10000000);
    TReal twoexp31=  0x80000000;
    
    TReal srtcpindexincremented= iSRTCPIndexEStripped+1;
    TReal result;    
    TInt32 intResult;       
    // count (ROC - 1) mod 2^31   
       
    TInt res = Math::Mod(result, srtcpindexincremented, twoexp31); 
    if (res!=KErrNone)
        {
        User::Leave(res);
        }
    
    intResult = (TUint)result;
    
    iSRTCPIndexEStripped = intResult;
    // E bit set to 1 indicates encrpted pacet 0 otherwise
    iSRTCPIndex =0x00000000;
    if (CryptoParams().iSrtcpEncAlg!=ENullAlg)
    	{
    	iSRTCPIndex =0x80000000;	
    	}
    	
    iSRTCPIndex = iSRTCPIndex + iSRTCPIndexEStripped;
    iCurrentPacket->SetPacketIndex(iSRTCPIndex);
    //Checking next Index if Keys needed to be renew
    IndexReKeyCheckL();
    }
    
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::ReceiverPacketIndexL()
// ---------------------------------------------------------------------------
//    
void CSRTPCryptoHandlerSRTCP::ReceiverPacketIndexL()
    { 
    if (iReKey)
    	{
    	//Re-Key was needed earlier but key has not yet been updated
    	User::Leave(KErrTooBig);
    	}      
    /* Can be done in the Replay protection
    if (iCurrentPacket->PacketIndex()<iSRTCPIndex)
    	{
    	//the receiver packet index is samller than the earlier packet index
    	//ignore this packet by leave
    	User::Leave(KErrCorrupt);
    	}*/
    //Updated the packet index during the construction of Packet	
    iSRTCPIndex = iCurrentPacket->PacketIndex();
    //Strip EBit only when the encAlg is not NULL
    if (CryptoParams().iSrtcpEncAlg!=ENullAlg)
    	StripEBit();
    IndexReKeyCheckL();
    }
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::InitialPacketIndex()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::InitialPacketIndex()
	{
	//iSRTCPIndexEStripped=0x00;
	iSRTCPIndex =0x80000000;
	StripEBit();
	iSRTCPIndex = iSRTCPIndex + iSRTCPIndexEStripped;
	iCurrentPacket->SetPacketIndex(iSRTCPIndex);
	}

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::AddEflagAndSrtcpIndex()
// ---------------------------------------------------------------------------
//	
void CSRTPCryptoHandlerSRTCP::AddEflagAndSrtcpIndex(TUint8* aSrtcpPacketPtr)
	{
	//EBit set to 1 as hardcoded
	
	TUint8* ptr = aSrtcpPacketPtr;        

    //Step 6, Add E flag and SRTCP Index (RFC 3711 section 3.4)
    
    ptr += iCurrentPacket->HeaderLength();
    ptr += iCurrentPacket->PayloadLength();
    //EBit set to 1 indicates encryted packet, otherwise set to 0
    if (CryptoParams().iSrtcpEncAlg!=ENullAlg )
    	{
    	TSRTPUtils::Write32( ptr, iSRTCPIndex );
    	}
	else
    	{
    	TSRTPUtils::Write32( ptr, iSRTCPIndexEStripped);
    	}
	
	}
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::IndexReKeyCheck()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTCP::IndexReKeyCheckL()
	{
	TReal twoexp31=  0x80000000;
    
    TReal srtcpIndexIncremented= iSRTCPIndexEStripped+1;
    TReal remainder;    
       
    
       
    TInt res = Math::Mod(remainder, srtcpIndexIncremented, twoexp31); 
    if (res!=KErrNone)
        {
        User::Leave(res);
        }
    if (remainder == 0.0 && iSRTCPIndexEStripped != 0)
    	{
    	//RFC 3711 8.1 & 9.2
    	iReKey = ETrue;
    	iStream.KeyExpired();
    	//called incase the packets index is big in the begining
    	iStream.ReKeyNeeded();
    	}
    //should create function for checking.when value is bigger
    TUint64 reKeyNumPacket = iNumPackets + KLeftNumOfPacketBeforeReKey; 
    // Re-key only call one time when there are 100 number packets left to send/recv
    if ( ( reKeyNumPacket ) == iSRTCPMasterKeyLifeTime )
    	{
    	iStream.ReKeyNeeded();
    	}
    //Key is really expired	
    if ( iNumPackets >=  iSRTCPMasterKeyLifeTime )
    	{
		iReKey = ETrue;
    	iStream.KeyExpired();        
    	}
    }

 // ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTCP::AddReplayIndex()
// ---------------------------------------------------------------------------
//   
void CSRTPCryptoHandlerSRTCP::AddReplayIndex()
	{
	if (CryptoParams().iSrtcpReplayProtection)
		{
		if (iReplayIndexDelta > 0) 
			{
		    /* shift forward by delta */
		    iReplayPktIndex = iSRTCPIndexEStripped;
		  	} 
		 else 
		  	{
		    /* delta is in window, so flip bit in bitmask */
		    /* mark as seen */
		    TInt diff= -(iReplayIndexDelta);
		    iBitmap |= ((TUint64)1 << diff); 
		  	}
		  /* note that we need not consider the case that delta == 0 */
		}
	//Add Number of packets received
	iNumPackets++;  
	}
