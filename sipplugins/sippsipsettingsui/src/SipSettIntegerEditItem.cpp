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
#include    <gssipsettingspluginrsc.rsg>
#include    "SipSettIntegerEditItem.h"
#include    "sipsettlistsipsrvtextsetitem.h"
#include    "sipsettlistsipsrvtextsetpage.h"
#include    "tgssipvalidatorwrapper.h"
#include    "SIPSettIntegerSetPage.h"
#include    "tgssipserverdata.h"
#include    "gssippluginlogger.h"

const TInt KPortMaxLen = 5;
_LIT16(NULLString, " ");

// -----------------------------------------------------------------------------
// CSipSettIntegerEditItem::CSipSettIntegerEditItem
//constructor
// -----------------------------------------------------------------------------
//
CSipSettIntegerEditItem::CSipSettIntegerEditItem(
    TInt aIdentifier, TDes & aInteger):
    CAknTextSettingItem( aIdentifier, aInteger )
    {
    __GSLOGSTRING("CSipSettIntegerEditItem::CSipSettIntegerEditItem" )   
    
    TInt value = -1;
    TLex lex;
    lex.Assign(aInteger.Ptr());
    lex.Val(value);
    iValue = value;
    
 
    
    }

// Destructor
    CSipSettIntegerEditItem::~CSipSettIntegerEditItem()
    {    
    __GSLOGSTRING("CSipSettIntegerEditItem::~CSipSettIntegerEditItem" )
    }

// -----------------------------------------------------------------------------
// CSipSettIntegerEditItem::EditItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSipSettIntegerEditItem::EditItemL( TBool /*aCalledFromMenu*/ )
    {  
    __GSLOGSTRING("CSipSettIntegerEditItem::EditItemL" )
        TInt id = SettingPageResourceId();    
      
        
        CSIPSettIntegerSetPage* page = 
            new( ELeave )CSIPSettIntegerSetPage( 
                id,  
                iValue, 
                CAknIntegerSettingPage::EEmptyValueAllowed );
        page->SetMaximumWidth( KPortMaxLen );
        CleanupStack::PushL( page );
        page->ConstructL();
        CleanupStack::Pop( page );

        // Execute the setting page
        SetSettingPage( page );
        page->ExecuteLD();
        SetSettingPage( NULL );
        if( iValue == -1 )
        	{
        	InternalTextPtr().Copy( NULLString );
        	}
        else
        	{
        	TBuf<KMaxServerPortLength> ValueString;
        	ValueString.AppendNum( iValue );
        	InternalTextPtr().Copy( ValueString );
        	}
        	
        // Update text on this side as well
        UpdateListBoxTextL();  
    }

//  End of File  
