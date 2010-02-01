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
EXPORT_C CSRTPMasterSalt* CSRTPMasterSalt::NewL( const TDesC8& aKey, TUint aN_S )
    {
    CSRTPMasterSalt* self = CSRTPMasterSalt::NewLC( aKey, aN_S );
    CleanupStack::Pop( self );        
    return self;
    }	

// ---------------------------------------------------------------------------
// Two-phased constructor. Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPMasterSalt* CSRTPMasterSalt::NewLC( const TDesC8& aKey, TUint aN_S )
    {
                                          
    CSRTPMasterSalt* self = new( ELeave )CSRTPMasterSalt ( aN_S );
                                                                
                                                      
    CleanupStack::PushL( self );    
    self->ConstructL( aKey );
    return self;
    }	

// ---------------------------------------------------------------------------
// CSRTPMasterSalt::CSRTPMasterSalt
// ---------------------------------------------------------------------------
//
CSRTPMasterSalt::CSRTPMasterSalt( TUint aN_S ) :
            iN_S( aN_S )
            {
            }

// -----------------------------------------------------------------------------
// CSRTPMasterSalt::ConstructL
// -----------------------------------------------------------------------------
//
void CSRTPMasterSalt::ConstructL(  const TDesC8& aKey )
	{
    iKey = aKey.AllocL();
    if(aKey.Length()>14)
    	{
    	TSRTPUtils::HexString( *iKey );	
    	}
	}
	
// ---------------------------------------------------------------------------
// CSRTPMasterSalt::~CSRTPMasterSalt
// ---------------------------------------------------------------------------
//
CSRTPMasterSalt::~CSRTPMasterSalt( )
           
    {
    delete iKey;
    }


// ---------------------------------------------------------------------------
// CSRTPMasterSalt::MasterSalt
// ---------------------------------------------------------------------------
//
EXPORT_C  const TDesC8& CSRTPMasterSalt::MasterSalt() const
    {
    return *iKey;
    }
                            
// ---------------------------------------------------------------------------
// CSRTPMasterSalt::SaltLength
// ---------------------------------------------------------------------------
//
EXPORT_C  TUint CSRTPMasterSalt::SaltLength() const
    {
    return iN_S;
    }


// ---------------------------------------------------------------------------
// CSRTPMasterSalt::operator=
// ---------------------------------------------------------------------------
//
 const CSRTPMasterSalt& CSRTPMasterSalt::operator=(const CSRTPMasterSalt& aSalt)
    {
    return aSalt;
    }
    
// ---------------------------------------------------------------------------
// CSRTPMasterSalt::Valid()
// ---------------------------------------------------------------------------
//       
EXPORT_C  TBool CSRTPMasterSalt::Valid() const
    {    
    if (iKey->Length() != KSRTPDefaultMasterSaltLength/8)
        {
        return EFalse;
        }       
    
    if (iN_S != KSRTPDefSessionSaltingKeyLength)
        {
        return EFalse;
        }                   
            
    return ETrue;
    }    

