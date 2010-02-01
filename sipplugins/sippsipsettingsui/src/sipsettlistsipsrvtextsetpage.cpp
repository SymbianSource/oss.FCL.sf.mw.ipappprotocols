/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Text setting page with a validation of the entered text
*
*/


// INCLUDE FILES

#include    "sipsettlistsipsrvtextsetpage.h"
#include    <aknnotewrappers.h>
#include    <eikedwin.h>
#include    <avkon.rsg>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    "gssippluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvTextSetPage::CSIPSettListSIPSrvTextSetPage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPSrvTextSetPage::CSIPSettListSIPSrvTextSetPage(
    TInt aResourceID, 
    TGSSIPValidatorWrapper aValidator,
    TDes& aText, 
    TInt aTextSettingPageFlags ) :
    CAknTextSettingPage( aResourceID, aText, aTextSettingPageFlags ),
    iValidator( NULL, NULL ),
    iID( aResourceID )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvTextSetPage::CSIPSettListSIPSrvTextSetPage" )
    iValidator = aValidator;
    }
    
// Destructor
CSIPSettListSIPSrvTextSetPage::~CSIPSettListSIPSrvTextSetPage()
    {  
    __GSLOGSTRING("CSIPSettListSIPSrvTextSetPage::~CSIPSettListSIPSrvTextSetPage" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvTextSetPage::OkToExitL
// Called to validate input before closing the page
// -----------------------------------------------------------------------------
//
TBool CSIPSettListSIPSrvTextSetPage::OkToExitL( TBool aAccept )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvTextSetPage::OkToExitL" )
    TBool ret( EFalse );
    
    if ( !aAccept )
        {
        // User pressed cancel - return back to original text and go away
        RestoreOriginalSettingL();
        return ETrue;
        }

    // Ask from the validation method how it feels about this
    UpdateTextL();
    
    TBool exit = EFalse;
    TRAPD( err, exit = iValidator.ValidateL() );
    // No use of handling that error code - it just means that things weren't
    // validated
    
    if ( exit && err == KErrNone )
        {
        // Everything OK, save setting and exit page
        AcceptSettingL();
        ret = ETrue;
        }
    else
        {
        // No good, show error message and let user try again
        HBufC* errorTxt = ErrorMsgTxtLC();        
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
        note->ExecuteLD( errorTxt->Des() );
        CleanupStack::PopAndDestroy( errorTxt );
        TextControl()->SelectAllL();
        ret = EFalse;
        }
       
    return ret;    
    }


// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvTextSetPage::ErrorMsgTxtLC
// Reads error messge text to buffer and returns it
// -----------------------------------------------------------------------------
//
HBufC* CSIPSettListSIPSrvTextSetPage::ErrorMsgTxtLC()
    {
    __GSLOGSTRING("CSIPSettListSIPSrvTextSetPage::ErrorMsgTxtLC" )
    HBufC* errorMsgTxt = NULL;
    TInt id = 0;

    switch( iID )
        {
        case R_SIP_USERNAME_PAGE:
            id = R_QTN_SIP_ERROR_INVALID_USER_NAME;
            break;

        case R_SIP_SERVER_ADDR_PAGE:
            id = R_QTN_SIP_ERROR_PROXY_ADDRESS;
            break;

        case R_SIP_REG_SERVER_ADDR_PAGE:
            id = R_QTN_SIP_ERROR_REGISTRAR_ADDRESS;
            break;

        default:
            User::Leave( KErrNotFound );
            break;
        }

    errorMsgTxt = iEikonEnv->AllocReadResourceLC( id );
    return errorMsgTxt;
    }

//  End of File  
