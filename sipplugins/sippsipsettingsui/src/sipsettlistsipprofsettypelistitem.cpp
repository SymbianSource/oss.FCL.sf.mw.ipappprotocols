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
* Description:  The modified list item for the type selection
*
*/



// INCLUDE FILES

#include    "sipsettlistsipprofsettypelistitem.h"
#include    "sipsettlistsipprofsetmodel.h"
#include    <akntitle.h>
#include    "gssippluginlogger.h"

// LOCAL CONSTANTS AND MACROS

const TInt KNullTextArray   =  1;
const TInt KTextArray       =  5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetTypeListItem::CSIPSettListSIPProfSetTypeListItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetTypeListItem::CSIPSettListSIPProfSetTypeListItem(         
    TInt aResourceId, 
    TInt& aValue,
    CSIPSettListSIPProfSetModel* aModel ) :
    CAknEnumeratedTextPopupSettingItem( aResourceId, aValue ),
    iModel( aModel ),
    iEnumValue( aValue )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetTypeListItem::CSIPSettListSIPProfSetTypeListItem" ) 
    }
   
// Destructor
CSIPSettListSIPProfSetTypeListItem::~CSIPSettListSIPProfSetTypeListItem()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetTypeListItem::~CSIPSettListSIPProfSetTypeListItem" )    
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetTypeListItem::InitializeListL()
// Initializes the enumeration list/txt pairs for the pop-up list
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetTypeListItem::InitializeListL( 
    CArrayPtr<CAknEnumeratedText>*& aTextArray,
    CArrayPtr<HBufC>*& aNullTextArray )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetTypeListItem::InitializeListL" )
    // The text-enumerated thing pair
    aTextArray = new ( ELeave ) CArrayPtrFlat<CAknEnumeratedText>( KTextArray );
    TCleanupItem clItem( ResetAndDestroy, &aTextArray );

    CleanupStack::PushL( clItem );

    // This must be valid, but is empty
	aNullTextArray = new ( ELeave ) CArrayPtrFlat<HBufC>( KNullTextArray );
    CleanupStack::PushL( aNullTextArray );

    // Get & set lists    
    iModel->GetTypeListL( aTextArray, iEnumValue );        

    // Ownership has changed, we can pop the arrays
    CleanupStack::Pop( aNullTextArray );
    CleanupStack::Pop();	//clItem
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetTypeListItem::CompleteConstructionL
// Fetches and changes the lists after the construction is completed
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetTypeListItem::CompleteConstructionL()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetTypeListItem::CompleteConstructionL" )
    CArrayPtr<CAknEnumeratedText>* textArray = NULL;
    CArrayPtr<HBufC>* nullTextArray = NULL;

    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();    

    // Read the AP list
    InitializeListL( textArray, nullTextArray );    

    // Set new text arrays
    SetEnumeratedTextArrays( textArray, nullTextArray );
    
    // Update the lists internally
    HandleTextArrayUpdateL();
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetTypeListItem::EditItemL
// Called before the pop-up list is shown. Updates it, if there was a 
// non-matching AP UID at the start-up 
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetTypeListItem::EditItemL( 
    TBool aCalledFromMenu )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetTypeListItem::EditItemL Start" )
    CArrayPtr<CAknEnumeratedText>* array = EnumeratedTextArray();
    TBool found = EFalse;
    TInt i( 0 );
    for ( i = 0; i < array->Count(); i++ )
        {
        if ( array->At( i )->EnumerationValue() == iEnumValue )
            {
            found = ETrue;
            }
        }

    if ( !found && array->Count() > KErrNone )
        {
        iEnumValue = array->At( 0 )->EnumerationValue();
        }
    
    CAknEnumeratedTextPopupSettingItem::EditItemL( aCalledFromMenu );
    __GSLOGSTRING("CSIPSettListSIPProfSetTypeListItem::EditItemL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetTypeListItem::ResetAndDestroy
// For deleting array in case of leave
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetTypeListItem::ResetAndDestroy( TAny* aPointerArray )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetTypeListItem::ResetAndDestroy" )
    CArrayPtrFlat<CAknEnumeratedText>* array =
        static_cast<CArrayPtrFlat<CAknEnumeratedText>*>( aPointerArray );
    array->ResetAndDestroy();
    delete array;
    }

//  End of File  
