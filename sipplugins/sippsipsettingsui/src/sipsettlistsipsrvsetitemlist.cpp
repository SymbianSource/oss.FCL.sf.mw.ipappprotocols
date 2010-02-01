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
* Description:  The item list of the proxy & registrar views
*
*/



// INCLUDE FILES
#include    "sipsettlistsipsrvsetitemlist.h"
#include    "sipsettlistsipsrvsetmodel.h"
#include    "sipsettlistsipsrvtextsetitem.h"
#include    "sipupdatedpasswordsetitem.h"
#include    "sipserversettingvalidator.h"
#include    "tsipsettingsdata.h"
#include 	"SIPSettListSIPSrvTakeOverCBAKeyPress.h"
#include    "SipSettIntegerEditItem.h"
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    <StringLoader.h>
#include    <sipprofiletypeinfo.h>
#include    <aknnotewrappers.h>
#include    <akntextsettingpage.h>
#include    "gssippluginlogger.h"

// CONSTANTS

const TInt KSIPExtraPasswdChars = 4;
const TInt KSIPRemovePasswdChars = 2;
_LIT16( NULLString, " " );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetItemList::CSIPSettListSIPSrvSetItemList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPSrvSetItemList::CSIPSettListSIPSrvSetItemList( 
    CSIPSettListSIPSrvSetModel* aModel )
    : iModel( aModel )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetItemList::CSIPSettListSIPSrvSetItemList" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetItemList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetItemList::ConstructL( 
    CCoeControl* aParent,
    TInt aResourceID )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetItemList::ConstructL" )
    // Set server type.
    if ( aResourceID == R_SIP_PROXY_SETTING_VIEW_ITEM_LIST )
        {
        iServerType = EGSItemProxyServer;
        }
    else
        {
        iServerType = EGSItemRegistrarServer;
        }
    
    // Fetch new settings before constructing the item list
    iServerData = iModel->Settings();

    // Continue with construction
    SetContainerWindowL( *aParent );	
    ConstructFromResourceL( aResourceID );

    ListBox()->CreateScrollBarFrameL( ETrue );
    ListBox()->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
 
    ListBox()->View()->ItemDrawer()->SetFlags( CTextListItemDrawer::ESingleClickEnabled );
    
    // Remove two asterisks from ListBoxText() descriptor if needed.
    if ( iServerData->iPassword.Length() )
        {
        TInt pos = iPassword->SettingName().Length() + KSIPExtraPasswdChars;
        TPtr ptr = iPassword->ListBoxText()->Des();
        ptr.Delete( pos, KSIPRemovePasswdChars );     
        }

    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetItemList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPSrvSetItemList* CSIPSettListSIPSrvSetItemList::NewL(
    CCoeControl* aParent, 
    CSIPSettListSIPSrvSetModel* aModel,
    TInt aResourceID )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetItemList::NewL" )
    CSIPSettListSIPSrvSetItemList* self = 
        new ( ELeave ) CSIPSettListSIPSrvSetItemList( aModel );
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent, aResourceID );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CSIPSettListSIPSrvSetItemList::~CSIPSettListSIPSrvSetItemList()
    {
    
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetItemList::CreateSettingItemL
// Creates the items to the list
// -----------------------------------------------------------------------------
//
CAknSettingItem* CSIPSettListSIPSrvSetItemList::CreateSettingItemL( 
    TInt aSettingId )
    {   
    __GSLOGSTRING("CSIPSettListSIPSrvSetItemList::CreateSettingItemL" )
    __GSLOGSTRING1("CSIPSettListSIPSrvSetItemList::CreateSettingItemL aSettingId: %d", aSettingId)
    CAknSettingItem* item = NULL;

    switch ( aSettingId )
        {
        case EGSServerItemAddr:
            iAddress = new ( ELeave ) CSIPSettListSIPSrvTextSetItem(
                aSettingId, iServerData->iAddress,
                SIPServerSettingValidator::ValidateServerAddressL );
            item = iAddress;
            break;

        case EGSServerItemRealm:
            iRealm = new ( ELeave ) CAknTextSettingItem(
                aSettingId, iServerData->iRealm );
            iRealm->SetSettingPageFlags( 
                CAknTextSettingPage::EZeroLengthAllowed );            
            item = iRealm;
            break;

        case EGSServerItemUsername:
            // If opening view is registrar server view, 
            // service profile type is IMS and private user identity 
            // is set, set provisioned text to the setting item.
            if ( iServerType == EGSItemRegistrarServer && 
                iModel->ProfileType() == TSIPProfileTypeInfo::EIms &&
                iServerData->iPrivateIdentity.Length() > KErrNone )
                {
                HBufC* provisionedText = 
                    StringLoader::LoadLC( R_QTN_SIP_IMS_UNAME_PROVISIONED );
                iUName.Copy( *provisionedText );
                CleanupStack::PopAndDestroy( provisionedText );    
                
                iUsername = new ( ELeave ) CAknTextSettingItem(
                    aSettingId, iUName ); 
                }
            else
                {
                iUsername = new ( ELeave ) CAknTextSettingItem(
                    aSettingId, iServerData->iUsername );
                }    
            
            iUsername->SetSettingPageFlags( 
                CAknTextSettingPage::EZeroLengthAllowed );
            item = iUsername;
            break;

        case EGSServerItemPassword:
            iPassword = new ( ELeave ) CSIPUpdatedPasswordSetItem(
                aSettingId, CAknPasswordSettingItem::EAlpha,
                iServerData->iPassword );
            iPassword->SetSettingPageFlags(
                CAknTextSettingPage::EZeroLengthAllowed );    
            item = iPassword;
            break;

        case EGSServerItemRouting:
            iLooseRouting = new ( ELeave ) CAknEnumeratedTextPopupSettingItem(
                aSettingId, iServerData->iLooseRouting );
            item = iLooseRouting;
            break;      

        case EGSServerItemPort:
        	if( iServerData->iPortNumber == KDefaultPortNumber )
                {
                iPortValue.Copy(NULLString);
                iPort = new ( ELeave ) CSipSettIntegerEditItem( 
                aSettingId, iPortValue );
                } 
            else
                {
                iPortValue.Alloc();
                iPortValue.AppendNum( iServerData->iPortNumber );
                iPort = new ( ELeave ) CSipSettIntegerEditItem( 
                aSettingId, iPortValue );
                }
            
			iPort->SetEmptyItemTextL( NULLString );
            item = iPort;
            break;

        case EGSServerItemTransportProtocol:
            iTransportProtocol = 
                new ( ELeave ) CAknEnumeratedTextPopupSettingItem(
                    aSettingId, iServerData->iTransportProtocol );
            item = iTransportProtocol;
            break;
               
        default:
            // Leave if no matching ID is found
            User::Leave( KErrNotSupported );
            break;
        }
    
    return item;
    }          

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetItemList::EditItemL
// Manages list box events
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetItemList::EditItemL( 
    TInt aIndex, 
    TBool aCalledFromMenu )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetItemList::EditItemL Start" )
    TInt currentIndex = ListBox()->CurrentItemIndex();

    // If currently shown profile is locked, show a note and return.
    if ( iModel->ProfileLocked() )
        {
        HBufC* lockedText = 
            StringLoader::LoadLC( R_QTN_SIP_INFONOTE_PROFILE_LOCKED );
        CAknInformationNote* lockedNote = new (ELeave) CAknInformationNote();
        lockedNote->ExecuteLD( *lockedText );
        CleanupStack::PopAndDestroy( lockedText );
        return;
        }

	switch ( currentIndex )	
	{
    	case EGSServerItemUsername:	// Username
    		{
		    // If service profile type is IMS and private user identity is set
    		// don't allow editing the setting item. 
    		// Information note is diplayed.    

	        if ( iServerType == EGSItemRegistrarServer &&
	            iModel->ProfileType() == TSIPProfileTypeInfo::EIms &&
	            iServerData->iPrivateIdentity.Length() > KErrNone )
	            {
	            HBufC* provisionedText = StringLoader::LoadLC(
	                R_QTN_SIP_IMS_UNAME_NOT_EDITABLE );
	            CAknInformationNote* note = 
	                new ( ELeave ) CAknInformationNote( ETrue );
	            note->ExecuteLD( provisionedText->Des() );
	            CleanupStack::PopAndDestroy( provisionedText );   
	            }
	        else 
	            {
	            CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
	            }
	                			
			iUsername->StoreL();
        
        	AknTextUtils::StripCharacters( 
            	iServerData->iUsername, KAknStripListControlChars );
	        CheckSettingForSpaces( iServerData->iUsername );
	        iUsername->LoadL();
	        iUsername->UpdateListBoxTextL();
			break;
    		} 

		case EGSServerItemAddr:		// Address 
			{
        	CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );    			

	        iAddress->StoreL();
        
	        AknTextUtils::StripCharacters( 
    	        iServerData->iAddress, KAknStripListControlChars );
        
        	CheckSettingForSpaces( iServerData->iAddress );
        	iAddress->LoadL();
        	iAddress->UpdateListBoxTextL();
			break;
			}
    	
		case EGSServerItemRealm:	// Realm
			{
        	CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );    			

			iRealm->StoreL();
	        
	        AknTextUtils::StripCharacters( 
	            iServerData->iRealm, KAknStripListControlChars );
	        
	        CheckSettingForSpaces( iServerData->iRealm );
	        iRealm->LoadL();
	        iRealm->UpdateListBoxTextL();	
			break;
			}
			
		case EGSServerItemPassword:	// // Password
			{
	    	// Catch right soft key press in password field to reject 
	    	// password change, if cancel is pressed.	
		    CSIPSettListSIPSrvTakeOverCBAKeyPress* takeOverCancelKey = 
		    		CSIPSettListSIPSrvTakeOverCBAKeyPress::NewLC();			
            CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
			
			// Accept password, if the cancel key hasn't pressed
			if ( !takeOverCancelKey->BackCalled() )
				{
				iServerData->iPasswordChanged = ETrue;
				}
 
			CleanupStack::PopAndDestroy ( takeOverCancelKey );	

	        iPassword->StoreL();
	        
	        AknTextUtils::StripCharacters( 
	            iServerData->iPassword, KAknStripListControlChars );
	        
	        CheckSettingForSpaces( iServerData->iPassword );
	        iPassword->LoadL();
	        iPassword->UpdateListBoxTextL();
	        
	        // Remove two asterisks from ListBoxText() descriptor if needed.
	        if ( iServerData->iPassword.Length() )
	            {
	            TInt pos = iPassword->SettingName().Length() + KSIPExtraPasswdChars;
	            TPtr ptr = iPassword->ListBoxText()->Des();
	            ptr.Delete( pos, KSIPRemovePasswdChars );     
	            }

			break;
			}
		case EGSRegisterServerItemPort:
		    {
			if( iServerType == EGSItemRegistrarServer )
			    {
				CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
				CSipSettIntegerEditItem *Port =  static_cast<CSipSettIntegerEditItem *>(iPort);
				iServerData->iPortNumber = Port->iValue;	     
			    Port->UpdateListBoxTextL();
				}
			else
			    {
			    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu ); 
				}
			}
			break;	
		case EGSServerItemPort:
		    {
			if( iServerType == EGSItemProxyServer )
			    {
				CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
				CSipSettIntegerEditItem *Port =  static_cast<CSipSettIntegerEditItem *>(iPort);
				iServerData->iPortNumber = Port->iValue;	     
			    Port->UpdateListBoxTextL();
				}
			else
			    {
			    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu ); 
				}
			}
			break;
		default:
			{
        	CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );    			
			break;
			}
		}
    __GSLOGSTRING("CSIPSettListSIPSrvSetItemList::EditItemL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetItemList::HandleListBoxEventL
// Manages list box events
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetItemList::HandleListBoxEventL( 
    CEikListBox* aListBox,
    TListBoxEvent aEventType )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetItemList::HandleListBoxEventL Start" )
    TInt currentIndex = aListBox->CurrentItemIndex();
 
    if ( aEventType == EEventEnterKeyPressed ||
    	aEventType == EEventItemSingleClicked  )
        {
    // If currently shown profile is locked, show a note and return.
    if ( iModel->ProfileLocked() )
        {
        HBufC* lockedText = 
            StringLoader::LoadLC( R_QTN_SIP_INFONOTE_PROFILE_LOCKED );
        CAknInformationNote* lockedNote = new (ELeave) CAknInformationNote();
        lockedNote->ExecuteLD( *lockedText );
        CleanupStack::PopAndDestroy( lockedText );
        return;
        }
        // Proxy server.
        if ( iServerType == EGSItemProxyServer )
            {
            // Loose routing.
            if ( currentIndex == EGSServerItemRouting )
                {
                iLooseRouting->StoreL();
                if ( iServerData->iLooseRouting == EGSLooseRoutingOff )
                    {
                    iServerData->iLooseRouting = EGSLooseRoutingOn;
                    }
                else
                    {
                    iServerData->iLooseRouting = EGSLooseRoutingOff;
                    }
                iLooseRouting->LoadL();
                iLooseRouting->UpdateListBoxTextL();    
                aListBox->ActivateL();    
                aListBox->HandleItemAdditionL();
                return;
                }
            }
        // Registrar server. (No loose routing option.)
        else
            {
            // Do nothing
            }
        }
                     
    CAknSettingItemList::HandleListBoxEventL( aListBox, aEventType );   
    __GSLOGSTRING("CSIPSettListSIPSrvSetItemList::HandleListBoxEventL End" )
    }
    
// -----------------------------------------------------------------------------
// CGSSettListSIPSrvSetItemList::CheckSettingForSpaces
// Checks that does setting contains only spaces and removes them if needed.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetItemList::CheckSettingForSpaces( TDes& aSettingText )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetItemList::CheckSettingForSpaces Start" )
    TBool onlySpaces( ETrue );
    
    // Check that is there other characters than spaces.
    for ( TInt i = 0; i < aSettingText.Length() && onlySpaces; i++ )
        {
        const TChar KSpaceCharacter = ' ';
        if ( aSettingText[ i ] != KSpaceCharacter )
            {
            onlySpaces = EFalse;
            }
        }
    
    // If only spaces, reset setting value.
    if ( onlySpaces )
        {
        aSettingText.Zero();
        }
    __GSLOGSTRING("CSIPSettListSIPSrvSetItemList::CheckSettingForSpaces End" )
    }

//  End of File  
