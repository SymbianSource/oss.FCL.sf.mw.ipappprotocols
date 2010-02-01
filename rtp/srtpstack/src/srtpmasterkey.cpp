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
#include "srtpmasterkey.h"
#include "srtpmastersalt.h"
#include "srtpcryptocontext.h"
#include "srtpdef.h"
#include "srtputils.h"



// ---------------------------------------------------------------------------
// Two-phased constructor. Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPMasterKey* CSRTPMasterKey::NewL( const TDesC8& aKey, 
                       const TDesC8& aMKI, 
                       TUint aN_E, 
                       TUint aN_A )
    {
    
    
    CSRTPMasterKey* self = CSRTPMasterKey::NewLC( aKey,
                                                  aMKI,
                                                  aN_E, 
                                                  aN_A);
    CleanupStack::Pop( self );        
    return self;
    }	

// ---------------------------------------------------------------------------
// Two-phased constructor. Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPMasterKey* CSRTPMasterKey::NewLC( const TDesC8& aKey, 
                       const TDesC8& aMKI, 
                       TUint aN_E, 
                       TUint aN_A )
    {
                                          
    CSRTPMasterKey* self = new( ELeave )CSRTPMasterKey ( aN_E, 
                                                         aN_A);                                                
    CleanupStack::PushL( self );    
    self->ConstructL( aKey, aMKI );
    return self;
    }	

// ---------------------------------------------------------------------------
// CSRTPMasterKey::CSRTPMasterKey
// ---------------------------------------------------------------------------
//
CSRTPMasterKey::CSRTPMasterKey( TUint aN_E, 
                                TUint aN_A) :
            iN_E( aN_E ),
            iN_A( aN_A )
            {
            }

// -----------------------------------------------------------------------------
// CSRTPMasterKey::ConstructL
// -----------------------------------------------------------------------------
//
void CSRTPMasterKey::ConstructL(  const TDesC8& aKey, 
                                  const TDesC8& aMKI )
	{
    iKey = aKey.AllocL();
    if(aKey.Length()>16)
	    {
	    TSRTPUtils::HexString( *iKey );	
	    }
    iMKI = aMKI.AllocL();
    iMKI->Des().SetLength(aMKI.Length());
   	}
	
// ---------------------------------------------------------------------------
// CSRTPMasterKey::~CSRTPMasterKey
// ---------------------------------------------------------------------------
//
CSRTPMasterKey::~CSRTPMasterKey( )
           
    {
    delete iKey;
    delete iMKI;
    }


// ---------------------------------------------------------------------------
// CSRTPMasterKey::MasterKey
// ---------------------------------------------------------------------------
//
EXPORT_C  const TDesC8& CSRTPMasterKey::MasterKey() const
    {
    return *iKey;
    }
        

// ---------------------------------------------------------------------------
// CCSRTPMasterKey::MKI
// ---------------------------------------------------------------------------
//
EXPORT_C  const TDesC8& CSRTPMasterKey::MKI() const
    {
    return *iMKI;
    };
                            
// ---------------------------------------------------------------------------
// CSRTPMasterKey::EncrKeyLength
// ---------------------------------------------------------------------------
//
EXPORT_C  TUint CSRTPMasterKey::EncrKeyLength() const
    {
    return iN_E;
    }


// ---------------------------------------------------------------------------
// CSRTPMasterKey::AuthKeyLength
// ---------------------------------------------------------------------------
//
EXPORT_C  TUint CSRTPMasterKey::AuthKeyLength() const
    {
    return iN_A;
    }


// ---------------------------------------------------------------------------
// CSRTPMasterKey::operator=
// ---------------------------------------------------------------------------
//
 const CSRTPMasterKey& CSRTPMasterKey::operator=(const CSRTPMasterKey& aKey)
    {
    return aKey;
    }

// ---------------------------------------------------------------------------
// CSRTPMasterKey::Valid()
// ---------------------------------------------------------------------------
//       
EXPORT_C  TBool CSRTPMasterKey::Valid() const
    {    
    if (iKey->Length() != KSRTPDefaultMasterKeyLength/8)
        {
        return EFalse;
        }       
    
    if (iN_E != KSRTPDefSessionEncryptionKeyLength)
        {
        return EFalse;
        }   
        
    if (iN_A != KSRTPDefSessionAuthenticationKeyLength)
        {
        return EFalse;
        }
        
    return ETrue;
    }
    



