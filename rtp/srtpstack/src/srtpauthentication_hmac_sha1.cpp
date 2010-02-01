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
* Description:    Provides class for authentication with HMAC-SHA1 algorithm.
*
*/




#include "srtpauthentication_hmac_sha1.h" 
#include "srtpaesctrcrypto.h"
#include "hash.h"
#include "srtpdef.h"
#include "srtputils.h"

#define DES_AS_8_BIT(str) (TPtrC8((TText8*)((str).Ptr()), (str).Size()))
    
// ---------------------------------------------------------------------------
// CSRTPAuthentication_HMAC_SHA1::NewL
// ---------------------------------------------------------------------------
//
CSRTPAuthentication_HMAC_SHA1* CSRTPAuthentication_HMAC_SHA1::NewL()
    {
    CSRTPAuthentication_HMAC_SHA1* self = CSRTPAuthentication_HMAC_SHA1::NewLC();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// CSRTPAuthentication_HMAC_SHA1::NewLC
// ---------------------------------------------------------------------------
//
CSRTPAuthentication_HMAC_SHA1* CSRTPAuthentication_HMAC_SHA1::NewLC()
    {
    CSRTPAuthentication_HMAC_SHA1* self = new( ELeave ) CSRTPAuthentication_HMAC_SHA1();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// SRTPAuthentication_HMAC_SHA1::AuthenticateL
// -----------------------------------------------------------------------------
//
HBufC8* CSRTPAuthentication_HMAC_SHA1::AuthenticateL(TUint aBitLength,
                                                    const TDesC8& aKey, 
                                                    const TDesC8& aAuthPortion,
                                                    const TDesC8& aRoc)
    {        
    SRTP_DEBUG_DETAIL( "CSRTPAuthentication_HMAC_SHA1::AuthenticateL Entry" );
    
    TBuf8<20> buf;    
	

    if (aBitLength != KSRTPAuthTagLength80 && aBitLength != KSRTPAuthTagLength32)
        {
        SRTP_DEBUG_TUINT_VALUE( "aBitLength is not valid", aBitLength );
	    
        User::Leave(KErrArgument);
        }
	
    // reserve size for 160 bit tag which seems to be the default        
	HBufC8* result = HBufC8::NewL(20);  
	TPtr8 ptrOutputBuff = result->Des();	
	
    CleanupStack::PushL(result);

    CSHA1* sha1 = CSHA1::NewL(); 
    CleanupStack::PushL(sha1);
    
    CHMAC *hmac = CHMAC::NewL(aKey, sha1);        
    CleanupStack::Pop(sha1);
    CleanupStack::Pop(result);
    
    SRTP_DEBUG_TINT_VALUE( "HMAC INPUT and INPUT Length is ", aAuthPortion.Length() );
    SRTP_DEBUG_TINT_VALUE( "HMAC INPUT and INPUT Size is ", aAuthPortion.Size());     
    SRTP_DEBUG_PACKET ( DES_AS_8_BIT(aAuthPortion) );    
    
    if(aRoc.Length())
    	{
  		hmac->Update(DES_AS_8_BIT(aAuthPortion));
    	buf.Copy(hmac->Final(DES_AS_8_BIT(aRoc))); 	
    	}
    else
    	{
    	buf.Copy(hmac->Final(DES_AS_8_BIT(aAuthPortion)));
    	}
    *result = buf;    
    ptrOutputBuff.SetLength(aBitLength/8);
    
    delete hmac;
    
   	SRTP_DEBUG_DETAIL( "HMAC caculated Authentication tag");
    SRTP_DEBUG_PACKET( *result );    
    SRTP_DEBUG_DETAIL( "CSRTPAuthentication_HMAC_SHA1::AuthenticateL Exit" );
                            
    return result;            
    }
    
    
// ---------------------------------------------------------------------------
// CSRTPAuthentication_HMAC_SHA1::ConstructL
// ---------------------------------------------------------------------------
//
void CSRTPAuthentication_HMAC_SHA1::ConstructL()
    {    
    
    }

// ---------------------------------------------------------------------------
// CSRTPAuthentication_HMAC_SHA1::~CSRTPAuthentication_HMAC_SHA1
// ---------------------------------------------------------------------------
//
CSRTPAuthentication_HMAC_SHA1::~CSRTPAuthentication_HMAC_SHA1()
    {    
    }

// ---------------------------------------------------------------------------
// CSRTPAuthentication_HMAC_SHA1::CSRTPAuthentication_HMAC_SHA1
// ---------------------------------------------------------------------------
//
CSRTPAuthentication_HMAC_SHA1::CSRTPAuthentication_HMAC_SHA1()
    {    
        
    }

