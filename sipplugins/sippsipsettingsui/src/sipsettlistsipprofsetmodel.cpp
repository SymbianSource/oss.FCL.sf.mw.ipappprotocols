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
* Description:  The model of the profile setting view
*
*/


// INCLUDE FILES

#include    "sipsettlistsipprofsetmodel.h"
#include    "gssipmodel.h"
#include    "tsipsettingsdata.h"
#include    "mgssipprofilehandler.h"
#include    "gssippanics.h"
#include    <aknsettingitemlist.h>
#include    <commdb.h>
#include    <eikenv.h>
#include    <eikappui.h>
#include    <StringLoader.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    <featmgr.h>
#include    <cmmanager.h>
#include    <cmdestination.h>
#include    <cmconnectionmethoddef.h>
#include    "gssippluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::CSIPSettListSIPProfSetModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetModel::CSIPSettListSIPProfSetModel(
    MGSSIPProfileHandler* aHandler ) :
    iHandler( aHandler ),
    iLocked( EFalse )
    { 
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::CSIPSettListSIPProfSetModel" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetModel::ConstructL()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::ConstructL" )
    iProfileData = iHandler->ProfileData();
    // Find right profile from SIP profile array.
    CArrayPtr<CSIPManagedProfile>* list = iHandler->ProfileArray();
    for ( TInt i = 0; i < list->Count(); i++ )
        {
        TUint32 profileId ( KErrNone );
        list->At( i )->GetParameter( KSIPProfileId, profileId );
        if ( profileId == iProfileData->iID )
            {
             // Get SIP profile lock information.
            list->At( i )->GetParameter( KSIPProfileLocked, iLocked );
            list->At( i )->GetParameter( KSIPProfileRegistered, iRegistered );
            if ( iLocked && iRegistered )
                {
                list->At( i )->GetParameter( KSIPAccessPointId, 
                                        iProfileData->iAPIndex );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetModel* CSIPSettListSIPProfSetModel::NewL(
    MGSSIPProfileHandler* aHandler )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::NewL" )
    CSIPSettListSIPProfSetModel* self = 
        new ( ELeave ) CSIPSettListSIPProfSetModel( aHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::~CSIPSettListSIPProfSetModel   
// Destructor
// -----------------------------------------------------------------------------
//
CSIPSettListSIPProfSetModel::~CSIPSettListSIPProfSetModel()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::~CSIPSettListSIPProfSetModel" )
    if ( iTypeInfo )
        {
        iTypeInfo->Close();
        delete iTypeInfo;
        }
	iAPsUIDs.Close();
	iSNAPUIDs.Close();
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::Settings()
// Fetches settings of the modified profile
// -----------------------------------------------------------------------------
//
TSIPSettingsData& CSIPSettListSIPProfSetModel::Settings()    
    {
    return *iProfileData;    
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::GetAPListL
// Fetches Access Point settings
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetModel::GetAPListL( 
    CArrayPtr<CAknEnumeratedText>* aArray,
    TInt& aIndex )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::GetAPListL Start" )
    using namespace CMManager;
    TInt i( 0 );
    iAPsUIDs.Reset();

    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    RArray< TUint32 > connectionMethodsIds;
    CleanupClosePushL( connectionMethodsIds );
    TBool checkBearerType(ETrue);
    TBool legacyOnly(EFalse);
    TBool easyWlan(EFalse);
    cmManager.ConnectionMethodL( connectionMethodsIds,
    							 checkBearerType,
    							 legacyOnly,
    							 easyWlan );
    
    for( i = 0; i < connectionMethodsIds.Count(); i++ )
        {                
        // New
        TUint32 iapId = connectionMethodsIds[i];
        HBufC* apName = 
            cmManager.GetConnectionMethodInfoStringL( iapId, ECmName );
        CleanupStack::PushL( apName );
        // Add Access Point name to enumerated text list.
        CAknEnumeratedText* item = 
            new( ELeave )CAknEnumeratedText( i, apName );
        CleanupStack::Pop( apName ); // Ownership is transferred to item    
        CleanupStack::PushL( item );
        aArray->AppendL( item );
        CleanupStack::Pop( item );
        
        // Add Access Point id to id list.
        iAPsUIDs.AppendL( iapId );        
        }
        
    CleanupStack::PopAndDestroy(); // connectionMethodsId
    CleanupStack::PopAndDestroy(); // cmManager       
    
    // Get LAN AP's also if we are under emulator environment.        
    #ifdef __WINS__
    CCommsDatabase* db = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( db );
    CCommsDbTableView* table = db->OpenIAPTableViewMatchingBearerSetLC(
        KCommDbBearerLAN, ECommDbConnectionDirectionOutgoing );
    TInt error = table->GotoFirstRecord();
    while ( error == KErrNone )
        {
        TUint32 id( 0 );
        TPtrC ptr( COMMDB_ID );
        TPtrC ptr2( COMMDB_NAME );
    
        // Get AP id and name from CommsDB.
        table->ReadUintL( ptr, id );
        
        if ( iAPsUIDs.Find( id ) < 0 )
            {
            // AP not in the list yet        
            HBufC* txt = table->ReadLongTextLC( ptr2 );

            CAknEnumeratedText* item = 
                new( ELeave )CAknEnumeratedText( i, txt );
            CleanupStack::Pop( txt );    // ownership of txt changes to item
            CleanupStack::PushL( item );
            aArray->AppendL( item );
            CleanupStack::Pop( item );
        
            // Add Access Point id to id list. 
            iAPsUIDs.AppendL( id );
            }

        error = table->GotoNextRecord();
        i++;    // To next enumeration
        }
    CleanupStack::PopAndDestroy( table );
    CleanupStack::PopAndDestroy( db );
    #endif  //  __WINS__    
    
    // Loop through the list to find seleced profile.
	for ( TInt j = 0; j < iAPsUIDs.Count(); j++ )
		{
		if ( iProfileData->iAPIndex == TInt( iAPsUIDs[j] ) )
			{
			// Set the currently active access point
			aIndex = j;
			}
		}   
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::GetAPListL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::GetDestinationsListL
// Fetches destinations settings
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetModel::GetDestinationsListL( 
    CArrayPtr<CAknEnumeratedText>* aArray,
    TInt& aIndex )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::GetDestinationsListL Start" )
    iSNAPUIDs.Reset();

    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL(cmManager);
    
    RArray<TUint32> destinations;
    CleanupClosePushL(destinations);
    cmManager.AllDestinationsL(destinations);
    
    RCmDestination destination;
    HBufC *destinationName;
    
    for( TInt i = 0; i < destinations.Count(); i++)
        {
        destination = cmManager.DestinationL(destinations[i]);
        CleanupClosePushL(destination);
        
        destinationName = destination.NameLC();
        CAknEnumeratedText* destItem = 
            new( ELeave )CAknEnumeratedText( i, destinationName );
        CleanupStack::PushL( destItem );
        aArray->AppendL( destItem );
        CleanupStack::Pop( destItem );
        TUint32 destId = destination.Id();
        iSNAPUIDs.Append( destId );
        CleanupStack::Pop( destinationName ); // Ownership is transferred to item    
        destinationName = NULL;
        CleanupStack::PopAndDestroy(); // destination
        }
    
    CleanupStack::PopAndDestroy(); // destinations
    CleanupStack::PopAndDestroy(); // cmManager

    // Loop through the list to find seleced profile.
	for ( TInt j = 0; j < iSNAPUIDs.Count(); j++ )
		{
		if ( iProfileData->iSNAPIndex == TInt( iSNAPUIDs[j] ) )
			{
			// Set the currently active access point
			aIndex = j;
			}
		}
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::GetDestinationsListL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::GetTypeListL
// Fetches Service type list settings
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetModel::GetTypeListL( 
    CArrayPtr<CAknEnumeratedText>* aArray,
    TInt& aIndex )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::GetTypeListL Start" )
    if ( iTypeInfo )
        {
        iTypeInfo->Close();
        delete iTypeInfo;
        iTypeInfo = NULL;
        }

    iTypeInfo = iHandler->TypeListL();       

    TInt i( 0 );
    TInt count( iTypeInfo->Count() );

    // Set aIndex to be the first item if no service profile is set
    aIndex = 0;    
    for ( i  = 0; i < count; i++ )
        { 
        TSIPProfileTypeInfo info = ProfileTypeInfo( i );
        if ( ( info.iSIPProfileName == 
               iProfileData->iServiceProfile.iSIPProfileName ) &&
             ( info.iSIPProfileClass == 
               iProfileData->iServiceProfile.iSIPProfileClass ) )
            {
            // Found the currently active index
            aIndex = i;
            }
        
        // Set correct names based on profile type.
        HBufC* profTxt = NULL;
        if ( info.iSIPProfileClass == TSIPProfileTypeInfo::EInternet )
            {
            profTxt = StringLoader::LoadLC( R_QTN_SIP_IETF );
            }
        else if ( info.iSIPProfileClass == TSIPProfileTypeInfo::EIms )
            {
            profTxt = StringLoader::LoadLC( R_QTN_SIP_THREE_GPP );
            }
        else
            {
            profTxt = HBufC::NewLC( KMaxServiceTypeName );
            profTxt->Des().Copy( ProfileTypeInfo( i ).iSIPProfileName );
            }    
        
        CAknEnumeratedText* item = 
            new ( ELeave ) CAknEnumeratedText( i, profTxt );
        CleanupStack::Pop( profTxt );

        CleanupStack::PushL( item );
        aArray->AppendL( item );        
        CleanupStack::Pop( item );
        }
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::GetTypeListL End" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::ProviderNameLC
// Returns the name of the service provider
// -----------------------------------------------------------------------------
//
HBufC* CSIPSettListSIPProfSetModel::ProviderNameLC()
    {
    HBufC* buf = iProfileData->iProviderName.AllocLC();
    return buf;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::StoreServiceProfileSetting
// Stores the service profile setting, i.e. matches the chosen
// index to the TSIPProfileTypeInfo struct accordingly
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetModel::StoreServiceProfileSetting()
    {
    iProfileData->iServiceProfile = 
        ProfileTypeInfo( iProfileData->iServiceProfileIndex );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::ProfileTypeInfo
// Returns the profile setting with the provided index.
// -----------------------------------------------------------------------------
TSIPProfileTypeInfo& CSIPSettListSIPProfSetModel::ProfileTypeInfo( TInt aIndex )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::ProfileTypeInfo" )
	TInt index;
	if ( aIndex <= 0 )
		{
		index = 0;
		}
	else if ( iTypeInfo->Count() <= aIndex )
		{
		index = aIndex - 1;
		}
	else
		{
		index = aIndex;
		}
    return ( (*iTypeInfo)[ index ] );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::SaveProfileL
// Starts saving the modified settings
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetModel::SaveProfileL()
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::SaveProfileL" )
    StoreServiceProfileSetting();
    iHandler->SaveProfileL();   
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::APUID
// Returns Access Point UID based on the index
// -----------------------------------------------------------------------------
//
TUint32 CSIPSettListSIPProfSetModel::APUID( const TInt aIndex ) const
	{
	return iAPsUIDs[aIndex];
	} 
	
// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::APUID
// Returns Access Point UID
// -----------------------------------------------------------------------------
//
TUint32 CSIPSettListSIPProfSetModel::APUID( ) const
	{
	return iProfileData->iAPIndex;
	} 
	
// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::SNAPUID
// Returns SNAP UID based on the index
// -----------------------------------------------------------------------------
//
TUint32 CSIPSettListSIPProfSetModel::SNAPUID( const TInt aIndex ) const
	{
	return iSNAPUIDs[aIndex];
	} 

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::CheckDuplicateNameL
// Checks that given name is not already exists.
// -----------------------------------------------------------------------------
//
TBool CSIPSettListSIPProfSetModel::CheckDuplicateNameL( TDes& aName )
	{
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::CheckDuplicateNameL" )
	TBool ret( EFalse );
	TBuf<KMaxSIPProviderNameLength> savedName ( KNullDesC );
	savedName.Copy( aName );
	iHandler->CheckDuplicateNameL( iProfileData->iID, aName );
	
	// Return ETrue if name unique.
	if ( !savedName.Compare( aName ) )
	    {
	    ret = ETrue;
	    }
	    
	return ret;    
	} 

// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::ProfileLocked
// Checks is currently shown profile locked.
// -----------------------------------------------------------------------------
//    
TBool CSIPSettListSIPProfSetModel::ProfileLocked()
    {
    return iLocked;
    }
    	
// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::APLocked
// Checks is SNAP is defined and so AP setting is locked.
// -----------------------------------------------------------------------------
//    
TBool CSIPSettListSIPProfSetModel::APLocked()
    {
    return iAPLocked;
    }
    	
// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::SetApLocked
// SNAPid defined, lock AP setting.
// -----------------------------------------------------------------------------
//    
void CSIPSettListSIPProfSetModel::SetApLocked()
    {
    iAPLocked = ETrue;
    }

// ---------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::IsRegistered
// Profile registration status.
// ---------------------------------------------------------------------------
//
TBool CSIPSettListSIPProfSetModel::IsRegistered()
	{
	return iRegistered;
	}
	
// ---------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::CleanAPUid
// Remove defined AP UID .
// ---------------------------------------------------------------------------
//
void CSIPSettListSIPProfSetModel::CleanAPUid()
	{
    iProfileData->iAPIndex = -1;
	}
	
// -----------------------------------------------------------------------------
// CSIPSettListSIPProfSetModel::DestinationsCountL
// Gets the destinations count.
// -----------------------------------------------------------------------------
//
TInt CSIPSettListSIPProfSetModel::DestinationsCountL( )
    {
    __GSLOGSTRING("CSIPSettListSIPProfSetModel::DestinationsCountL" )
    TInt count = 0;
    
    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL(cmManager);
    
    RArray<TUint32> destinations;
    CleanupClosePushL(destinations);
    cmManager.AllDestinationsL(destinations);
    
    count = destinations.Count();
    
    CleanupStack::PopAndDestroy(); // destinations
    CleanupStack::PopAndDestroy(); // cmManager
    
    return count;
    }
//  End of File  
