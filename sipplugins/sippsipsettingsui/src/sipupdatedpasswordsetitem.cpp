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
* Description:  To update the text on the list box every time
*
*/


// INCLUDE FILES

#include "sipupdatedpasswordsetitem.h"
#include    "gssippluginlogger.h"
// CONSTANTS

const TInt KSIPExtraPasswdChars = 4;
const TInt KSIPRemovePasswdChars = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPUpdatedPasswordSetItem::CSIPUpdatedPasswordSetItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPUpdatedPasswordSetItem::CSIPUpdatedPasswordSetItem(
    TInt aIdentifier,
    enum TAknPasswordSettingItemMode aMode, 
    TDes& aPassword ) :
    CAknPasswordSettingItem( aIdentifier, aMode, aPassword ),
    iPassword( aPassword )
    {
    __GSLOGSTRING("CSIPUpdatedPasswordSetItem::CSIPUpdatedPasswordSetItem" )
    }

// Destructor
CSIPUpdatedPasswordSetItem::~CSIPUpdatedPasswordSetItem()
    {
    __GSLOGSTRING("CSIPUpdatedPasswordSetItem::~CSIPUpdatedPasswordSetItem" )
    }

// -----------------------------------------------------------------------------
// CSIPUpdatedPasswordSetItem::EditItemL
// Displays the popup list and then updates the list manually afterwards
// -----------------------------------------------------------------------------
//
void CSIPUpdatedPasswordSetItem::EditItemL( TBool aCalledFromMenu )
    {
    __GSLOGSTRING("CSIPUpdatedPasswordSetItem::EditItemL" )
    CAknPasswordSettingItem::EditItemL( aCalledFromMenu );
    UpdateListBoxTextL();
    StoreL();
       
    if ( iPassword.Length() )
        {
        // Remove two asterisks from ListBoxText() descriptor if needed.
        TInt pos = SettingName().Length() + KSIPExtraPasswdChars;
        TPtr ptr = ListBoxText()->Des();
        ptr.Delete( pos, KSIPRemovePasswdChars );
        }
    }
