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
* Description:  SIP Settings view controller
*
*/

// INCLUDE FILES


#include    <aknnotewrappers.h>
#include    <eikedwin.h>
#include    <avkon.rsg>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    "SIPSettIntegerSetPage.h"
#include    "tsipsettingsdata.h"
#include    "gssippluginlogger.h"

_LIT( NULLString,"" );
_LIT16( ZeroString,"-1" );
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettIntegerSetPage::CSIPSettIntegerSetPage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettIntegerSetPage::CSIPSettIntegerSetPage(
    TInt aResourceID, 
    TInt& aInteger, 
    TInt aTextSettingPageFlags ) :
    CAknIntegerSettingPage( aResourceID, aInteger, aTextSettingPageFlags )
    {
    __GSLOGSTRING("CSIPSettIntegerSetPage::CSIPSettIntegerSetPage" )
    iValue = aInteger;
    }
    
// Destructor
    CSIPSettIntegerSetPage::~CSIPSettIntegerSetPage()
    {    
    __GSLOGSTRING("CSIPSettIntegerSetPage::~CSIPSettIntegerSetPage" )
    }

// -----------------------------------------------------------------------------
// CSIPSettIntegerSetPage::OkToExitL
// Called to validate input before closing the page
// -----------------------------------------------------------------------------
//
TBool CSIPSettIntegerSetPage::OkToExitL( TBool aAccept )
    {
    __GSLOGSTRING("CSIPSettIntegerSetPage::OkToExitL " )
    TBool ret( ETrue );
    TLex16 lex;
    TInt value;
    TBuf<KMaxServerPortLength> intText; 
    if ( !aAccept )
        {
        // User pressed cancel - return back to original text and go away
        RestoreOriginalSettingL();
        return ETrue;
        }
       TextControl()->GetText( intText );
       lex.Assign( intText.Ptr() );
       lex.Val( value );
       if( value >= 0 && value <= 65535 || intText == NULLString )
    	   {
           if( intText == NULLString )
    	       {
               TextControl()->SetTextL(&ZeroString);
    	       }
           UpdateSettingL();  
            
           // Everything OK, save setting and exit page
           AcceptSettingL();	   
    	   ret = ETrue;
    	   }
       else
    	   {
    	   ret = EFalse;
    	   }
        
        return ret;    
    }
// -----------------------------------------------------------------------------
// CSIPSettIntegerSetPage::DynamicInitL
// Called to the Setpage initialize complete
// -----------------------------------------------------------------------------
//
void  CSIPSettIntegerSetPage::DynamicInitL ()
	{
	CAknIntegerSettingPage::DynamicInitL();
	iNULLString.Copy(NULLString);
	if( iValue == -1 )
	    {
	    TextControl()->SetTextL(&iNULLString);
	    } 
	
	}

//  End of File  

