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




#include "srtpauthentication_rcc.h" 
#include "srtpaesctrcrypto.h"
#include "hash.h"
#include "srtpdef.h"
#include "srtputils.h"

#define DES_AS_8_BIT(str) (TPtrC8((TText8*)((str).Ptr()), (str).Size()))
    
// ---------------------------------------------------------------------------
// CSrtpAuthentication_RCC::NewL
// ---------------------------------------------------------------------------
//
CSrtpAuthentication_RCC* CSrtpAuthentication_RCC::NewL()
    {
    CSrtpAuthentication_RCC* self = CSrtpAuthentication_RCC::NewLC();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// CSrtpAuthentication_RCC::NewLC
// ---------------------------------------------------------------------------
//
CSrtpAuthentication_RCC* CSrtpAuthentication_RCC::NewLC()
    {
    CSrtpAuthentication_RCC* self = new( ELeave ) CSrtpAuthentication_RCC();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// SrtpAuthentication_RCC::AuthenticateL
// -----------------------------------------------------------------------------
//
HBufC8* CSrtpAuthentication_RCC::AuthenticateL(TUint aBitLength,
                                                    const TDesC8& aKey, 
                                                    const TDesC8& aAuthPortion,
                                                    const TDesC8& aRoc)
    {        
    SRTP_DEBUG_DETAIL( "CSrtpAuthentication_RCC::AuthenticateL Entry" );

    TBuf8<20> buf;    
	

    if (aBitLength != KSRTPAuthTagLength80 )
        {
        SRTP_DEBUG_TUINT_VALUE ("aBitLength is not valid and the length is",aBitLength );
	    
        User::Leave(KErrArgument);
        }
	
    // reserve size for 160 bit tag which seems to be the default        
	HBufC8* result = HBufC8::NewL(20);  
	TPtr8 ptrOutputBuff = result->Des();	
	
    CleanupStack::PushL(result);

    if ( !iHmac || !iKey || (*iKey != aKey) )
        {
        CreateHmacL(aKey);
        }
   
    CleanupStack::Pop(result);
    SRTP_DEBUG_TINT_VALUE( "HMAC INPUT and INPUT Length is ", aAuthPortion.Length());
    SRTP_DEBUG_TINT_VALUE( "HMAC INPUT and INPUT Size is ", aAuthPortion.Size());     
    SRTP_DEBUG_PACKET( DES_AS_8_BIT(aAuthPortion) );    
              
    if(aRoc.Length())
    	{
  		iHmac->Update(DES_AS_8_BIT(aAuthPortion));
    	buf.Copy(iHmac->Final(DES_AS_8_BIT(aRoc))); 	
    	}
    else
    	{
    	buf.Copy(iHmac->Final(DES_AS_8_BIT(aAuthPortion)));
    	}
    *result = buf;    
    ptrOutputBuff.SetLength(aBitLength/8);
    
   	SRTP_DEBUG_DETAIL( "HMAC caculated Authentication tag" );
   	SRTP_DEBUG_PACKET( *result );    
    
    SRTP_DEBUG_DETAIL( "CSrtpAuthentication_RCC::AuthenticateL Exit" );
                                
    return result;            
    }
    
    
// ---------------------------------------------------------------------------
// CSrtpAuthentication_RCC::ConstructL
// ---------------------------------------------------------------------------
//
void CSrtpAuthentication_RCC::ConstructL()
    {    
    
    }

// ---------------------------------------------------------------------------
// CSrtpAuthentication_RCC::~CSrtpAuthentication_RCC
// ---------------------------------------------------------------------------
//
CSrtpAuthentication_RCC::~CSrtpAuthentication_RCC()
    {    
    delete iHmac;
    delete iKey;
    }

// ---------------------------------------------------------------------------
// CSrtpAuthentication_RCC::CSrtpAuthentication_RCC
// ---------------------------------------------------------------------------
//
CSrtpAuthentication_RCC::CSrtpAuthentication_RCC()
    {    
        
    }

// ---------------------------------------------------------------------------
// CSrtpAuthentication_RCC::CreateHmacL
// ---------------------------------------------------------------------------
//
void CSrtpAuthentication_RCC::CreateHmacL(const TDesC8& aKey)
    {
    delete iHmac;
    iHmac = 0;
    delete iKey;
    iKey = 0;
    iKey = aKey.AllocL();
    CSHA1* sha1 = CSHA1::NewL(); 
    CleanupStack::PushL(sha1);
    iHmac = CHMAC::NewL(*iKey, sha1);        
    CleanupStack::Pop(sha1);
    }

