/*
* Copyright (c) 2003 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application's document class
*
*/


// INCLUDE FILES

#include    <aknnotewrappers.h>
#include    <StringLoader.h>
#include    <aknview.h>
#include 	<gsplugininterface.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    <escapeutils.h>
#include    "gssipmodel.h"
#include    "gssipappshutter.h"
#include    "gssipprofileutil.h"
#include    "gssippluginlogger.h"

// CONSTANTS
_LIT( KOpenParenthesis, "(" );
_LIT( KClosedParenthesis, ")" );
const TInt KMaxDuplicatePostfixLength = 10;
const TInt KSIPGranularity = 5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGSSIPModel::CGSSIPModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGSSIPModel::CGSSIPModel() 
    {
    __GSLOGSTRING("CGSSIPModel::CGSSIPModel" )
    iQuitAfterSave = EFalse;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ConstructL()
    {
    __GSLOGSTRING("CGSSIPModel::ConstructL" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSSIPModel* CGSSIPModel::NewL()
    {
    __GSLOGSTRING("CGSSIPModel::NewL" )
    CGSSIPModel* self = new( ELeave ) CGSSIPModel();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

  
// Destructor
CGSSIPModel::~CGSSIPModel()
    {
    __GSLOGSTRING("CGSSIPModel::~CGSSIPModel" )
    if ( iProfiles )
        {
        iProfiles->ResetAndDestroy();
        delete iProfiles;
        }

    delete iAppShutter;
    delete iEngine;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::CreateEngineL
// Creates the application engine
// -----------------------------------------------------------------------------
//
void CGSSIPModel::CreateEngineL( CGSPluginInterface* aAppUi )
    {
    __GSLOGSTRING("CGSSIPModel::CreateEngineL Start" )
    iAppUi = aAppUi;
    
    // Create AppShutter instance.
    iAppShutter = CGSSIPAppShutter::NewL( iAppUi );   
        
    iEngine = CSIPManagedProfileRegistry::NewL( *this );    
    ReadArrayFromEngineL();
    __GSLOGSTRING("CGSSIPModel::CreateEngineL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::EditProfileL
// Copies profile's data to iProfileData
// -----------------------------------------------------------------------------
//
TBool CGSSIPModel::EditProfileL( TInt aIndex )
    {    
    __GSLOGSTRING("CGSSIPModel::EditProfileL " )
    __GSLOGSTRING1("CGSSIPModel::EditProfileL aIndex: %d", aIndex)
    TBool moveToNextView = ETrue;

    CSIPManagedProfile* profile = 
        static_cast<CSIPManagedProfile*>( iProfiles->At( aIndex ) );

    if ( iEngine->IsInUseL( *profile ) )
        {
        HBufC* errorTxt = StringLoader::LoadLC( R_QTN_SIP_ERROR_PROFILE_USED );
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
        note->ExecuteLD( errorTxt->Des() );
        CleanupStack::PopAndDestroy( errorTxt );        

        moveToNextView = EFalse;
        }
    else
        {
        CopyToProfileDataStructL( aIndex );
        }

    return moveToNextView;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::CreateAndEditProfileL
// Starts editing a new profile that is based on an existing one
// -----------------------------------------------------------------------------
//
void CGSSIPModel::CreateAndEditProfileL( TInt aCopiedIndex )
    {
    __GSLOGSTRING("CGSSIPModel::CreateAndEditProfileL Start" )
    __GSLOGSTRING1("CGSSIPModel::CreateAndEditProfileL aCopiedIndex: %d", aCopiedIndex)
    if ( aCopiedIndex == KUnknownProfileIndex )
        {
        // Create new profile with default values
        FillDataStructWithDefaultValsL();        
        }
    else
        {
        CopyToProfileDataStructL( aCopiedIndex );
        iProfileData.iID = KUnknownProfileIndex;
        }
        
    CheckDuplicateNameL( iProfileData.iID, 
        iProfileData.iProviderName , ETrue);   
    __GSLOGSTRING("CGSSIPModel::CreateAndEditProfileL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ProfileData
// Returns pointer to profile data structure for modifying it directly
// -----------------------------------------------------------------------------
//
TSIPSettingsData* CGSSIPModel::ProfileData()
    {
    __GSLOGSTRING("CGSSIPModel::ProfileData" )
    return &iProfileData;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ProfileData
// Returns pointer to the profile array
// -----------------------------------------------------------------------------
//
CArrayPtr<CSIPManagedProfile>* CGSSIPModel::ProfileArray()
    {
    __GSLOGSTRING("CGSSIPModel::ProfileArray" )
    return iProfiles;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::SaveProfileL
// Saves profile to persistent storage
// -----------------------------------------------------------------------------
//
TBool CGSSIPModel::SaveProfileL()
    {
    __GSLOGSTRING("CGSSIPModel::SaveProfileL" )
    if( iNewProfile )
    	{
    	return EFalse;
    	}
    TBool modifications = ETrue;
    TInt err( KErrNone );

    CheckDuplicateNameL( iProfileData.iID, iProfileData.iProviderName );

    if ( iProfileData.iID == KUnknownProfileIndex )
        {
        // Need to create a new profile
        CSIPManagedProfile* profile = 
        	iEngine->CreateL( iProfileData.iServiceProfile );
        
        CleanupStack::PushL( profile );

        // Copy attributes to the new profile        
        iNewProfile = profile;
        profile = CopyDataToProfileL();        
        
        // Save the profile to persistent storage                
        TRAP( err, iEngine->SaveL( *profile ) );
        
        if ( err )
            {
            // When exit is called and we get error code because of same realm
            // with different username, we sohuld exit without saving.
            // Otherwise this method leaves.
            if ( iQuitAfterSave )
                {
                CleanupStack::PopAndDestroy( profile );
                modifications = EFalse;
                }
            else
                {
                User::Leave( err );
                }
            }
        else
            {
            CleanupStack::Pop( profile );
            }
        }
    else
        {
		// check at first if a profile is locked
    	TBool locked( EFalse );
		err = Profile( iProfileData.iID )->GetParameter( KSIPProfileLocked,
	    		locked );
    	if ( locked )
    		{
    		return EFalse;
    		}

        // We can save the modified changes if any
        if ( CheckForModificationsL() )  
            {
            // Check that the profile is not in use
            CSIPManagedProfile* profile = Profile( iProfileData.iID );
            CSIPManagedProfile* clonedProfile = profile->CloneL();
            CleanupStack::PushL( clonedProfile );

            profile = CopyDataToProfileL();
            
            TRAP( err, iEngine->SaveL( *profile ) );                        

            if ( err )
                {
                // Copy old profile data back to temporary variable
                // If this leaves, then nope can do; there hasn't been enough
                // memory on the first place to do the operations, so there isn't
                // much we can do execpt prevent further memory loss               
                ReplaceProfileL( iProfileData.iID, clonedProfile );
                
                // When exit is called and we get error code because of same realm
                // with different username, we sohuld exit without saving.
                // Otherwise this method leaves.
                if ( iQuitAfterSave )
                    {
                    modifications = EFalse;
                    }
                else
                    {
                    User::Leave( err );
                    }    
                }

            CleanupStack::PopAndDestroy( clonedProfile );
            }
        else
            {
            // No modifications, not saving for nothing
            modifications = EFalse;
            }
        }

    return modifications;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::SetDefaultProfileL
// Sets new default profile
// -----------------------------------------------------------------------------
//
void CGSSIPModel::SetDefaultProfileL( TInt aIndex )
    {
    __GSLOGSTRING("CGSSIPModel::SetDefaultProfileL Start" )
    __GSLOGSTRING1("CGSSIPModel::SetDefaultProfileL aIndex: %d", aIndex)
    TBool bDefault = EFalse ;
    TInt err = 
    	iProfiles->At( aIndex )->GetParameter( KSIPDefaultProfile, bDefault );
    User::LeaveIfError( err );
    if ( !bDefault )
        {
        CSIPManagedProfile* profile = 
            static_cast<CSIPManagedProfile*>( iProfiles->At( aIndex ) );

        if ( iEngine->IsInUseL( *profile ) )
            {
            // Can't change the profile, it is in use
            User::Leave( KErrInUse );
            }        

        // Set the profile to be default and save sum' settings
        err= profile->SetParameter( KSIPDefaultProfile, ETrue );
        User::LeaveIfError( err );
        
        TRAP( err, iEngine->SaveL( *profile ) );

        // If SaveL() leaves profile and the old default profile might be
        // set as defaults; need to read the whole array again..
        if ( err )
            {
            ReadProfileListFromEngineSafeL();                            
            User::Leave( err );
            }        
        }
    __GSLOGSTRING("CGSSIPModel::SetDefaultProfileL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::DefaultProfileIndex
// Returns index to the default profile
// -----------------------------------------------------------------------------
//
TInt CGSSIPModel::DefaultProfileIndex()
    {     
    __GSLOGSTRING("CGSSIPModel::DefaultProfileIndex " )
    TInt i( 0 );
    TInt index( KUnknownProfileIndex );
    // "Default values" are not values from default profile; instead they
    // are the values that are defined on UI specification
    
    // Scans through the array and returns the index of the profile
    for ( i = 0; i < iProfiles->Count() && index == KUnknownProfileIndex; i++ )
        {
        TBool defProfile = EFalse;		
    	TInt err = 
    		iProfiles->At( i )->GetParameter( KSIPDefaultProfile, defProfile );        
        if ( err )
            {
            // No luck if leaves
            defProfile = EFalse;
            }

        if ( defProfile )
            {
            index = i;
            }
        }
    __GSLOGSTRING1("CGSSIPModel::DefaultProfileIndex: %d", index) 
    return index;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::TypeListL
// Returns an array consisting of the typenames
// -----------------------------------------------------------------------------
//
RArray<TSIPProfileTypeInfo>* CGSSIPModel::TypeListL()
    {  
    __GSLOGSTRING("CGSSIPModel::TypeListL" )
    RArray<TSIPProfileTypeInfo>* typeArray = 
        new (ELeave) RArray<TSIPProfileTypeInfo>;
    
    TCleanupItem clItem( CloseAndDestroy, &typeArray );
    CleanupStack::PushL( clItem );
    
    // Get the supported profile types
    iEngine->SupportedProfileTypesL( *typeArray );

    CleanupStack::Pop(); //clItem
    
    return typeArray;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::DeleteProfileL
// Deletes a profile from permanent storage
// -----------------------------------------------------------------------------
//
void CGSSIPModel::DeleteProfileL( TInt aIndex )
    {
    __GSLOGSTRING("CGSSIPModel::DeleteProfileL Start" )
    __GSLOGSTRING1("CGSSIPModel::DeleteProfileL aIndex: %d", aIndex) 
    CSIPManagedProfile* profile = iProfiles->At( aIndex );    
    
    if ( iEngine->IsInUseL( *profile ) )
        {
        User::Leave( KErrInUse );
        }

    iEngine->DestroyL( *profile );
    __GSLOGSTRING("CGSSIPModel::DeleteProfileL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::QuitAfterSave
// Set "Quit after save" flag
// -----------------------------------------------------------------------------
//
void CGSSIPModel::QuitAfterSave()
    {
    __GSLOGSTRING("CGSSIPModel::QuitAfterSave" )
    iQuitAfterSave = ETrue;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ErrorInRegistration
// Checks if there has been error in registration
// -----------------------------------------------------------------------------
//
TBool CGSSIPModel::ErrorInRegistration( TInt aIndex )
    {    
    __GSLOGSTRING("CGSSIPModel::ErrorInRegistration" )
    __GSLOGSTRING1("CGSSIPModel::ErrorInRegistration aIndex: %d", aIndex)
    return ( iEngine->LastRegistrationError( 
        *( iProfiles->At( aIndex ) ) ) != KErrNone );
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ProfileRegistryEventOccurred
// SIP profile event 
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ProfileRegistryEventOccurred( 
    TUint32 aSIPProfileId, TEvent aEvent )
	{
	__GSLOGSTRING("CGSSIPModel::ProfileRegistryEventOccurred Start" )
    __GSLOGSTRING1("CGSSIPModel::ProfileRegistryEventOccurred aSIPProfileId: %d", aSIPProfileId)  
    __GSLOGSTRING1("CGSSIPModel::ProfileRegistryEventOccurred aEvent: %d", aEvent) 
    switch( aEvent )	
        {
        case EProfileCreated:
			TRAP_IGNORE( ProfileCreatedL( aSIPProfileId ) )
			break;
        case EProfileUpdated:	
            TRAP_IGNORE( ProfileUpdatedL( aSIPProfileId ) )
            break;
        case EProfileRegistered:						
        case EProfileDeregistered:
            ProfileRegistrationStatusChanged( aSIPProfileId );
            break;
        case EProfileDestroyed:
            ProfileDestroyed( aSIPProfileId );
            break;
        default:
            // Do nothing.
            break;
		}
	__GSLOGSTRING("CGSSIPModel::ProfileRegistryEventOccurred End" )
	}

// -----------------------------------------------------------------------------
// CGSSIPModel::ProfileCreatedL
// SIP profile creation event 
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ProfileCreatedL( TUint32 aSIPProfileId )
    {   
    __GSLOGSTRING("CGSSIPModel::ProfileCreatedL Start" )
    __GSLOGSTRING1("CGSSIPModel::ProfileCreatedL aSIPProfileId: %d", aSIPProfileId)
    CSIPManagedProfile* profile = NULL;
    TBool byThisApp = ETrue;
    TUint32 profileId = 0;
    if( iNewProfile )
    	{
    	TInt err = iNewProfile->GetParameter( KSIPProfileId, profileId );
    	if( err )	
    		{
    	    return;
    		}
    	}
    if ( iNewProfile && aSIPProfileId == profileId )
        {
        profile = iNewProfile;
        iNewProfile = NULL;
        }
    else
        {
        // Some other application has created a new profile
        byThisApp = EFalse;
        TRAPD( err, profile = static_cast<CSIPManagedProfile*>( 
                        iEngine->ProfileL( aSIPProfileId ) ) );
        if ( err )
            {
            // Can't fetch the new profile - nothing to do
            return;
            }
        }

    // Inserts the profile to the correct place
    TInt count = iProfiles->Count();
    TInt i( 0 );    
    TBool profileAdded( EFalse );
    TInt err( KErrNone );
    const TDesC8* providerName = NULL;
    
    err = profile->GetParameter( KSIPProviderName, providerName );
    if( !err )	
    	{
    	const TDesC8* oldProvider = NULL;
    	for ( i = 0; i < count; i++ )
        	{
        	err = iProfiles->At( i )->GetParameter( 
        		KSIPProviderName, oldProvider );
        	//if there is an error it should continue for next . 
        	if ( !err && !profileAdded
        		&& providerName->CompareC( *oldProvider ) < 0 )
            	{
            	TRAP( err, iProfiles->InsertL( i, profile ) );            
            	profileAdded = ETrue;
            	iAddedProfileIndex = i;
	            i = count;
            	}
        	}                          	
    	}
                
    // Profile's name was the 'biggest' - append it
    if ( !profileAdded )
        {
        TRAP( err, iProfiles->AppendL( profile ) );
        iAddedProfileIndex = iProfiles->Count() - 1;
        }

    if ( iQuitAfterSave && byThisApp )
        {
        // This application has created a new profile, now
        // quit the app
        iAppShutter->RequestAppShutdown();        
        }
    else
        {
        // Update the view if not exiting the application
        UpdateListCmd();
        }
    __GSLOGSTRING("CGSSIPModel::ProfileCreatedL End" )
    }        
    
// -----------------------------------------------------------------------------
// CGSSIPModel::ProfileUpdatedL
// SIP profile information update event
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ProfileUpdatedL( 
    TUint32 aSIPProfileId )
    {
    __GSLOGSTRING("CGSSIPModel::ProfileUpdatedL Start" )
    __GSLOGSTRING1("CGSSIPModel::ProfileUpdatedL aSIPProfileId: %d", aSIPProfileId)
    if ( iQuitAfterSave && aSIPProfileId == TUint( iProfileData.iID ) )
        {
        // Updated the profile that should be updated - now quit
        iAppShutter->RequestAppShutdown();        
        }
    else
        {
        // Updated profile name is not probably the same.
        // Remove from list and add it again, so it will be in right place 
        // in the list in alphabetical order
        TInt err( KErrNone );
        TInt i( 0 );
    	TUint32 profileId = 0;
    	TInt count = iProfiles->Count();    
    	TInt index( -1 );
        CSIPManagedProfile* profile = NULL;
        
        TRAP( err, profile = static_cast<CSIPManagedProfile*>( 
        	iEngine->ProfileL( aSIPProfileId ) ) );
        
        if ( err )
            {
            // Can't fetch the new profile - nothing to do
            return;
            }
        
    	for ( i = 0; i < count; i++ )
			{
			err =  iProfiles->At( i )->GetParameter(KSIPProfileId, profileId);
			if (profileId == aSIPProfileId && !err )
				{
				index = i;
				i = count;
	            }
			}
			
		if ( index >= 0 )
			{
			// first delete profile from list then add it in right place
			// CSIPProfile* profile = iProfiles->At( index );
			CSIPProfile* profileRemoved = iProfiles->At( index );
			iProfiles->Delete( index );
			delete profileRemoved;
			count--;
		
			// now add it again
		    const TDesC8* providerName = NULL;
	        TBool profileAdded( EFalse );
			err = profile->GetParameter( KSIPProviderName, providerName );
    		if( !err )	
    			{
    			const TDesC8* oldProvider = NULL;
    			for ( i = 0; i < count; i++ )
        			{
        			err = iProfiles->At( i )->GetParameter( 
        				KSIPProviderName, oldProvider );
        			
        			if ( !err && !profileAdded
        				&& providerName->CompareC( *oldProvider ) < 0 )
            			{
            			TRAP( err, iProfiles->InsertL( i, profile ) );            
            			profileAdded = ETrue;
						iAddedProfileIndex = i;
	            		i = count;
            			}
        			}
       	    	// Profile's name was the 'biggest' - append it
       	    	if ( !profileAdded )
					{
		        	TRAP( err, iProfiles->AppendL( profile ) );
			        iAddedProfileIndex = iProfiles->Count() - 1;
        			}
    			}
    		}
    		
        UpdateListCmd();
        }
    __GSLOGSTRING("CGSSIPModel::ProfileUpdatedL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ProfileRegistrationStatusChanged
// SIP profile registration status changed event
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ProfileRegistrationStatusChanged( 
    TUint32 /*aSIPProfileId*/ )
    {
    __GSLOGSTRING("CGSSIPModel::ProfileRegistrationStatusChanged" )
    UpdateListCmd();
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ProfileDestroyed
// SIP profile destruction event.
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ProfileDestroyed(
    TUint32 aSIPProfileId )
    {	
    __GSLOGSTRING("CGSSIPModel::ProfileDestroyed Start" )
    __GSLOGSTRING1("CGSSIPModel::ProfileDestroyed aSIPProfileId: %d", aSIPProfileId)
	TInt i( 0 );
    TUint32 profileId = 0;
    TInt count = iProfiles->Count();    	
	for ( i = 0; i < count; i++ )
		{
		TInt err=  iProfiles->At( i )->GetParameter(KSIPProfileId,profileId);
		if (profileId == aSIPProfileId && !err )
			{
			CSIPProfile* profile = iProfiles->At( i );
			iProfiles->Delete( i );
			delete profile;
            i = count;
            }
		}

    UpdateListCmd( EGSCmdProfileDeleted );
    __GSLOGSTRING("CGSSIPModel::ProfileDestroyed End" )
    }     	    

// -----------------------------------------------------------------------------
// CGSSIPModel::AddedProfileIndex
// Added profile index in the profiles list.
// -----------------------------------------------------------------------------
//
TInt CGSSIPModel::AddedProfileIndex()
	{
	__GSLOGSTRING("CGSSIPModel::AddedProfileIndex" )
	return iAddedProfileIndex;
	}

// -----------------------------------------------------------------------------
// CGSSIPModel::ProfileRegistryErrorOccured
// An asynchronous error has occurred related to SIP profile
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ProfileRegistryErrorOccurred(				    
	TUint32 /*aSIPProfileId*/,
	TInt /*aError*/ )
    {       
    __GSLOGSTRING("CGSSIPModel::ProfileRegistryErrorOccurred" )
    // Updates list to show that error has happened in registration
    UpdateListCmd();
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ReadArrayFromEngineL
// Reads the profile array from SIP Profile Client
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ReadArrayFromEngineL()
    {
    __GSLOGSTRING("CGSSIPModel::ReadArrayFromEngineL Start" )
    RArray<TSIPProfileTypeInfo> array;
    CleanupClosePushL( array );
    
    // Get the supported profile types
    iEngine->SupportedProfileTypesL( array );
    
    // check iProfiles, if exists delete
    if ( iProfiles )
        {
        iProfiles->ResetAndDestroy();
        delete iProfiles;
        }

    // Create the profile pointer array
    iProfiles = 
        new ( ELeave ) CArrayPtrFlat<CSIPManagedProfile>( KSIPGranularity );    

    TInt i( 0 );
    RPointerArray<CSIPProfile> profilePointerArray;
    TCleanupItem clItem( ResetAndDestroy, &profilePointerArray );
    CleanupStack::PushL( clItem );
    
    // Get all profiles based on profile types
    for ( i = 0; i < array.Count(); i++ )
        {        
        iEngine->ProfilesL( array[i], profilePointerArray );
        TInt j( 0 );
        while ( profilePointerArray.Count() > 0 )
            {
            CSIPManagedProfile* profile = 
                static_cast<CSIPManagedProfile*>( profilePointerArray[j] );
            iProfiles->AppendL( profile );
            profilePointerArray.Remove( j );
            }                    
        profilePointerArray.Reset();    
        }

    // Sort the profiles according to their ID
    // Bubble sort is slow, but because we have only few items it doesn't 
    // matter
    TInt j( 0 );
	TInt err(KErrNone);
    const TDesC8* providerName1 = NULL;
    const TDesC8* providerName2 = NULL;
    for ( i = 0; i < iProfiles->Count() - 1; i++ ) 
        {
        for ( j = 0; j < iProfiles->Count() - 1 - i; j++ )
            {
            err=  iProfiles->At( j + 1 )->GetParameter(
            	KSIPProviderName, providerName1 );
            User::LeaveIfError( err );
    		err=  iProfiles->At( j )->GetParameter( 
    			KSIPProviderName, providerName2 );
			User::LeaveIfError(	err	);  
			if ( providerName1->CompareC( *providerName2 ) < 0 )  		       			
                {  
                CSIPManagedProfile* tmp = iProfiles->At( j );
                CSIPManagedProfile* tmp2 = iProfiles->At( j + 1 );
                iProfiles->Delete( j );
                iProfiles->Delete( j );
                iProfiles->InsertL( j, tmp );
                iProfiles->InsertL( j, tmp2 );                
                }
            }
        }

    CleanupStack::PopAndDestroy();  // clItem
    CleanupStack::PopAndDestroy( &array );
    __GSLOGSTRING("CGSSIPModel::ReadArrayFromEngineL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::CopyToProfileDataStructL
// Copies profile's attributes to the internal data structure
// -----------------------------------------------------------------------------
//
void CGSSIPModel::CopyToProfileDataStructL( TInt aIndex )
    {
    __GSLOGSTRING("CGSSIPModel::CopyToProfileDataStructL Start" )
    __GSLOGSTRING1("CGSSIPModel::CopyToProfileDataStructL  aIndex: %d", aIndex)
    FillDataStructWithDefaultValsL(); 
     
    CSIPManagedProfile* profile = iProfiles->At( aIndex );
    
    GSSIPProfileUtil::CopyToTempStructL( profile, iProfileData );
    GSSIPProfileUtil::CopyServerSettingsToTempStructL(
        profile, KSIPOutboundProxy, iProfileData.iProxy );
    GSSIPProfileUtil::CopyServerSettingsToTempStructL(
        profile, KSIPRegistrar, iProfileData.iRegistrar );  
    __GSLOGSTRING("CGSSIPModel::CopyToProfileDataStructL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::FillDataStructWithDefaultValsL
// Fills the data structures fields with default values
// -----------------------------------------------------------------------------
//
void CGSSIPModel::FillDataStructWithDefaultValsL()
    {
    __GSLOGSTRING("CGSSIPModel::FillDataStructWithDefaultValsL Start" )
    HBufC* providerName = StringLoader::LoadLC( R_QTN_DEF_PROFILE_NAME );
    iProfileData.iProviderName = providerName->Des();
    CleanupStack::PopAndDestroy( providerName );
    
    iProfileData.iID = KUnknownProfileIndex;
    iProfileData.iAPIndex = KDefaultIAP;
    iProfileData.iSNAPIndex = KDefaultSNAP;
    iProfileData.iUsername = KNullDesC;
    iProfileData.iCompression = KDefaultCompression;
    iProfileData.iRegistrationMode = KDefaultRegistration;
    iProfileData.iSecurity = KDefaultSecurity;

    // Get the supported profile types
    RArray<TSIPProfileTypeInfo> typeArray;
    CleanupClosePushL( typeArray );    
    iEngine->SupportedProfileTypesL( typeArray );
    TInt count = typeArray.Count();
    // Leave if no supported profiles found.
    if ( count == KErrNone )
        {
        User::Leave( KErrNotFound );
        }
    // Find default profile type.
    iProfileData.iServiceProfile = typeArray[0];
    for ( TInt i = 0; i < count; i++ )
        {
        if ( typeArray[i].iSIPProfileClass == TSIPProfileTypeInfo::EInternet )
            {
            iProfileData.iServiceProfile = typeArray[i];
            i = count;
            }
        }
       
    CleanupStack::PopAndDestroy( &typeArray );
    
    iProfileData.iProxy.iAddress = KNullDesC;
    iProfileData.iProxy.iRealm = KNullDesC;
    iProfileData.iProxy.iUsername = KNullDesC;

    iProfileData.iProxy.iLooseRouting = KDefaultLooseRouting;
    iProfileData.iProxy.iPassword = KNullDesC;
    iProfileData.iProxy.iPortNumber = KDefaultPortNumber;
    iProfileData.iProxy.iTransportProtocol = KDefaultTranspProtocol;

    // Initial registrar server settings are the same than with proxy
    iProfileData.iRegistrar = iProfileData.iProxy;      
    iProfileData.iRegistrar.iLooseRouting = EGSLooseRoutingOff;  
    
    // Private user identity
    iProfileData.iProxy.iPrivateIdentity = KNullDesC;
    iProfileData.iRegistrar.iPrivateIdentity = KNullDesC;
    __GSLOGSTRING("CGSSIPModel::FillDataStructWithDefaultValsL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::Profile
// Returns profile based on the ID
// -----------------------------------------------------------------------------
//
CSIPManagedProfile* CGSSIPModel::Profile( TUint aId )
    {
    __GSLOGSTRING("CGSSIPModel::Profile" )
    __GSLOGSTRING1("CGSSIPModel::Profile aId: %d", aId)
    TInt i( 0 );
    CSIPManagedProfile* profile = NULL;
    
    TInt count = iProfiles->Count();
    TUint32 profileId( 0 );
    TInt err( KErrNone );
    for ( i = 0; i < count; i++ )
        {
        err = iProfiles->At( i )->GetParameter( KSIPProfileId,profileId );    	
        if ( profileId == aId && !err )
            {
            profile = iProfiles->At( i );
            i = count;
            }
        }

    return profile;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::CheckForModificationsL
// Checks for the modifications on the active profile
// -----------------------------------------------------------------------------
//
TBool CGSSIPModel::CheckForModificationsL()
    {
    __GSLOGSTRING("CGSSIPModel::CheckForModificationsL " )
    CSIPManagedProfile* profile = Profile( iProfileData.iID );

    TBool modifs = 
        GSSIPProfileUtil::ProfileModificationsL( profile, iProfileData );       
    modifs |= GSSIPProfileUtil::ServerModificationsL( 
        profile, KSIPOutboundProxy, iProfileData.iProxy );
    modifs |= GSSIPProfileUtil::ServerModificationsL(
        profile, KSIPRegistrar, iProfileData.iRegistrar );

    return modifs;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::CopyDataToProfileL
// Copies data from temporary settings structure to profile
// -----------------------------------------------------------------------------
//
CSIPManagedProfile* CGSSIPModel::CopyDataToProfileL()
    {
    __GSLOGSTRING("CGSSIPModel::CopyDataToProfileL " )
    CSIPManagedProfile* profile = NULL;

    // Get profile based on ID
    if ( iProfileData.iID == KUnknownProfileIndex )
        {
        profile = iNewProfile;
        }
    else
        {
        profile = Profile( iProfileData.iID );
        }
    
    GSSIPProfileUtil::CopyProfileAttributesL( profile, iProfileData );
    GSSIPProfileUtil::CopyServerAttributesL( 
        profile, KSIPOutboundProxy, iProfileData.iProxy );
    GSSIPProfileUtil::CopyServerAttributesL( 
        profile, KSIPRegistrar, iProfileData.iRegistrar );
    
    return profile;
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ReplaceProfileL
// Replaces one profile with another on the array; only cloned profiles
// should be replaced
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ReplaceProfileL( 
    TInt aId, 
    CSIPManagedProfile* aProfile )
    {
    __GSLOGSTRING("CGSSIPModel::ReplaceProfileL Start " )
    __GSLOGSTRING1("CGSSIPModel::ReplaceProfileL aId: %d", aId)
    TInt i( 0 );
    CSIPManagedProfile* origProfile = NULL;
    TUint32 profileId( 0 );
    TInt err( KErrNone );
    for ( i = 0; i < iProfiles->Count(); i++ )
        {
        err = iProfiles->At( i )->GetParameter( KSIPProfileId, profileId );
        if ( TUint( aId ) == profileId && !err )
            {
            origProfile = iProfiles->At( i );
            }
        }

    User::LeaveIfNull( origProfile );
    GSSIPProfileUtil::CopyProfileL(  aProfile, origProfile );
    __GSLOGSTRING("CGSSIPModel::ReplaceProfileL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ReadProfileListFromEngineSafeL
// Reads profile list from the engine; if reading fails, keeps the old list 
// safe and returns it
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ReadProfileListFromEngineSafeL()
    {    
    __GSLOGSTRING("CGSSIPModel::ReadProfileListFromEngineSafeL Start" )
    // To prevent complete disaster it we'll save the pointer of the old array
    CArrayPtrFlat<CSIPManagedProfile>* profiles = iProfiles;
    iProfiles = 0;
    TRAPD( err, ReadArrayFromEngineL() );
    
    if ( err )
        {
        // Yes, problems with re-reading profiles; use existing
        // array
        if ( iProfiles )
            {
            iProfiles->ResetAndDestroy();
            delete iProfiles;
            }
        iProfiles = profiles;
        User::Leave( err );
        }
    else
        {
        // No problems; delete backup array
        profiles->ResetAndDestroy();
        delete profiles;
        }
    __GSLOGSTRING("CGSSIPModel::ReadProfileListFromEngineSafeL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::UpdateListCmd
// Calls app ui to udpate the lists; might also trigger exit
// -----------------------------------------------------------------------------
//
void CGSSIPModel::UpdateListCmd( 
    TInt aCmd )
    {
    __GSLOGSTRING("CGSSIPModel::UpdateListCmd" )
    TRAP_IGNORE( iAppUi->HandleCommandL( aCmd ) )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::ResetAndDestroy
// For deleting RPointerArray in case of leave
// -----------------------------------------------------------------------------
//
void CGSSIPModel::ResetAndDestroy( 
    TAny* aPointerArray )
    {
    __GSLOGSTRING("CGSSIPModel::ResetAndDestroy" )
    RPointerArray<CSIPProfile>* array =
        static_cast<RPointerArray<CSIPProfile>*>( aPointerArray );
    array->ResetAndDestroy();
    array->Close();
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::CloseAndDestroy
// For deleting RArray in case of leave
// -----------------------------------------------------------------------------
//
void CGSSIPModel::CloseAndDestroy( 
    TAny* aArray )
    {
    __GSLOGSTRING("CGSSIPModel::CloseAndDestroy" )
    RArray<TSIPProfileTypeInfo>* array =
        static_cast<RArray<TSIPProfileTypeInfo>*>( aArray );
    array->Close();
    delete array;
    }
    
// -----------------------------------------------------------------------------
// CGSSIPModel::CheckDuplicateNameL
// Checks if duplicate named sip profiles. Renames if same.
// -----------------------------------------------------------------------------
//    
void CGSSIPModel::CheckDuplicateNameL( TInt32 aProfileId, TDes& aName, 
                                    TBool aCreatedFromExistingProfile)
    {
    __GSLOGSTRING("CGSSIPModel::CheckDuplicateNameL Start" )
    HBufC8* oldName = EscapeUtils::ConvertFromUnicodeToUtf8L( aName );
    CleanupStack::PushL( oldName );
    
    HBufC8* newName = 
        HBufC8::NewLC( oldName->Length() + KMaxDuplicatePostfixLength );
    newName->Des().Copy( oldName->Des() );

    // Create 16-bit TBuf for comparison and convert its digit type   
    TBuf<KMaxSIPProviderNameLength + KMaxDuplicatePostfixLength>
        oldNameBuf;
    oldNameBuf.Copy(aName);
    AknTextUtils::ConvertDigitsTo(oldNameBuf, EDigitTypeWestern);
     
    TInt count( iProfiles->Count() );
    for( TInt n = 0; n < count; n++ )
        {
        TUint32 id( KErrNone );
        iProfiles->At( n )->GetParameter( KSIPProfileId, id );
        
        const TDesC8* existingName; 
        iProfiles->At( n )->GetParameter( KSIPProviderName, existingName );   
                
        // Create 16-bit TBuf for comparison and convert its digit type      
        HBufC* tempExistingName16 = 
            EscapeUtils::ConvertToUnicodeFromUtf8L( *existingName );
        CleanupStack::PushL( tempExistingName16 );        
        TBuf<KMaxSIPProviderNameLength *3 + KMaxDuplicatePostfixLength> 
            existingNameBuf;
        existingNameBuf.Copy(tempExistingName16->Des()); 
        CleanupStack::PopAndDestroy( tempExistingName16 );           
        AknTextUtils::ConvertDigitsTo(existingNameBuf, EDigitTypeWestern);
        
        if ( id != aProfileId && !existingNameBuf.Compare( oldNameBuf ) )
            {
            TBool isUnique( EFalse );

            for( TInt i = 1; !isUnique; i++ )
                {
                TBool found( EFalse );
                    
                TBuf<KMaxDuplicatePostfixLength> postFix;
                postFix.Append( KOpenParenthesis() );
                postFix.AppendNum( ( TInt64 )i );
                postFix.Append( KClosedParenthesis() );
                
                newName->Des().Copy( *oldName );
                newName->Des().Append( postFix );
                                
                // Convert back to 16-bit descriptor 
                // because of possible Chinese/Japanese variant. 
                HBufC* newName16 = 
                    EscapeUtils::ConvertToUnicodeFromUtf8L( newName->Des() );
                CleanupStack::PushL( newName16 );                               
                
                TInt length = newName16->Length();
                if ( length > KMaxSIPProviderNameLength )
                    {
                    TInt deleteLength = length - KMaxSIPProviderNameLength;
                    TInt pos = length - postFix.Length() - deleteLength;
                    newName16->Des().Delete( pos, deleteLength );
    
                    HBufC8* newName8 = EscapeUtils::ConvertFromUnicodeToUtf8L( 
                        newName16->Des() );
                    newName->Des().Copy( newName8->Des() );
                    delete newName8;
                    }
                                        
                // Create 16-bit TBuf for comparison and convert its digit type
                TBuf<KMaxSIPProviderNameLength + KMaxDuplicatePostfixLength> 
                    newName16Buf;
                newName16Buf.Copy(newName16->Des()); 
                AknTextUtils::ConvertDigitsTo(newName16Buf, EDigitTypeWestern);                
                CleanupStack::PopAndDestroy( newName16 );
                
                for( TInt m = 0; m < count && !found; m++ )
                    {
                    TUint32 mid( KErrNone );
                    iProfiles->At( m )->GetParameter( KSIPProfileId, mid );
        
                    iProfiles->At( m )->GetParameter( 
                        KSIPProviderName, existingName );
                                    
                    // Convert the digit type again in existingNameBuf
                    HBufC* temporaryExistingName16 = 
                        EscapeUtils::ConvertToUnicodeFromUtf8L( *existingName );
                    CleanupStack::PushL( temporaryExistingName16 );
                    existingNameBuf.Copy(temporaryExistingName16->Des()); 
                    CleanupStack::PopAndDestroy( temporaryExistingName16 );   
                    AknTextUtils::ConvertDigitsTo(existingNameBuf, EDigitTypeWestern);
 
                    if( mid != aProfileId && 
                        !existingNameBuf.Compare( newName16Buf ) )
                        {
                        found = ETrue;
                        }                      
                    }
                    
                if( !found )
                    {
                    isUnique = ETrue;
                    }
                }
            }
        }    

    HBufC* tempName = 
        EscapeUtils::ConvertToUnicodeFromUtf8L( newName->Des() );
    CleanupStack::PushL( tempName );    
    
    if (aCreatedFromExistingProfile)
        {
        // Convert numbers to language specific numbers.
        TPtr ptr = tempName->Des();
        AknTextUtils::LanguageSpecificNumberConversion( ptr ); 
        }
  
    aName.Copy( tempName->Des() );
    CleanupStack::PopAndDestroy( tempName );  
       
    CleanupStack::PopAndDestroy( newName );
    CleanupStack::PopAndDestroy( oldName ); 
    __GSLOGSTRING("CGSSIPModel::CheckDuplicateNameL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::CheckProfileForDeleteL
// Checks if profile can be deleted.
// -----------------------------------------------------------------------------
//    
void CGSSIPModel::CheckProfileForDeleteL( 
            TInt aIndex, TBool& aIsUse, TBool& aDefault, TBool& aLocked )
    {
    __GSLOGSTRING("CGSSIPModel::CheckProfileForDeleteL Start" )
    CSIPManagedProfile* profile = iProfiles->At( aIndex );    
    
    if ( profile )
        {
        profile->GetParameter( KSIPDefaultProfile, aDefault );
        profile->GetParameter( KSIPProfileLocked, aLocked );
        aIsUse = iEngine->IsInUseL( *profile );
        } 
    __GSLOGSTRING("CGSSIPModel::CheckProfileForDeleteL End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPModel::CheckIsProfileInUseL
// Checks if profile is used.
// -----------------------------------------------------------------------------
//    
TBool CGSSIPModel::CheckIsProfileInUseL( TUint32 aProfileId )
    {
    __GSLOGSTRING("CGSSIPModel::CheckIsProfileInUseL" )
    TBool inUse( EFalse );
    CSIPManagedProfile* profile = Profile( aProfileId );
    
    if ( profile )
        {
        profile->GetParameter( KSIPProfileRegistered, inUse );
        }
    
    return inUse;    
    }

//  End of File  

