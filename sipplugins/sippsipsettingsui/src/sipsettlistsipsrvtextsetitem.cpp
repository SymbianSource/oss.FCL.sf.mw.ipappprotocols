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
* Description:  Text setting item with a validation of the entered text
*
*/



// INCLUDE FILES
#include    <gssipsettingspluginrsc.rsg>
#include    "sipsettlistsipsrvtextsetitem.h"
#include    "sipsettlistsipsrvtextsetpage.h"
#include    "tgssipvalidatorwrapper.h"
#include    "gssippluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvTextSetItem::CSIPSettListSIPSrvTextSetItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPSrvTextSetItem::CSIPSettListSIPSrvTextSetItem(
    TInt aIdentifier, TDes& aText,    
    TInt (*aFunctionL)(TAny *aAny ) ):
    CAknTextSettingItem( aIdentifier, aText )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvTextSetItem::CSIPSettListSIPSrvTextSetItem" )
    iFunctionL = aFunctionL;
    }

// Destructor
CSIPSettListSIPSrvTextSetItem::~CSIPSettListSIPSrvTextSetItem()
    { 
    __GSLOGSTRING("CSIPSettListSIPSrvTextSetItem::~CSIPSettListSIPSrvTextSetItem" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvTextSetItem::EditItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvTextSetItem::EditItemL( TBool /*aCalledFromMenu*/ )
    {    
    __GSLOGSTRING("CSIPSettListSIPSrvTextSetItem::EditItemL Start" )
    TInt id = SettingPageResourceId();    
    TGSSIPValidatorWrapper cBack( iFunctionL, &(InternalTextPtr()) );
    
    CSIPSettListSIPSrvTextSetPage* page = 
        new( ELeave )CSIPSettListSIPSrvTextSetPage( 
            id, 
            cBack, 
            InternalTextPtr(), 
            CAknTextSettingPage::EZeroLengthAllowed );
	CleanupStack::PushL( page );
    page->ConstructL();
    CleanupStack::Pop( page );

    // Execute the setting page
    SetSettingPage( page );
    page->ExecuteLD();
    SetSettingPage( NULL );

    // Update text on this side as well
    UpdateListBoxTextL();   
    __GSLOGSTRING("CSIPSettListSIPSrvTextSetItem::EditItemL End" )
    }

//  End of File  
