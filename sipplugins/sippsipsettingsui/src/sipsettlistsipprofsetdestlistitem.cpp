/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The modified list item for the destinations
*
*/



// INCLUDE FILES

#include    <akntitle.h>
#include    <StringLoader.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    <aknnotewrappers.h>
#include    "sipsettlistsipprofsetdestlistitem.h"
#include    "sipsettlistsipprofsetmodel.h"
#include    "gssippluginlogger.h"
// LOCAL CONSTANTS AND MACROS



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetDestListItem::CSIPSettListSIPProfSetDestListItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetDestListItem::CSIPSettListSIPProfSetDestListItem(         
    TInt aResourceId, 
	TInt& aIndex,
    CSIPSettListSIPProfSetModel* aModel ) :
    CAknEnumeratedTextPopupSettingItem( aResourceId, aIndex ),
	iModel( aModel ),
    iEnumValue( aIndex ),
    iBackupValue( aIndex )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetDestListItem::CSIPSettListSIPProfSetDestListItem" )
    }
   
// Destructor
CSIPSettListSIPProfSetDestListItem::~CSIPSettListSIPProfSetDestListItem()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetDestListItem::~CSIPSettListSIPProfSetDestListItem" )  
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetDestListItem::ValueMatches
// Compares enumeration values to see if a match can be found from
// the list of current destinations
// -----------------------------------------------------------------------------
//
TBool CSIPSettListSIPProfSetDestListItem::ValueMatches(
    CArrayPtr<CAknEnumeratedText>* aArray )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetDestListItem::ValueMatches" )
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
// CSIPSettListSIPProfSetDestListItem::InitializeListL()
// Initializes the enumeration list/txt pairs for the pop-up list
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetDestListItem::InitializeListL( 
    CArrayPtr<CAknEnumeratedText>*& aTextArray,
    CArrayPtr<HBufC>*& aNullTextArray )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetDestListItem::InitializeListL" )
    // The text-enumerated thing pair
    aTextArray = new ( ELeave ) CArrayPtrFlat<CAknEnumeratedText>( KTextArray );
    TCleanupItem clItem( ResetAndDestroy, aTextArray );
    CleanupStack::PushL( clItem );

    // This must be valid, but is empty
	aNullTextArray = new ( ELeave ) CArrayPtrFlat<HBufC>( KNullTextArray );
    CleanupStack::PushL( aNullTextArray );

    iModel->GetDestinationsListL( aTextArray, iEnumValue );
    
    // Ownership has changed, we can pop the arrays
    CleanupStack::Pop( aNullTextArray );
    CleanupStack::Pop();    // clItem
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetDestListItem::CompleteConstructionL
// Fetches and changes the lists after the construction is completed
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetDestListItem::CompleteConstructionL()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetDestListItem::CompleteConstructionL" )
    CArrayPtr<CAknEnumeratedText>* textArray = NULL;
    CArrayPtr<HBufC>* nullTextArray = NULL;

    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();    

    // Read the AP list
    InitializeListL( textArray, nullTextArray );    

    // Check that we are pointing to an actual thing, otherwise
    // we need to modify this
    if ( textArray->Count() > KErrNone && !ValueMatches( textArray ) )
        {
        // Should display text 'None' when the AP is not found
        textArray->ResetAndDestroy();
        iEnumValue = KUnknownAPUID;
        }

    // Set new text arrays
    SetEnumeratedTextArrays( textArray, nullTextArray );
    
    // Update the lists internally
    HandleTextArrayUpdateL();
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetADestListItem::EditItemL
// Called before the pop-up list is shown. Updates it, if there was a 
// non-matching destination UID at the start-up 
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetDestListItem::EditItemL( 
    TBool aCalledFromMenu )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetDestListItem::EditItemL" )
    // Set backup value.
    iBackupValue = iEnumValue;
        
    if ( iEnumValue == KUnknownAPUID )
        {
        // destination was not found, create list for user to change the 
        // destination.
        // The destination will be anyway changed; no matter does the user
        // press Cancel or not..(Chosen destination will be the first one 
        // on the list, if user presses Cancel)        
        CArrayPtr<CAknEnumeratedText>* textArray = NULL;
        CArrayPtr<HBufC>* nullTextArray = NULL;
        InitializeListL( textArray, nullTextArray );     

        if ( textArray->Count() > KErrNone )
            {
            // There might be situation that no destinationss exist.
            iEnumValue = textArray->At( KErrNone )->EnumerationValue();
            }

        SetEnumeratedTextArrays( textArray, nullTextArray );    
        HandleTextArrayUpdateL();
        }
    
    CAknEnumeratedTextPopupSettingItem::EditItemL( aCalledFromMenu );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetDestListItem::ResetAndDestroy
// For deleting array in case of leave
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetDestListItem::ResetAndDestroy( TAny* aPointerArray )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetDestListItem::ResetAndDestroy" )
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
void CSIPSettListSIPProfSetDestListItem::HandleSettingPageEventL(
    CAknSettingPage* /*aSettingPage*/, TAknSettingPageEvent aEventType )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetDestListItem::HandleSettingPageEventL Start" )
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
    else if ( iBackupValue == KUnknownAPUID )
        { //if not defined before && now defined && 'ok' pressed
        // Load and initialize the dialog text
        HBufC* dlgTxt = StringLoader::LoadLC( 
            R_SIP_QUERY_SELECT_DESTINATION );

        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        CleanupStack::PushL( dlg );
        dlg->SetPromptL( *dlgTxt );
        CleanupStack::Pop( dlg );
        
        if ( !dlg->ExecuteLD( R_SIP_DESTINATION_CONFIRMATION_DLG ) )
            {
            // Set text to setting page.
            CArrayPtr<CAknEnumeratedText>* textArray = NULL;
            CArrayPtr<HBufC>* nullTextArray = NULL;
            
            InitializeListL( textArray, nullTextArray );
            textArray->ResetAndDestroy();
            
            SetEnumeratedTextArrays( textArray, nullTextArray );    
            HandleTextArrayUpdateL();

            // Setting page cancelled.
            iEnumValue = iBackupValue;
        
            // Update internal value.
            this->LoadL();
            }
        else
            {
            if ( iModel->APUID() != TUint32(KUnknownAPUID) )
                {
                iModel->CleanAPUid();
                }
            }
        
        CleanupStack::PopAndDestroy( dlgTxt );      
        } 
    __GSLOGSTRING("CSIPSettListSIPProfSetDestListItem::HandleSettingPageEventL End" )
    }

//  End of File  
