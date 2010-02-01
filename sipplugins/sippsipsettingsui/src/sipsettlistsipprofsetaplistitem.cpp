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
* Description:  The modified list item for the access points
*
*/



// INCLUDE FILES

#include    <akntitle.h>
#include    "sipsettlistsipprofsetaplistitem.h"
#include    "sipsettlistsipprofsetmodel.h"
#include    "gssippluginlogger.h"
// LOCAL CONSTANTS AND MACROS

const TInt KUnknownAPUID    = -1;
const TInt KNullTextArray   =  1;
const TInt KTextArray       =  5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetAPListItem::CSIPSettListSIPProfSetAPListItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetAPListItem::CSIPSettListSIPProfSetAPListItem(         
    TInt aResourceId, 
	TInt& aIndex,
    CSIPSettListSIPProfSetModel* aModel ) :
    CAknEnumeratedTextPopupSettingItem( aResourceId, aIndex ),
	iModel( aModel ),
    iEnumValue( aIndex ),
    iBackupValue( aIndex )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetAPListItem::CSIPSettListSIPProfSetAPListItem" )
    }
   
// Destructor
CSIPSettListSIPProfSetAPListItem::~CSIPSettListSIPProfSetAPListItem()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetAPListItem::~CSIPSettListSIPProfSetAPListItem" )   
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetAPListItem::ValueMatches
// Compares enumeration values to see if a match can be found from
// the list of current APs
// -----------------------------------------------------------------------------
//
TBool CSIPSettListSIPProfSetAPListItem::ValueMatches(
    CArrayPtr<CAknEnumeratedText>* aArray )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetAPListItem::ValueMatches" )
    TInt i( 0 );
    TBool found = EFalse;
    TInt count( aArray->Count() );

    for ( i = 0; i < count; i++ )
        {
        if ( aArray->At( i )->EnumerationValue() == iEnumValue )
            {
            found = ETrue;
            i = count;
            }
        }

    return found;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetAPListItem::InitializeListL()
// Initializes the enumeration list/txt pairs for the pop-up list
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetAPListItem::InitializeListL( 
    CArrayPtr<CAknEnumeratedText>*& aTextArray,
    CArrayPtr<HBufC>*& aNullTextArray )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetAPListItem::InitializeListL" )
    // The text-enumerated thing pair
    aTextArray = new ( ELeave ) CArrayPtrFlat<CAknEnumeratedText>( KTextArray );
    TCleanupItem clItem( ResetAndDestroy, aTextArray );
    CleanupStack::PushL( clItem );

    // This must be valid, but is empty
	aNullTextArray = new ( ELeave ) CArrayPtrFlat<HBufC>( KNullTextArray );
    CleanupStack::PushL( aNullTextArray );

    // Get & set lists    
    iModel->GetAPListL( aTextArray, iEnumValue );        

    // Ownership has changed, we can pop the arrays
    CleanupStack::Pop( aNullTextArray );
    CleanupStack::Pop();    // clItem
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetAPListItem::CompleteConstructionL
// Fetches and changes the lists after the construction is completed
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetAPListItem::CompleteConstructionL()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetAPListItem::CompleteConstructionL Start" )
    CArrayPtr<CAknEnumeratedText>* textArray = NULL;
    CArrayPtr<HBufC>* nullTextArray = NULL;

    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();    

    // Read the AP list
    InitializeListL( textArray, nullTextArray );    

    // Check that we are pointing to an actual thing, otherwise
    // we need to modify this
    if ( ( textArray->Count() > KErrNone && !ValueMatches( textArray )  )
        || ( iModel->APLocked() && !iModel->IsRegistered() ) )
        {
        // Should display text 'None' when the AP is not found
        textArray->ResetAndDestroy();
        iEnumValue = KUnknownAPUID;
        }

    // Set new text arrays
    SetEnumeratedTextArrays( textArray, nullTextArray );
    
    // Update the lists internally
    HandleTextArrayUpdateL();
    __GSLOGSTRING("CSIPSettListSIPProfSetAPListItem::CompleteConstructionL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetAPListItem::EditItemL
// Called before the pop-up list is shown. Updates it, if there was a 
// non-matching AP UID at the start-up 
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetAPListItem::EditItemL( 
    TBool aCalledFromMenu )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetAPListItem::EditItemL Start" )
    if ( iModel->APLocked() )
        {
        return;
        }
        
    // Set backup value.
    iBackupValue = iEnumValue;
    
    if ( iEnumValue == KUnknownAPUID )
        {
        // AP was not found, create list for user to change the AP.
        // The AP will be anyway changed; no matter does the user
        // press Cancel or not..(Chosen AP will be the first one on the list,
        // if user presses Cancel)        
        CArrayPtr<CAknEnumeratedText>* textArray = NULL;
        CArrayPtr<HBufC>* nullTextArray = NULL;
        InitializeListL( textArray, nullTextArray );     

        if ( textArray->Count() > KErrNone )
            {
            // There might be situation that no access points exist.
            iEnumValue = textArray->At( KErrNone )->EnumerationValue();
            }

        SetEnumeratedTextArrays( textArray, nullTextArray );    
        HandleTextArrayUpdateL();
        }
    
    CAknEnumeratedTextPopupSettingItem::EditItemL( aCalledFromMenu );
    __GSLOGSTRING("CSIPSettListSIPProfSetAPListItem::EditItemL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetAPListItem::ResetAndDestroy
// For deleting array in case of leave
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetAPListItem::ResetAndDestroy( TAny* aPointerArray )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetAPListItem::ResetAndDestroy" )
    CArrayPtrFlat<CAknEnumeratedText>* array =
        static_cast<CArrayPtrFlat<CAknEnumeratedText>*>( aPointerArray );
    array->ResetAndDestroy();
    delete array;
    }

// -----------------------------------------------------------------------------
// CGSSettListSIPProfSetAPListItem::HandleSettingPageEventL
// From MAknSettingPageObserver
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetAPListItem::HandleSettingPageEventL(
    CAknSettingPage* /*aSettingPage*/, TAknSettingPageEvent aEventType )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetAPListItem::HandleSettingPageEventL" )
    if ( aEventType == EEventSettingCancelled )
        {
        // If new profile or access point was not defined.
        if ( iBackupValue == KUnknownAPUID )
            {
            // Set text to setting page.
            CArrayPtr<CAknEnumeratedText>* textArray = NULL;
            CArrayPtr<HBufC>* nullTextArray = NULL;
            
            InitializeListL( textArray, nullTextArray );
            textArray->ResetAndDestroy();
            
            SetEnumeratedTextArrays( textArray, nullTextArray );    
            HandleTextArrayUpdateL();
            }
        
        // Setting page cancelled.
        iEnumValue = iBackupValue;
        
        // Update internal value.
        this->LoadL();
        }
    }

//  End of File  
