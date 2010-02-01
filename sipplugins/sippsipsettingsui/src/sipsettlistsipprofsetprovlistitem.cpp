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
* Description:  Setting provider list item, modifies title pane
*
*/


// INCLUDE FILES
#include    <akntitle.h>
#include    "sipsettlistsipprofsetprovlistitem.h"
#include    "tsipsettingsdata.h"
#include    "gssippluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetProvListItem::CSIPSettListSIPProfSetProvListItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetProvListItem::CSIPSettListSIPProfSetProvListItem( 
    TInt aIdentifier, 
    TDes& aText, 
    CAknTitlePane* aPane ) :
    CAknTextSettingItem( aIdentifier, aText ),
    iPane( aPane )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetProvListItem::CSIPSettListSIPProfSetProvListItem" )
    }
   
// Destructor
CSIPSettListSIPProfSetProvListItem::~CSIPSettListSIPProfSetProvListItem()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetProvListItem::~CSIPSettListSIPProfSetProvListItem" )    
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetProvListItem::HandleSettingPageEventL
// Gets the setting page events and updates the text on title pane, if
// necessary
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetProvListItem::HandleSettingPageEventL( 
    CAknSettingPage* aSettingPage,
    TAknSettingPageEvent aEventType )
    {    
    __GSLOGSTRING("CSIPSettListSIPProfSetProvListItem::HandleSettingPageEventL" )
    if ( aEventType == EEventSettingOked )
        {
        TBuf<KMaxSIPProviderNameLength> paneText ( KNullDesC );
        paneText.Copy( SettingTextL() );
        AknTextUtils::StripCharacters( paneText, KAknStripListControlChars );
        iPane->SetTextL( paneText );
        } 

    CAknTextSettingItem::HandleSettingPageEventL( aSettingPage, aEventType );
    }

//  End of File  
