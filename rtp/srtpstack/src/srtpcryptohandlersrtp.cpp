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
#include "srtpcryptohandler.h"
#include "srtpcryptohandlersrtp.h"
#include "msrtpcryptohandlercontext.h"
#include "srtputils.h"
#include <e32math.h>
#include "srtpstream.h"
#include "srtpmasterkey.h"
#include "srtpmastersalt.h"
#include "msrtpkeyderivation.h"
#include "msrtpauthentication.h"
#include "msrtpcipher.h"
#include "srtppacketrtp.h"
#include "srtppacketsrtp.h"
#include "srtpcryptoparams.h"
#include "srtpcipher_aescm128.h"
#include "srtpcipher_null.h"
#include "srtpauthentication_hmac_sha1.h"
#include "srtpauthentication_null.h"
#include "srtpauthentication_rcc.h"

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::CSRTPCryptoHandlerSRTP
// ---------------------------------------------------------------------------
//
CSRTPCryptoHandlerSRTP::CSRTPCryptoHandlerSRTP( CSRTPStream& aStream):
            CSRTPCryptoHandler(aStream)
            {}

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::~CSRTPCryptoHandlerSRTP
// ---------------------------------------------------------------------------
//
CSRTPCryptoHandlerSRTP::~CSRTPCryptoHandlerSRTP( )
    {

    }


// ---------------------------------------------------------------------------
// Two-phased constructor. 
// 
// ---------------------------------------------------------------------------
//
CSRTPCryptoHandlerSRTP* CSRTPCryptoHandlerSRTP::NewL(CSRTPStream& aStream)
    {
    CSRTPCryptoHandlerSRTP* self = CSRTPCryptoHandlerSRTP::NewLC( aStream);
    CleanupStack::Pop( self );
    return self;    
    }

// -----------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::NewLC
// -----------------------------------------------------------------------------
//
CSRTPCryptoHandlerSRTP* CSRTPCryptoHandlerSRTP::NewLC(CSRTPStream& aStream)
    {
    CSRTPCryptoHandlerSRTP* self = new( ELeave )CSRTPCryptoHandlerSRTP( aStream);
    CleanupStack::PushL( self );    
    self->ConstructL();
    return self;
    }



// -----------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::SetReceiver_s_l
// -----------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::SetReceiver_s_l()
    {
    // initialize s_l value for the first packet
    iS_L=iCurrentPacket->SequenceNumber();
    }


// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::ConstructL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::ConstructL() 
    {    
    //construct the upper class
    CSRTPCryptoHandler::ConstructL();
    SetEncAndAuthL( CryptoParams().iSrtpEncAlg,
    				CryptoParams().iSrtpAuthAlg	);
    //when the session starts set ROC to zero        
    iROC =0; //only using when RCC mode as sender Roc
    SetROC(0);      
    iPktIndex=0;//Replay Index
    iReplayIndexDelta=0;
    iSRTPMasterKeyLifeTime = CryptoParams().iMasterKeysLifeTime;
    if ( !iSRTPMasterKeyLifeTime || 
    	iSRTPMasterKeyLifeTime > KSRTPMasterKeysLifeTime )
    	{
    	iSRTPMasterKeyLifeTime = KSRTPMasterKeysLifeTime;								
    	}    
    }

// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTP::MasterKeysUpdated()
// ---------------------------------------------------------------------------
//
TBool CSRTPCryptoHandlerSRTP::MasterKeysUpdated()
    {
    return iMasterDataUpdated;
    }  

 
// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTP::InitializePlainPacketL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::InitializePlainPacketL(const TDesC8& aPacket)
    {
    if(iCurrentPacket)
    	delete iCurrentPacket; iCurrentPacket=NULL; 
    iCurrentPacket = CSRTPPacketRTP::NewL(aPacket, *this);
    }  

// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTP::InitializeEncryptedPacketL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::InitializeEncryptedPacketL(const TDesC8& aPacket)
    {
    if(iCurrentPacket)
    	delete iCurrentPacket;iCurrentPacket=NULL; 
    iCurrentPacket = CSRTPPacketSRTP::NewL(aPacket, *this);
    
    }  
        
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::DecryptL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCryptoHandlerSRTP::DecryptL()
    {    
    // Step 6, decryption (in RFC 3711, section 3.3):    
  	SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTP::DecryptL() Entry" );

	HBufC8* decryptedPayload = CountIV_And_TransformL();      
    CleanupStack::PushL(decryptedPayload);                                               
                                                                 
    TUint8* decryptedPayloadPtr = const_cast<TUint8*>(decryptedPayload->Des().Ptr());        
    HBufC8* rtpPacket = iCurrentPacket->CreateDecryptedPacketL(decryptedPayloadPtr);
    TUint8* rtpPacketPtr = const_cast<TUint8*>(rtpPacket->Des().Ptr());        
    
    SRTP_DEBUG_DETAIL( "SRTP Decrypt Payload" );
    SRTP_DEBUG_PACKET( *decryptedPayload );    
                                            
    CleanupStack::PopAndDestroy( decryptedPayload );  
    
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTP::DecryptL() Exit" );
           
    return rtpPacket;                              
    }


// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::AuthenticateL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::AuthenticateL()
    {         
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTP::AuthenticateL() ENTRY" );
            
    //Note that RCCm1 only to integrity check when carrying ROC
    if ( iCurrentPacket->AuthenticationTag().Length() > 0 && 
    	( CryptoParams().iSrtpAuthAlg == EAuthHMAC_SHA1 ||
    	CryptoParams().iSrtpAuthAlg == EAuthRCCm2||
    	( CryptoParams().iSrtpAuthAlg == EAuthRCCm1&& CarryROC() ) ))
        {        
        // calculate auth tag from the authenticated area..
        HBufC8* calculatedAuthTag = CalculateAuthTagL(); 
        	if (!calculatedAuthTag)
        	{
        	User::Leave(KErrNotFound);	
        	}
        CleanupStack::PushL(calculatedAuthTag);
                        
        // finally, compare the two auth tags..
        if (iCurrentPacket->AuthenticationTag().Compare(
                                    *calculatedAuthTag) != 0)
            {
            SRTP_DEBUG_DETAIL( "SRTP HMAC caculated Authentication tag and the compared is wrong" );
            SRTP_DEBUG_PACKET( *calculatedAuthTag );    
    		 
            // authentication failed, since tags dont match
            User::Leave(KErrNotFound);
            }
        
        // then, lose the auth tag we just calculated 
        CleanupStack::PopAndDestroy(calculatedAuthTag);       
                       
        }
    //authentication successful then set localROC to sender ROC need to be implemented     
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTP::AuthenticateL() EXIT" );
    
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::CalculateAuthTagL()
// only using in SRTP 
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCryptoHandlerSRTP::CalculateAuthTagL()
    {                                     
    // M denotes the authenticated area, that is header + payload..
    // ..but for RTP we need to add the ROC,
    // that is, M = header + payload || ROC        
 
    TBuf8<16> roc;
    TBuf8<8> roc_hexed;    
    //using iV instead of ROC because ROC is not updated yet until decryption sucessfull 
    roc.AppendNumUC(iV, EHex); 
    
    TSRTPUtils::DeHex(roc, roc_hexed); 
    TUint size = roc.Size();
                     
	for (TInt i=0; i<(4-size); i++)
        {
        roc_hexed.Append(0x00);        
        }
 	TInt len = iCurrentPacket->HeaderLength() + iCurrentPacket->PayloadLength();
   	HBufC8* authPortion = iCurrentPacket->Get_Concatenated_MDataL(EFalse);
   	CleanupStack::PushL(authPortion);
    authPortion->Des().SetLength(len);    
                 
    HBufC8* calculatedAuthTag = DoAuthenticationL(*authPortion, roc_hexed);
    
    CleanupStack::PopAndDestroy(authPortion);          
    
    roc.Zero();
    roc_hexed.Zero();  
    return calculatedAuthTag;
    }


// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::CountReceiverPacketIndexL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::CountReceiverPacketIndexL()
    {   
    if (iReKey)
    	{
    	User::Leave(KErrTooBig);
    	}
    TReal result;                          
    
    TReal twoexp32=  4294967296;
    //TReal twoexp16= 65536;
    TReal ROCnumber= ROC()-1;
    
    if (iS_L < 32768)
        {
        if (iCurrentPacket->SequenceNumber() - iS_L > 32768)
            {
            User::LeaveIfError(Math::Mod(result, ROCnumber /*ROC() - 1*/, twoexp32));
            User::LeaveIfError(Math::Int(iV, result));
            }
        else
            {
            iV = ROC();
            }
        }
    else
        {
        if (iS_L - 32768 > iCurrentPacket->SequenceNumber())
            {
            User::LeaveIfError(Math::Mod(result, ROC() + 1, twoexp32));
            User::LeaveIfError(Math::Int(iV, result));                                    
            }
        else
            {
            iV = ROC();
            }        
        }
                     
    ReKeyCheck();
    
    //Set for current Packet index  
    //check if authAlg is RCCmode and if carry ROC 
    if (CarryROC())
    	{
    	iCurrentPacket->SetPacketIndex(CountSndIndxbySndROC());
    	iROC = iCurrentPacket->GetSenderROC();
    	}
    else
    	{
    	iCurrentPacket->SetPacketIndex(iCurrentPacket->SequenceNumber() + iV * 65536/*K2EXP16*/);	
    	}	
    
    }

    
// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTP::CheckMasterKeyIdentifierL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::CheckMasterKeyIdentifierL()
    {
    // Step 3 (in RFC 3711, section 3.3):
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
// CSRTPCryptoHandlerSRTP::Update_s_l_and_RocL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::Update_s_l_and_RocL()
    {   

    TReal result;      
    TInt32 intResult;    
    TReal modulus(0x100000000);
     
    // count (ROC - 1) mod 2^32   
       
    TInt res = Math::Mod(result, ROC() - 1, modulus);
    if (res!=KErrNone)
        {
        User::Leave(res);
        }
    
    intResult = (TUint)result;
        
    // if v == (ROC - 1) mod 2^32
    if (iV == intResult)
        {
        // no update
        return;
        }
            
    // if v == ROC        
    if (iV == ROC())
        {
        // if SEQ > s_l
        if (iCurrentPacket->SequenceNumber() > iS_L)
            {
            //set s_l = SEQ
            iS_L = iCurrentPacket->SequenceNumber();
            }        
        }
    else
        {
        // count (ROC + 1) mod 2^32             
        TReal temp=  4294967296;        
        res = Math::Mod(result, ROC() + 1, temp/*K2EXP32*/);
        if (res!=KErrNone)
            {
            User::Leave(res);
            }
        res = Math::Int(intResult, result);
        if (res!=KErrNone)
            {
            User::Leave(res);
            }
        
        // if v == (ROC + 1) mod 2^32
        if (iV == intResult)
            {
            // set s_l to SEQ, and
            // ROC to v
            iS_L = iCurrentPacket->SequenceNumber();
            SetROC(iV);
            }
        } 
    //RCCm1 and RCCm2 authentication successful then set localROC to sender ROC                     
    //no authentication needed for RCCm3
     if ((iCurrentPacket->AuthenticationTag().Length()>0 && CarryROC())||
     		(CryptoParams().iSrtpAuthAlg == EAuthRCCm3 &&
     		CarryROC() && !CryptoParams().iIsRCCm3Sync))
     	{
     	SetROC(iROC);
     	}
    
    }

        
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::CountSenderPacketIndex()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::CountSenderPacketIndexL()
    {   
    if (iReKey)
    	{
    	User::Leave(KErrTooBig);
    	}
    //iReKey should be false
    iCurrentPacket->CountSenderPacketIndex();
    /*For Re-Key,
    RFC 3711 3.2.1 and 9.2
    i= 2^16*roc + seq
    */
    
    ReKeyCheck();
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::ReKeyCheck()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::ReKeyCheck()
	{
	TReal remainder;
    Math::Mod(remainder, iCurrentPacket->SequenceNumber(), 0x10000-1);

    // update ROC when SEQ wraps 2^16,
    // with following exception:
    // when SEQ is 0, MOD result is also 0,
    // but we want remainder to be 0
    // only when SEQ == 65535     
    if (remainder == 0.0 && iCurrentPacket->SequenceNumber() != 0)
    	{
    	if (ROC()+1 >= (0x100000000-1))
	        {
	        //RFC3711 3.2.1, i=2^16(1+ROC) , i can not more than 2^48
	        //Hex 0x100000000 is 2^32
	        // CRITICAL error
	        // we have sent more than 2^48 packets,
	        // can not send any more, should do reKey  
	        //Should consider sending and receiving case  
	        //Re-Key is needed, can send bye packet or event should be logged
	    	//Sendbye should be at application level 
	    	iReKey = ETrue;
	    	iStream.KeyExpired();        
	        iStream.ReKeyNeeded();
	    	} 
    	}
    TUint64 reKeyNumPacket = iNumPackets + KLeftNumOfPacketBeforeReKey; 
    //Re-Key is only called once when the number is equal
    if ( ( reKeyNumPacket ) == iSRTPMasterKeyLifeTime )
    	{
    	//Call for Rekey before expired
    	iStream.ReKeyNeeded();
    	}
    //when packets is really gets there, key expired
    if ( iNumPackets >=  iSRTPMasterKeyLifeTime )
    	{
		iReKey = ETrue;
    	iStream.KeyExpired();        
    	}
	}
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::CountIV_And_TransformL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCryptoHandlerSRTP::CountIV_And_TransformL()
    {        
    /*    
    This step is the same in RTP encryption and decryption,
    RTCP case is different. 
    */    
    //IV is 128 bit value
    TBuf8<16> iv;           
    // set IV to be 128 bit, 16 octets
    iv.SetLength(16);       
    
    // count the IV for decryption   

    TInt ret = TSRTPUtils::CountIV(iv, *iSessionSaltKey, SSRC(), iCurrentPacket->PacketIndex());
    
    if (ret!=KErrNone)
        {
        User::Leave(ret);
        }
    
    // Step 5, encryption (in RFC 3711, section 3.3):    
    return iCipher->TransformL( *iSessionEncrKey, 
                                iv, iCurrentPacket->Payload() );  
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::EncryptL()
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCryptoHandlerSRTP::EncryptL()
    {        
    // Step 5, encryption (in RFC 3711, section 3.3):    
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTP::EncryptL() ENTRY" );
    
    SRTP_DEBUG_DETAIL( "CountIV_And_TransformL" ); 
    HBufC8* encryptedPayload = CountIV_And_TransformL();      
    CleanupStack::PushL(encryptedPayload);         
   
    TUint8* encryptedPayloadPtr = const_cast<TUint8*>(encryptedPayload->Des().Ptr());        

    HBufC8* srtpPacket = iCurrentPacket->CreateEncryptedPacketL(encryptedPayloadPtr);
    CleanupStack::PushL(srtpPacket);     
    
    TUint8* srtpPacketPtr = const_cast<TUint8*>(srtpPacket->Des().Ptr());                
  	  
    //Step 6, Add MKI (in RFC 3711, section 3.3):    
    AddMKIToPacket(srtpPacketPtr);

	SRTP_DEBUG_DETAIL( "AddAuthTag" );
       
    //Step 7, Add authentication tag (in RFC 3711, section 3.3):
    if(IfAddAuthTag())
    	{
    	AddAuthTagToPacketL(srtpPacketPtr);        	
    	}
    
    CleanupStack::Pop(srtpPacket); 
    
    SRTP_DEBUG_DETAIL( "Encrypted Payload" );
    SRTP_DEBUG_PACKET(  *encryptedPayload );    
                                               
    CleanupStack::PopAndDestroy(encryptedPayload);   
    
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTP::EncryptL() EXIT" );
                 
    return srtpPacket;        
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::AddMKIToPacket
// 
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::AddMKIToPacket(TUint8* aSrtpPacketPtr)
    {
    TUint8* ptr = aSrtpPacketPtr;        
    if(Context().MasterKey().MKI()!= KNullDesC8)
    	{
	    //Step 6, Add MKI (in RFC 3711, section 3.3):    
	    if (Context().MasterKey().MKI().Length()>0)
	        {
	        ptr += iCurrentPacket->HeaderLength();
	        ptr += iCurrentPacket->PayloadLength();
	        TUint8* mkiPtr = const_cast<TUint8*>(Context().MasterKey().MKI().Ptr());        
	        Mem::Copy( ptr, mkiPtr, Context().MasterKey().MKI().Length());                
	        }
    	}
    }
    

// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::AddAuthTagToPacketL
// 
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::AddAuthTagToPacketL(TUint8* aSrtpPacketPtr)
    {
    TUint8* ptr = aSrtpPacketPtr;        

    //Step 7, Add authentication tag (in RFC 3711, section 3.3):
    if (CryptoParams().iSrtpAuthTagLen>0)
        {
        ptr += iCurrentPacket->HeaderLength();
        ptr += iCurrentPacket->PayloadLength();
        if(Context().MasterKey().MKI()!= KNullDesC8)
	    	{
	        ptr += Context().MasterKey().MKI().Length(); 
	    	}
        TBuf8<16> roc;
    	TBuf8<8> roc_hexed;
    	  
    	if (!CarryROC())
    		{
    		roc.AppendNumUC(ROC(), EHex); 	
    		}
    	else
    		{
    		roc.AppendNumUC(iROC, EHex);
    		}	
    	
    
    	TSRTPUtils::DeHex(roc, roc_hexed); 
    	TUint size = roc.Size();
    	                
		for (TInt i=0; i<(4-size); i++)
        	{
        	roc_hexed.Append(0x00);        
        	}
        //Copy the authenticatin portion only not with MKI key part
  		TInt len= iCurrentPacket->HeaderLength() + 	iCurrentPacket->PayloadLength();
   		HBufC8* authPortion = HBufC8::NewMaxLC(len);
        TUint8* authptr = const_cast<TUint8*>(authPortion->Des().Ptr());
        Mem::Copy( authptr, aSrtpPacketPtr, len);
    	authPortion->Des().SetLength(len);    
                 
    	HBufC8* authTag = DoAuthenticationL(*authPortion, roc_hexed);
      	CleanupStack::PopAndDestroy( authPortion );         
        authPortion=NULL;
       
        if (CarryROC())
        	{
        	TSRTPUtils::Write32( ptr,ROC());	
        	ptr+= KSRTPROCLength4;
        	}
        if (authTag)
        	{
        	Mem::Copy( ptr, authTag->Ptr(), authTag->Length());
        	}
        delete authTag;
        authTag=NULL;
        }    
    }
    
// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::DoAuthenticationL
// 
// ---------------------------------------------------------------------------
//
HBufC8* CSRTPCryptoHandlerSRTP::DoAuthenticationL(const TDesC8& aAuthenticatedArea,
											const TDesC8& aRoc)
    {                                     
    // calculate auth tag from the authenticated area..
    TUint authtaglen= CryptoParams().iSrtpAuthTagLen;
    // Note that RCCm3 work as Auth Null so taglen does not matter to RTP/RTCP
    if (aRoc.Length() && 
    	(CryptoParams().iSrtpAuthAlg ==EAuthRCCm1 ||
		CryptoParams().iSrtpAuthAlg == EAuthRCCm2 ))
    	{
    	authtaglen-= KSRTPAuthTagLength32 ;
    	}
    if 	((!aRoc.Length()) && 
    		( CryptoParams().iSrtpAuthAlg != EAuthNull &&
    		CryptoParams().iSrtpAuthAlg != EAuthHMAC_SHA1))
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
// CSRTPCryptoHandlerSRTP::UpdateROC()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::UpdateROC()
    {
    // conditional update of ROC at sender's side
    
    TReal remainder;
    Math::Mod(remainder, iCurrentPacket->SequenceNumber(), 0x10000-1);

    // update ROC when SEQ wraps 2^16,
    // with following exception:
    // when SEQ is 0, MOD result is also 0,
    // but we want remainder to be 0
    // only when SEQ == 65535     
    if (remainder == 0.0 && iCurrentPacket->SequenceNumber() != 0)
        {
         SetROC(ROC()+1);        
        }
    //Add number of packets sending    
    iNumPackets++;    
    }


// ---------------------------------------------------------------------------
// CSRTPCryptoHandlerSRTP::DeriveSessionKeysL()
// ---------------------------------------------------------------------------
//   
void CSRTPCryptoHandlerSRTP::DeriveSessionKeysL()
    {
    SRTP_DEBUG_DETAIL( "CSRTPCryptoHandlerSRTP::DeriveSessionKeysL()  Entry " );

    TBuf8<100> sessionKey_X;

    DeleteSessionKeys();        
    
    TUint64 r = TSRTPUtils::Cnt_r(iCurrentPacket->PacketIndex(), 
                         CryptoParams().iSrtpKeyDervRate);

    sessionKey_X.SetLength(100);
    
    // session encryption key derivation :
    sessionKey_X.FillZ();    
    // first count X value for the session encryption key
    // note that the output length is determined by master salt
    Count_X(r, KSRTPEncryptionLabel, KSRTPIndexLength, sessionKey_X);    
    // then add possible padding bits,
    // and call the current PRF_n function (AESCM)
    iSessionEncrKey = iKeyDeriver->PRF_128BitL(Context().MasterKey().EncrKeyLength(),
                                            Context().MasterKey().MasterKey(), 
                                            sessionKey_X);
    
    SRTP_DEBUG_DETAIL( "iSessionEncrKey as CipherKey" );
             
    SRTP_DEBUG_PACKET( *iSessionEncrKey );     
                                       
    // note that the result is already truncated to the user defined size by PRF function

    // session authentication key derivation :
    sessionKey_X.FillZ();
    // first count X value for the session authentication key
    // note that the output length is determined by master salt    
    Count_X(r, KSRTPAuthenticationLabel, KSRTPIndexLength, sessionKey_X);        
    // then add possible padding bits,
    // and call the current PRF_n function (AESCM)
    iSessionAuthKey = iKeyDeriver->PRF_128BitL(Context().MasterKey().AuthKeyLength(),
                                            Context().MasterKey().MasterKey(), 
                                            sessionKey_X);
    //iSessionAuthKey->Des().SetLength(Context()->MasterKey().AuthKeyLength());                                        

	SRTP_DEBUG_DETAIL( "iSessionAuthKey" );
	SRTP_DEBUG_PACKET( *iSessionAuthKey );    
    // note that the result is already truncated to the user defined size by PRF function

    // session salting key derivation :
    sessionKey_X.FillZ();
    // first count X value for the session salting key
    // note that the output length is determined by master salt    
    Count_X(r, KSRTPSaltingLabel, KSRTPIndexLength, sessionKey_X);        
    // then add possible padding bits,
    // and call the current PRF_n function (AESCM)
    iSessionSaltKey = iKeyDeriver->PRF_128BitL(Context().MasterSalt().SaltLength(),
                                            Context().MasterKey().MasterKey(), 
                                            sessionKey_X);
    SRTP_DEBUG_DETAIL( "iSessionSaltKey" );
    SRTP_DEBUG_PACKET( *iSessionSaltKey );    
                                              
    // note that the result is already truncated to the user defined size by PRF function
    
     iMasterDataUpdated = EFalse;    
 	}
    

// ---------------------------------------------------------------------------
// void CSRTPCryptoHandlerSRTP::ReplayProtectionL()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandlerSRTP::ReplayProtectionL()
    {
    //Index estamation
    if ( CryptoParams().iSrtpReplayProtection )
    	{
    	TInt delta =0;
	  	delta= (iCurrentPacket->PacketIndex()) - iPktIndex; //set as 0
		      
		//Index check
		User::LeaveIfError (ReplayCheck(delta));
		//if the index is inside the replay window and index does not appear in rdb
		//then Index add after authentication passed  
	    iReplayIndexDelta= delta;
    	}
    }

// ---------------------------------------------------------------------------
// TInt CSRTPCryptoHandlerSRTP::AddReplayIndex() 
// ---------------------------------------------------------------------------
//
void
CSRTPCryptoHandlerSRTP::AddReplayIndex() 
	{
	if ( CryptoParams().iSrtpReplayProtection )
		{
		
		if (iReplayIndexDelta > 0) 
			{
		    /* shift forward by delta */
		    //iPktIndex += iReplayIndexDelta;
		    iPktIndex = iCurrentPacket->PacketIndex();
		  	} 
		else 
		  	{
		    /* delta is in window, so flip bit in bitmap */
		    /* mark as seen */
		    TInt diff= -(iReplayIndexDelta);
		    iBitmap |= ((TUint64)1 << diff); 
		  	}
		}
	  /* note that we need not consider the case that delta == 0 */
	iNumPackets++;
	}
	
// ---------------------------------------------------------------------------
// TBool CSRTPCryptoHandlerSRTP::CarryROC
// ---------------------------------------------------------------------------
//
TBool CSRTPCryptoHandlerSRTP::CarryROC()
	{
	if ( CryptoParams().iSrtpAuthAlg == EAuthRCCm1 ||
		 CryptoParams().iSrtpAuthAlg == EAuthRCCm2 ||
		 CryptoParams().iSrtpAuthAlg == EAuthRCCm3 )
		{
		TReal remainder;
		// SEQ%R
		Math::Mod( remainder, iCurrentPacket->SequenceNumber(), 
									CryptoParams().iROCTransRate);
		//SEQ%R=0
		if ( !remainder )
			return ETrue;
		//SEQ%R!=0
		return EFalse;
		}
	return EFalse;
	}

// ---------------------------------------------------------------------------
// TInt64 CSRTPCryptoHandlerSRTP::CountSndIndxbySndROC
// ---------------------------------------------------------------------------
//	
TInt64 CSRTPCryptoHandlerSRTP::CountSndIndxbySndROC()
	{
	//This is called when receiving
	//i = 2^16 * ROC + SEQ.

	TInt64 indx = 65535 * ( iCurrentPacket->GetSenderROC() ) + 
					iCurrentPacket->SequenceNumber();
	
	return indx;
	}

// ---------------------------------------------------------------------------
// TBool CSRTPCryptoHandlerSRTP::IfAddAuthTag()
// ---------------------------------------------------------------------------
//	
TBool CSRTPCryptoHandlerSRTP::IfAddAuthTag()
	{
	if (( CryptoParams().iSrtpAuthAlg== EAuthRCCm1 &&
		!CarryROC() ) || CryptoParams().iSrtpAuthAlg == EAuthNull
		)
		{
		return EFalse;
		}
	return ETrue;
	}

