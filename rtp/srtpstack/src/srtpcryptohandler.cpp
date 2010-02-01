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
#include "srtpcryptocontext.h"
#include "srtputils.h"
#include "srtpstream.h"
#include "srtpaesctrcrypto.h"
#include "srtpkeyderivation_aescm128.h"
#include "srtpcipher_aescm128.h"
#include "srtpcipher_null.h"
#include "srtpauthentication_hmac_sha1.h"
#include "srtpauthentication_null.h"
#include "srtpauthentication_rcc.h"
#include "srtppacket.h"
#include "srtpmasterkey.h"
#include "srtpmastersalt.h"

// CONSTANTS
const TInt CSRTPCryptoHandler::iHandlerOffset = _FOFF( CSRTPCryptoHandler, iHandlerLink );

// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::CSrtpCryptoHandler
// ---------------------------------------------------------------------------
//
CSRTPCryptoHandler::CSRTPCryptoHandler( CSRTPStream& aStream ) :
        iStream(aStream),
        iCurrentPacket(NULL),
        iKeyDeriver(NULL),
        iAuthenticator(NULL),
        iCipher(NULL),
        iMasterDataUpdated(EFalse),
        iSessionEncrKey(NULL),
        iSessionAuthKey(NULL),
        iSessionSaltKey(NULL),
        iReKey(EFalse),
        iBitmap(NULL),
        iNumPackets(NULL)
    {
        
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::~CSrtpCryptoHandler
// ---------------------------------------------------------------------------
//
CSRTPCryptoHandler::~CSRTPCryptoHandler( )
    { 
    if (&Context())
    	{
    	Context().RemoveCryptoChangeObserver(this);	
    	}
    
    delete iCurrentPacket; iCurrentPacket=NULL;
    delete iKeyDeriver; iKeyDeriver=NULL;
    delete iCipher; iCipher=NULL;
    delete iAuthenticator; iAuthenticator=NULL;        
    
    DeleteSessionKeys();
    }

// ---------------------------------------------------------------------------
// void CSRTPCryptoHandler::SRTPMasterKeyChanged()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandler::SRTPMasterKeyChanged()
    {
     iMasterDataUpdated = ETrue;
    //RFC 3711 8.1
     iReKey= EFalse;
    }
        
// ---------------------------------------------------------------------------
// void CSRTPCryptoHandler::SRTPMasterSaltChanged()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandler::SRTPMasterSaltChanged()
    {
     iMasterDataUpdated = ETrue;   
         
    }
    

// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandler::ConstructL( )
    {  
    if (!Context().Valid())
    	{
    	User::Leave(KErrArgument);
    	}
    Context().AddCryptoChangeObserver(this);	
    
    iKeyDeriver = CSRTPKeyDerivation_AESCM128::NewL();
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::SetEncAndAlg ();
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandler::SetEncAndAuthL (TSRTPEncAlg aEngAlg, 
										TSRTPAuthAlg aAuthAlg)
	{
	delete iCipher; iCipher=NULL;
    delete iAuthenticator; iAuthenticator=NULL;  
	switch (aEngAlg)
	    {
	    case EEncAES_CM:
	        {
	        iCipher = CSRTPCipherAESCM128::NewL();        
	        break;                
	        }      
	    case ENullAlg:
	        {
	        iCipher = CSRTPCipherNULL::NewL();        
	        break;                
	        }      
	    default:          
	        {
	        User::Leave(KErrNotSupported);
	        }
	    }  

	switch (aAuthAlg)
	    {
	    case EAuthHMAC_SHA1:
	   
	        {
	        iAuthenticator = CSRTPAuthentication_HMAC_SHA1::NewL();        
	        break;                
	        }     
	    case EAuthNull:  
	    case EAuthRCCm3:
	    	{
	        iAuthenticator = CSRTPAuthentication_NULL::NewL();        
	        break;                
	        } 
	    case EAuthRCCm1: 
	    case EAuthRCCm2: 	                 
	       	{
	        iAuthenticator = CSrtpAuthentication_RCC::NewL();        
	        break;                
	        } 

	    default:          
	        {
	        User::Leave(KErrNotSupported);
	        }
	    }  
	}
	
// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::Context()
// ---------------------------------------------------------------------------
//
CSRTPCryptoContext& CSRTPCryptoHandler::Context()
    {
    return iStream.GetCryptoContext();
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::CryptoParams()
// ---------------------------------------------------------------------------
//
const TSrtpCryptoParams& CSRTPCryptoHandler::CryptoParams()
    {
    return Context().CryptoParams();
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::DeleteSessionKeys()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandler::DeleteSessionKeys()
    {
     delete iSessionEncrKey;
     delete iSessionAuthKey;
     delete iSessionSaltKey;  
     iSessionEncrKey = NULL;
     iSessionAuthKey = NULL;
     iSessionSaltKey = NULL;
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::Count_X()
// ---------------------------------------------------------------------------
//
void CSRTPCryptoHandler::Count_X(TUint64 a_R,
                                TInt8 aLabel, 
                                const TUint64 aIndexLength, 
                                TDes8 &aRes)
    {
        
    TUint64 key_id = TSRTPUtils::Cnt_key_id(aLabel, a_R, aIndexLength);
    
    TSRTPUtils::Cnt_x(key_id, aRes, Context().MasterSalt().MasterSalt());
    
    } 



// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::SetROC()
// 
// ---------------------------------------------------------------------------
//

void CSRTPCryptoHandler::SetROC(  TUint32 aROC )
    {
    iStream.SetROC(aROC);
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::ROC
// 
// ---------------------------------------------------------------------------
//
TUint32 CSRTPCryptoHandler::ROC() const
    {
    return iStream.ROC();
    }


// ---------------------------------------------------------------------------
// CSRTPCryptoHandler::SSRC
// 
// ---------------------------------------------------------------------------
//
TUint CSRTPCryptoHandler::SSRC() const
    {
    return iStream.SSRC();
    }



// ---------------------------------------------------------------------------
// TInt CSRTPCryptoHandler::ReplayCheck()
// ---------------------------------------------------------------------------
//
TInt CSRTPCryptoHandler::ReplayCheck(TInt aDelta) 
	{
	TInt replayWSH= KReplayWindowSize;
	if (CryptoParams().iReplayWindowSizeHint!=replayWSH)
		{
		replayWSH= CryptoParams().iReplayWindowSizeHint;
		}
	//Remember to initial bitmask at the first time
  	//if aDelta ==0 it might be just re-sent	
	if (aDelta > 0) 
		{                
		/* new larger sequence number */
        
        if (aDelta < replayWSH) 
        	{  /* In window */
            iBitmap <<= aDelta;
            iBitmap |= 1;  /* set bit for this packet */
        	} else iBitmap = 1; /* This packet has a "way larger" */
        
        return KErrNone;   /* larger is good */
    	}
    TInt diff= -(aDelta);
    if (diff >= replayWSH) return KErrArgument; /* too old or wrapped */

    if (iBitmap & ((TUint64)1 << diff)) return KErrArgument; /* already seen */
    //if none of above then will mark as seen in the AddReplayIndex() function             
    return KErrNone;  /* out of order but good */

	}
