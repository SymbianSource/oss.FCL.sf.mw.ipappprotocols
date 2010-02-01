
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



// INCLUDES
#include    "SIPExAddrQueryDlg.h"
#include    "SIPEx.hrh"
#include    <sipex.rsg>
#include    <eikenv.h>
#include    <uri8.h>

// -----------------------------------------------------------------------------
// CSIPExAddrQueryDlg::NewL
// Creates the class instance and returns it.
// -----------------------------------------------------------------------------
//
CSIPExAddrQueryDlg* CSIPExAddrQueryDlg::NewL( TDes& aAddr )
    {
    CSIPExAddrQueryDlg* self = NewLC( aAddr );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExAddrQueryDlg::NewLC
// Creates the class instance and returns it.
// On return the instance is left to the CleanupStack.
// -----------------------------------------------------------------------------
//
CSIPExAddrQueryDlg* CSIPExAddrQueryDlg::NewLC( TDes& aAddr )
    {
    CSIPExAddrQueryDlg* self = new (ELeave) CSIPExAddrQueryDlg( aAddr );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExAddrQueryDlg::CSIPExAddrQueryDlg
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CSIPExAddrQueryDlg::CSIPExAddrQueryDlg( TDes& aAddr )
    : iAddress( aAddr )
    {
    }

// -----------------------------------------------------------------------------
// CSIPExAddrQueryDlg::~CSIPExAddrQueryDlg
// Desctructor.
// -----------------------------------------------------------------------------
//
CSIPExAddrQueryDlg::~CSIPExAddrQueryDlg()
    {
    }

// -----------------------------------------------------------------------------
// CSIPExAddrQueryDlg::ConstructL
// Symbian 2nd phase constructor that might leave.
// -----------------------------------------------------------------------------
//
void CSIPExAddrQueryDlg::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSIPExAddrQueryDlg::PreLayoutDynInitL
// From CEikDialog. Initializes the dialog's text control with default address
// if available.
// -----------------------------------------------------------------------------
//
void CSIPExAddrQueryDlg::PreLayoutDynInitL()
    {
    if ( iAddress.Length() > 0 )
        {
        ( static_cast<CEikEdwin*>( 
            Control( ESIPExInviteAddressId ) ) )->SetTextL( &iAddress );
        }
    }

// -----------------------------------------------------------------------------
// CSIPExAddrQueryDlg::OkToExitL
// From CEikDialog.
// Validates the given address and if not correct notifies user with info
// message.
// -----------------------------------------------------------------------------
//
TBool CSIPExAddrQueryDlg::OkToExitL( TInt aKeyCode )
    {
    TBool isOk( ETrue );
    
    if ( aKeyCode == EEikBidOk )
        {
        ( static_cast< CEikEdwin* >( 
            Control( ESIPExInviteAddressId ) ) )->GetText( iAddress );

        // Check the validity of the given address
        TInt err( KErrGeneral );
        
        HBufC8* address = HBufC8::NewLC( iAddress.Length() );
        address->Des().Copy( iAddress );
        
        if ( !AddressValid( *address ) )
            {
            HBufC* txt = 
                    iEikonEnv->AllocReadResourceLC( R_ERROR_IN_ADDRESS_TXT );
            CEikonEnv::Static()->InfoMsg( txt->Des() );
            CleanupStack::PopAndDestroy( txt );
            isOk = EFalse;
            }
        
        CleanupStack::PopAndDestroy( address );
        }

    return isOk;
    }

// -----------------------------------------------------------------------------
// CSIPExAddrQueryDlg::AddressValid
// Checks if user typed address is valid sip address.
// -----------------------------------------------------------------------------
//    
TBool CSIPExAddrQueryDlg::AddressValid( const TDesC8& aSipAddr )
    {
    _LIT8( KTypeSIP, "sip" );
    
    TUriParser8 parser;
    User::LeaveIfError( parser.Parse( aSipAddr ) ); 
    CUri8* uri8 = CUri8::NewLC( parser );
    
    TBool valid( ETrue );
    
    if ( uri8->Uri().Extract( EUriScheme ).CompareF( KTypeSIP() ) != KErrNone )
        {
        valid = EFalse;
        }
    if ( uri8->Uri().Extract( EUriUserinfo ) == KNullDesC8 )
        {
        valid = EFalse;
        }
    if ( uri8->Uri().Extract( EUriHost ) == KNullDesC8 )
        {
        valid = EFalse;
        }
    CleanupStack::PopAndDestroy( uri8 );
        
    return valid;        
    }

// End of file
