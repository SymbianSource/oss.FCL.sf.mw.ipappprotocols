
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
#include    "SIPExIMDialog.h"
#include    "SIPEx.hrh"
#include    <sipex.rsg>
#include    <eikenv.h>
#include    <uri8.h>


// -----------------------------------------------------------------------------
// CSIPExIMDialog::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
CSIPExIMDialog* CSIPExIMDialog::NewL( TDes& aAddress, TDes& aMsg )
    {
    CSIPExIMDialog* self = NewLC( aAddress, aMsg );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExIMDialog::NewLC
// Static constructor. On return the instance is left to the CleanupStack.
// -----------------------------------------------------------------------------
//
CSIPExIMDialog* CSIPExIMDialog::NewLC( TDes& aAddress, TDes& aMsg )
    {
    CSIPExIMDialog* self = new (ELeave) CSIPExIMDialog( aAddress, aMsg );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExIMDialog::CSIPExIMDialog
// C++ default constructor. Initializes the member variables with default values
// from client.
// -----------------------------------------------------------------------------
//
CSIPExIMDialog::CSIPExIMDialog( TDes& aAddress, TDes& aMsg )
    : iAddress( aAddress ), iMsg( aMsg )
    {
    }

// -----------------------------------------------------------------------------
// CSIPExIMDialog::~CSIPExIMDialog
// Destructor
// -----------------------------------------------------------------------------
//
CSIPExIMDialog::~CSIPExIMDialog()
    {
    }

// -----------------------------------------------------------------------------
// CSIPExIMDialog::ConstructL
// Symbian 2nd phase constructor that might leave.
// -----------------------------------------------------------------------------
//
void CSIPExIMDialog::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSIPExIMDialog::PreLayoutDynInitL
// From CEikDialog. Called before drawing the dialog.
// If address variable has used earlier use that value as default.
// -----------------------------------------------------------------------------
//
void CSIPExIMDialog::PreLayoutDynInitL()
    {
    if( iAddress.Length() > 0 )
        {
        ( static_cast<CEikEdwin*>( 
            Control( ESIPExIMAddressLineId ) ) )->SetTextL( &iAddress );
        }
    }

// -----------------------------------------------------------------------------
// CSIPExIMDialog::OkToExitL
// From CEikDialog. Called when user presses dialog's button.
// Validates the given address and if not correct notifies user with info
// message. The length of the message is limited in resource file to 256.
// The limitation is not in SIP implementation.
// -----------------------------------------------------------------------------
//
TBool CSIPExIMDialog::OkToExitL(TInt aKeyCode)
    {   
    TBool isOk( ETrue );
    
    if( aKeyCode == EEikBidOk )
        {
        ( static_cast<CEikEdwin*>( 
            Control( ESIPExIMMessageLineId ) ) )->GetText( iMsg );
        ( static_cast<CEikEdwin*>( 
            Control( ESIPExIMAddressLineId ) ) )->GetText( iAddress );

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
// CSIPExIMDialog::AddressValid
// Checks if user typed address is valid sip address.
// -----------------------------------------------------------------------------
//    
TBool CSIPExIMDialog::AddressValid( const TDesC8& aSipAddr )
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
