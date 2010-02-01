
// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDES
#include    "SIPExProfileQueryDlg.h"
#include    "SIPEx.hrh"
#include    <sipex.rsg>
#include    <eikenv.h>
#include 	<eikchlst.h>
#include    <uri8.h>
#include    <commdb.h>
#include    "ProfileUtil.h"


// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::NewL
// Creates the class instance and returns it.
// -----------------------------------------------------------------------------
//
CSIPExProfileQueryDlg* CSIPExProfileQueryDlg::NewL()
    {
    CSIPExProfileQueryDlg* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::NewLC
// Creates the class instance and returns it.
// On return the instance is left to the CleanupStack.
// -----------------------------------------------------------------------------
//
CSIPExProfileQueryDlg* CSIPExProfileQueryDlg::NewLC( )
    {
    CSIPExProfileQueryDlg* self = new (ELeave) CSIPExProfileQueryDlg();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::CSIPExProfileQueryDlg
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CSIPExProfileQueryDlg::CSIPExProfileQueryDlg()
    {
    }

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::~CSIPExProfileQueryDlg
// Desctructor.
// -----------------------------------------------------------------------------
//
CSIPExProfileQueryDlg::~CSIPExProfileQueryDlg()
    {
    delete iMProfileRegistry;
	iAPsUIDs.Close();
	
	delete iNewProfile;
    }
    
// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::ProfileRegistryEventOccurred
// SIP profile event 
// -----------------------------------------------------------------------------
//
void CSIPExProfileQueryDlg::ProfileRegistryEventOccurred(TUint32 aProfileId, TEvent aEvent)
	{
	switch(aEvent)	
		{
		case EProfileCreated:
			ProfileCreated(aProfileId);
			break;
		case EProfileUpdated:	
		case EProfileRegistered:						
		case EProfileDeregistered:
		case EProfileDestroyed:
		default:
				//don't know what to do . Nothing can be done !!!
			break;
		}
	}
    
// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::ProfileRegistryErrorOccured
// An asynchronous error has occurred related to SIP profile
// -----------------------------------------------------------------------------
//
void CSIPExProfileQueryDlg::ProfileRegistryErrorOccurred(				    
	TUint32 /*aSIPProfileId*/,
	TInt /*aError*/)
    {  
    //TInt err = aError;  
    //TUint32 profId = aSIPProfileId;
    }


// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::ProfileCreated
// SIP profile creation event 
// -----------------------------------------------------------------------------
//
void CSIPExProfileQueryDlg::ProfileCreated( TUint32 aSIPProfileId )
    {   
    CSIPManagedProfile* profile = NULL;
    TUint32 profileId = 0;
    if(iNewProfile)
    	{
    	TInt err= iNewProfile->GetParameter(KSIPProfileId,profileId);
    	if(err)	
    		{
    			//don't know what to do . perhaps nothing can be done 
    			return ;
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
         TRAPD( err, profile = static_cast<CSIPManagedProfile*>( 
                        iMProfileRegistry->ProfileL( aSIPProfileId ) ) );
        if ( err )
            {
            // Can't fetch the new profile - nothing to do
            return;
            }
        }

    }

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::ConstructL
// Symbian 2nd phase constructor that might leave.
// -----------------------------------------------------------------------------
//
void CSIPExProfileQueryDlg::ConstructL()
    {
   	iMProfileRegistry = CSIPManagedProfileRegistry::NewL( *this );

    FillDataStructWithDefaultValsL();
    }

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::ProfileData
// Saves profile to persistent storage
// -----------------------------------------------------------------------------
//
void CSIPExProfileQueryDlg::SaveProfileL()
    {
    // Need to create a new profile
    CSIPManagedProfile* profile = 
    				iMProfileRegistry->CreateL(iProfileData.iServiceProfile);
    CleanupStack::PushL( profile );

	// Copy attributes to the new profile        
    iNewProfile = profile;
    profile = CopyDataToProfileL();

	// Set the profile to be default and save sum' settings
    TInt err= profile->SetParameter(KSIPDefaultProfile,ETrue);
    User::LeaveIfError(err);

    // Save the profile to persistent storage                
    iMProfileRegistry->SaveL( *profile );
    
    HBufC* txt = iEikonEnv->AllocReadResourceLC( R_PROFILE_SAVED );
    CEikonEnv::Static()->InfoMsg( txt->Des() );
    CleanupStack::PopAndDestroy( txt );
            
    CleanupStack::Pop();    // profile
   	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::CopyDataToProfileL
// Copies data from temporary settings structure to profile
// -----------------------------------------------------------------------------
//
CSIPManagedProfile* CSIPExProfileQueryDlg::CopyDataToProfileL()
    {
    CSIPManagedProfile* profile = iNewProfile;
        

    ProfileUtil::CopyProfileAttributesL( profile, iProfileData );
    ProfileUtil::CopyServerAttributesL( 
        profile, KSIPOutboundProxy, iProfileData.iProxy );
    ProfileUtil::CopyServerAttributesL( 
        profile, KSIPRegistrar, iProfileData.iRegistrar );
    
    return profile;
    }

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::PreLayoutDynInitL
// From CEikDialog. Initializes the dialog's text control with default address
// if available.
// -----------------------------------------------------------------------------
//
void CSIPExProfileQueryDlg::PreLayoutDynInitL()
    {
    if ( iProfileData.iUsername.Length() > 0 )
        {
        ( static_cast<CEikEdwin*>( 
            Control( EItemUsername ) ) )->SetTextL( &iProfileData.iUsername );
        }
    if ( iProfileData.iProxy.iAddress.Length() > 0 )
        {
        ( static_cast<CEikEdwin*>( 
            Control( EProxyServerItemAddr ) ) )->SetTextL( &iProfileData.iProxy.iAddress );
        }
    if ( iProfileData.iRegistrar.iAddress.Length() > 0 )
        {
        ( static_cast<CEikEdwin*>( 
            Control( ERegistrarServerItemAddr ) ) )->SetTextL( &iProfileData.iRegistrar.iAddress );
        }
    // create accesspointid list
    
    CEikChoiceList* choiceList = static_cast<CEikChoiceList*>(Control(EItemAccesspointid));
	
	GetAPListL();

    choiceList->SetArrayL(static_cast<MDesCArray*>(iAccesspointids));
    
    }

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::OkToExitL
// From CEikDialog.
// Validates the given address and if not correct notifies user with info
// message.
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::OkToExitL( TInt aKeyCode )
    {
    TBool isOk( ETrue );
    
    if ( aKeyCode == EEikBidOk )
        {
        if ( !SetProfileUserNameL( ))
        	{
        	ShowErrorInfoL(EItemUsername);
        	isOk = EFalse;
        	}
    	if ( !SetProfileCompressionL())
    		{
        	ShowErrorInfoL(EItemCompression);
        	isOk = EFalse;
        	}
    	if ( !SetProfileSecurityL())
    		{
        	ShowErrorInfoL(EItemSecurity);
        	isOk = EFalse;
        	}
        if ( !SetProfileIAPIdL())
        	{
        	ShowErrorInfoL(EItemAccesspointid);
        	isOk = EFalse;
        	}
    	if ( !SetProfileProxyAddressL() || !SetProfileProxyRealmL()
    		|| !SetProfileProxyUNameL() || !SetProfileProxyPwL() )
    		{
        	ShowErrorInfoL(EItemProxyServer);
        	isOk = EFalse;
        	}
    	if ( !SetProfileRegistrarAddressL() || !SetProfileRegistrarRealmL()
    		|| !SetProfileRegistrarUNameL() || !SetProfileRegistrarPwL() )
    		{
        	ShowErrorInfoL(EItemRegistrarServer);
        	isOk = EFalse;
        	}
    	if (isOk)
    		{
    		SaveProfileL();	
    		}
    	}
    else //if (aKeyCode == EEikBidCancel )
    	{
    	}
       
    return isOk;
    }
    
// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::FillDataStructWithDefaultValsL()
// Fills the data structures fields with default values
// -----------------------------------------------------------------------------
//
void CSIPExProfileQueryDlg::FillDataStructWithDefaultValsL()
	{
    CEikonEnv* env = CEikonEnv::Static();
    
    iProfileData.iProviderName = 
        env->AllocReadResourceLC( R_SIP_EX_DEF_PROFILE_NAME )->Des();    
    CleanupStack::PopAndDestroy();  // providerName    
	
	iProfileData.iID = KUnknownProfileIndex;
    iProfileData.iAPIndex = KDefaultIAP;
    iProfileData.iCompression = KDefaultCompression;
    iProfileData.iRegistrationMode = KDefaultRegistration;
    iProfileData.iSecurity = KDefaultSecurity;

    iProfileData.iUsername = 
        env->AllocReadResourceLC( R_SIP_EX_DEF_USERNAME )->Des();     
    CleanupStack::PopAndDestroy();  // userName
    
    RArray<TSIPProfileTypeInfo> typeArray;
    CleanupClosePushL( typeArray );    
    // Get the supported profile types
    iMProfileRegistry->SupportedProfileTypesL( typeArray );

    if ( typeArray.Count() > 0 )
        {
        iProfileData.iServiceProfile = typeArray[0];
        }
    else
        {        
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy();  // typeArray
    
    
    iProfileData.iProxy.iAddress = 
        env->AllocReadResourceLC( R_SIP_EX_DEF_SERVER_ADDRESS )->Des();
    CleanupStack::PopAndDestroy();  // addr               
    
    iProfileData.iProxy.iRealm = KEmptyString;
    
    iProfileData.iProxy.iUsername = KEmptyString;

    iProfileData.iProxy.iLooseRouting = KDefaultLooseRouting;
    iProfileData.iProxy.iPassword = KEmptyString;
    iProfileData.iProxy.iPortNumber = KDefaultPortNumber;
    iProfileData.iProxy.iTransportProtocol = KDefaultTranspProtocol;

    // Initial registrar server settings are the same than with proxy
    iProfileData.iRegistrar = iProfileData.iProxy;      
    iProfileData.iRegistrar.iLooseRouting = ELooseRoutingOff;  	
	}
		
// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileUserNameL
// Sets iPublicUName from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileUserNameL( )
	{
	TBool isOk( ETrue );
    ( static_cast< CEikEdwin* >( 
    				Control( EItemUsername ) ) )->GetText( iProfileData.iUsername );

    // Check the validity of the given address
   // TInt err( KErrGeneral );
        
    HBufC8* address = HBufC8::NewLC( iProfileData.iUsername.Length() );
    address->Des().Copy( iProfileData.iUsername );
        
    CleanupStack::PopAndDestroy( address );
    return isOk;
	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileCompressionL
// Sets iCompression from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileCompressionL( )
	{
	TBool isOk( ETrue );
    iProfileData.iCompression = ( static_cast< CEikChoiceList* >( 
    				Control( EItemCompression ) ) )->CurrentItem();
    return isOk;
	}
	
// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileIAPIdL
// Sets access point Id.
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileIAPIdL( )
	{
	TBool isOk( ETrue );
    iProfileData.iAPIndex = iAPsUIDs[( static_cast< CEikChoiceList* >( 
    				Control( EItemAccesspointid ) ) )->CurrentItem()];
    return isOk;
	}
	
// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileSecurityL
// Sets iSecurity from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileSecurityL( )
	{
	TBool isOk( ETrue );
    iProfileData.iSecurity = ( static_cast< CEikChoiceList* >( 
    				Control( EItemSecurity ) ) )->CurrentItem();
	
	return isOk;
	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileProxyAddressL
// Sets iProxyAddress from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileProxyAddressL( )
	{
	TBool isOk( ETrue );

	HBufC* defBuf = CEikonEnv::Static()->AllocReadResourceLC(
        R_SIP_EX_DEF_SERVER_ADDRESS );

	( static_cast<CEikEdwin*>(Control( EProxyServerItemAddr ) ) )->GetText(
    											 iProfileData.iProxy.iAddress);
		
    if (iProfileData.iProxy.iAddress == defBuf->Des())
    	{
    	iProfileData.iProxy.iAddress = KNullDesC;
    	}
    CleanupStack::PopAndDestroy(); // defBuf
    
	return isOk;
	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileProxyRealmL
// Sets iProxyRealm from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileProxyRealmL( )
	{
	TBool isOk( ETrue );
    ( static_cast< CEikEdwin* >( 
    				Control( EProxyServerItemRealm ) ) )->GetText( iProfileData.iProxy.iRealm );
	
	return isOk;
	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileProxyUNameL
// Sets iProxyUName from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileProxyUNameL( )
	{
	TBool isOk( ETrue );
    ( static_cast< CEikEdwin* >( 
    				Control( EProxyServerItemUsername ) ) )->GetText( iProfileData.iProxy.iUsername );
	
	return isOk;
	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileProxyPwL
// Sets iProxyPw from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileProxyPwL( )
	{
	TBool isOk( ETrue );
    ( static_cast< CEikEdwin* >( 
    				Control( EProxyServerItemPassword ) ) )->GetText( iProfileData.iProxy.iPassword );
	
	return isOk;
	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileRegistrarAddressL
// Sets iRegistrarAddress from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileRegistrarAddressL( )
	{
	TBool isOk( ETrue );
	HBufC* defBuf = CEikonEnv::Static()->AllocReadResourceLC(
        R_SIP_EX_DEF_SERVER_ADDRESS );


    ( static_cast< CEikEdwin* >( 
    				Control( ERegistrarServerItemAddr ) ) )->GetText( iProfileData.iRegistrar.iAddress );
	
	if (iProfileData.iRegistrar.iAddress == defBuf->Des())
    	{
    	iProfileData.iRegistrar.iAddress = KNullDesC;
    	}
    CleanupStack::PopAndDestroy(); // defBuf

	return isOk;
	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileRegistrarRealmL
// Sets iRegistrarRealm from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileRegistrarRealmL( )
	{
	TBool isOk( ETrue );
    ( static_cast< CEikEdwin* >( 
    				Control( ERegistrarServerItemRealm ) ) )->GetText( iProfileData.iRegistrar.iRealm );
	
	return isOk;
	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileRegistrarUNameL
// Sets iRegistrarUName from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileRegistrarUNameL( )
	{
	TBool isOk( ETrue );
    ( static_cast< CEikEdwin* >( 
    				Control( ERegistrarServerItemUsername ) ) )->GetText( 
    				iProfileData.iRegistrar.iUsername );
	
	return isOk;
	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::SetProfileRegistrarPwL
// Sets iRegistrarPw from CEikDialog .
// -----------------------------------------------------------------------------
//
TBool CSIPExProfileQueryDlg::SetProfileRegistrarPwL( )
	{
	TBool isOk( ETrue );
    ( static_cast< CEikEdwin* >( 
    				Control( ERegistrarServerItemPassword ) ) )->GetText( 
    				iProfileData.iRegistrar.iPassword );
	
	return isOk;
	}

// -----------------------------------------------------------------------------
// CSIPExProfileQueryDlg::ShowErrorInfo
// Informs which part is not valid
// -----------------------------------------------------------------------------
//
void CSIPExProfileQueryDlg::ShowErrorInfoL(TProfileSettingItemListItems aItem)
	{
	HBufC* txt =0;
	switch (aItem)
		{
		case EItemUsername:
			txt = iEikonEnv->AllocReadResourceLC( R_ERROR_IN_USERNAME_TXT );
    		break;
		case EItemCompression:
			txt = iEikonEnv->AllocReadResourceLC( R_ERROR_IN_COMPRESSION_TXT );
    		break;
		case EItemSecurity:
			txt = iEikonEnv->AllocReadResourceLC( R_ERROR_IN_SECURITY_TXT );
    		break;
		case EItemProxyServer:
			txt = iEikonEnv->AllocReadResourceLC( R_ERROR_IN_PROXY_TXT );
    		break;
		case EItemRegistrarServer:
			txt = iEikonEnv->AllocReadResourceLC( R_ERROR_IN_REGISTRAR_TXT );
    		break;
    	default:
			txt = iEikonEnv->AllocReadResourceLC( R_ERROR_IN_GENERAL_TXT );
    		break;
    	}

  		CEikonEnv::Static()->InfoMsg( txt->Des() );
   		CleanupStack::PopAndDestroy( txt );
	}

// -----------------------------------------------------------------------------
// CSIPExAddrQueryDlg::AddressValid
// Checks if user typed address is valid address.
// -----------------------------------------------------------------------------
//    
TBool CSIPExProfileQueryDlg::AddressValid( const TDesC8& aAddr )
    {
    
    _LIT8( KTypeSIP, "sip" );
    
    TUriParser8 parser;
    User::LeaveIfError( parser.Parse( aAddr ) ); 
    CUri8* uri8 = CUri8::NewLC( parser );
    
    TBool valid( ETrue );
    
    if ( uri8->Uri().Extract( EUriScheme ).CompareF( KTypeSIP() ) != KErrNone )
        {
        valid = EFalse;
        }
    if ( uri8->Uri().Extract( EUriUserinfo ) == KNullDesC8 )
        {
        valid = EFalse;
        }
    if ( uri8->Uri().Extract( EUriHost ) == KNullDesC8 )
        {
        valid = EFalse;
        }
    CleanupStack::PopAndDestroy( uri8 );
        
    return valid;        
    }
    
void CSIPExProfileQueryDlg::GetAPListL()
	{
	if (iAccesspointids)
		{
    	iAccesspointids->Reset();
    	delete iAccesspointids;
 		}
	iAccesspointids = new (ELeave) CDesCArraySeg(100);
	
	CCommsDatabase* db = CCommsDatabase::NewL();
    CleanupStack::PushL( db );
//	TUint i( 0 );
    iAPsUIDs.Reset();
    CCommsDbTableView* comView = db->OpenTableLC( TPtrC( IAP ) );
    TInt error1 = comView->GotoFirstRecord();
    while ( error1 == KErrNone )
        {
        TUint32 id( 0 );

        TBuf< KCommsDbSvrMaxColumnNameLength > tmpName;
        
        comView->ReadUintL( TPtrC( COMMDB_ID ), id );
		comView->ReadTextL( TPtrC( COMMDB_NAME ), tmpName);
		
        iAccesspointids->AppendL(tmpName);
        
       // i++;    // To next enumeration   
        User::LeaveIfError( iAPsUIDs.Append( id ) );
        error1 = comView->GotoNextRecord();
        }
    CleanupStack::PopAndDestroy( comView );  // table
	
	//TInt apCount( iAPsUIDs.Count() );
    CleanupStack::PopAndDestroy( db );   // db

	}

// End of file
