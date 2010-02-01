// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : CSocketOpt.cpp
// Part of     : ConnectionMgr
// See class definition below.
// Version     : SIP/4.0
//



#include "CSocketOpt.h"
#include "SipLogs.h"
 
// -----------------------------------------------------------------------------
// CSocketOpt::NewLC
// -----------------------------------------------------------------------------
//
CSocketOpt* CSocketOpt::NewLC(
    TUint aOptionName,
	TUint aOptionLevel,
	TInt aOption )
    {
    CSocketOpt* self = 
        new ( ELeave ) CSocketOpt( aOptionName, aOptionLevel, aOption );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
      
// -----------------------------------------------------------------------------
// CSocketOpt::NewLC
// -----------------------------------------------------------------------------
//
CSocketOpt* CSocketOpt::NewLC(
    TUint aOptionName,
	TUint aOptionLevel,
	const TDesC8& aOption )
    {
    CSocketOpt* self = 
        new ( ELeave ) CSocketOpt( aOptionName, aOptionLevel );
    CleanupStack::PushL( self );
    self->ConstructL( aOption );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CSocketOpt::~CSocketOpt
// -----------------------------------------------------------------------------
//
CSocketOpt::~CSocketOpt()
    {
    delete iOptionBuf;
    }

// -----------------------------------------------------------------------------
// CSocketOpt::ConstructL
// -----------------------------------------------------------------------------
//
void CSocketOpt::ConstructL( const TDesC8& aOption )
    {
    if ( aOption != KNullDesC8 )
        {
        iOptionBuf = aOption.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// CSocketOpt::CSocketOpt
// -----------------------------------------------------------------------------
//    
CSocketOpt::CSocketOpt(
    TUint aOptionName,
	TUint aOptionLevel,
	TInt aOption ) :
	iOptionName( aOptionName ),
	iOptionLevel( aOptionLevel ),
	iOption( aOption )
    {
    }

// -----------------------------------------------------------------------------
// CSocketOpt::SetOpt
// -----------------------------------------------------------------------------
//         
TUint CSocketOpt::OptionName() const
    {
    return iOptionName;
    }
    
// -----------------------------------------------------------------------------
// CSocketOpt::SetOptL
// -----------------------------------------------------------------------------
//     
CSocketOpt* CSocketOpt::SetOptL( RSocket& aSocket )
    {
    __SIP_LOG( "CSocketOpt::SetOptL" )
    
    CSocketOpt* previousOpt( NULL );
    TInt err( KErrNone );
    
    if ( iOptionBuf )
        {
        // Errors are ignored
        HBufC8* optionBuf = HBufC8::NewLC( iOptionBuf->Des().MaxLength() * 2 );
        TPtr8 optionBufPtr = optionBuf->Des();
        err = aSocket.GetOpt( iOptionName, iOptionLevel, optionBufPtr );
        if ( !err )
            {
            previousOpt = CSocketOpt::NewLC( iOptionName, iOptionLevel, *optionBuf );
            CleanupStack::Pop( previousOpt );
            }
        __SIP_INT_LOG1( "CSocketOpt::SetOptL, previous opt read, err:", err )
        CleanupStack::PopAndDestroy( optionBuf );
        err = aSocket.SetOpt( iOptionName, iOptionLevel, *iOptionBuf );
        }
    else
        {
        // Errors are ignored
        TInt optionVal( 0 );
        err = aSocket.GetOpt( iOptionName, iOptionLevel, optionVal );
        if ( !err )
            {
            previousOpt = CSocketOpt::NewLC( iOptionName, iOptionLevel, optionVal );
            CleanupStack::Pop( previousOpt );
            }
        __SIP_INT_LOG1( "CSocketOpt::SetOptL, previous opt read, err:", err )
        err = aSocket.SetOpt( iOptionName, iOptionLevel, iOption );
        }  
        
    __SIP_INT_LOG1( "CSocketOpt::SetOptL, exit, err:", err )
    
    return previousOpt;
    }
    
// end of file
