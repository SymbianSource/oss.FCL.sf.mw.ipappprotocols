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
* Description:  Model SIPSettings plugin
*
*/



// INCLUDE FILES
#include    <aknnotewrappers.h>
#include    <e32std.h>
#include    <escapeutils.h>
#include    <StringLoader.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    "sipsettingsmodel.h"
#include    "gssipmodel.h"
#include    "mgssipprofilehandler.h"
#include    "gssippluginlogger.h"
#include    "gssippluginlogger.h"

// CONSTANTS
const TInt KSIPGranularity = 5;
_LIT( KSIPLockedIcon, "0\t" );
_LIT( KSIPEmptyIcon, "1\t" );
_LIT( KTab, "\t" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettingsModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettingsModel* CSIPSettingsModel::NewL( MGSSIPProfileHandler* aHandler )
    {
    __GSLOGSTRING("CSIPSettingsModel::NewL" )
    CSIPSettingsModel* self =
        new ( ELeave ) CSIPSettingsModel( aHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// -----------------------------------------------------------------------------
// CSIPSettingsModel::CSIPSettingsModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettingsModel::CSIPSettingsModel( MGSSIPProfileHandler* aHandler )
	: iHandler( aHandler )
    {    
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettingsModel::CSIPSettingsModel()" )
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettingsModel::ConstructL()
    {
    __GSLOGSTRING("CSIPSettingsModel::ConstructL" )
    iRegisteredTxt = StringLoader::LoadL( R_QTN_PROFILE_LIST_PROFILE_REG );    
    iNotRegisteredTxt = StringLoader::LoadL(
    	R_QTN_PROFILE_LIST_PROFILE_NOT_REG );
    iErrorInReg = StringLoader::LoadL( R_QTN_PROFILE_LIST_PROFILE_REG_FAILED );
    }

// Destructor
CSIPSettingsModel::~CSIPSettingsModel()
    {
    __GSLOGSTRING( "[GSSIPSettingsPlugin] CSIPSettingsModel::~CSIPSettingsModel()" )
    delete iRegisteredTxt;
    delete iNotRegisteredTxt;    
    delete iErrorInReg;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::DefaultProfileIndex
// Returns the default profile index
// -----------------------------------------------------------------------------
//
TInt CSIPSettingsModel::DefaultProfileIndex()
    {
    TInt index = iHandler->DefaultProfileIndex();    
    __GSLOGSTRING1("CSIPSettingsModel::DefaultProfileIndex index: %d", index)
	return index;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::SetDefaultProfileL
// Sets the new default profile
// -----------------------------------------------------------------------------
//
void CSIPSettingsModel::SetDefaultProfileL( 
    TInt aIndex )
    {
    __GSLOGSTRING1("CSIPSettingsModel::SetDefaultProfileL index: %d", aIndex)
    TRAPD(err, iHandler->SetDefaultProfileL( aIndex ));		
      if ( err == KErrInUse )
    	  {
          HBufC* txtErr = StringLoader::LoadLC( R_QTN_SIP_ERROR_PROFILE_USED );
          CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
          note->ExecuteLD( txtErr->Des() );
          CleanupStack::PopAndDestroy( txtErr );
    	  }
      else
    	  {
          User::Leave( err );
    	  }
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::ListOfProfileNamesL
// Returns the list of profile names
// -----------------------------------------------------------------------------
CDesCArray* CSIPSettingsModel::ListOfProfileNamesL()
    {
    __GSLOGSTRING("CSIPSettingsModel::ListOfProfileNamesL " )
    CArrayPtr<CSIPManagedProfile>* list = iHandler->ProfileArray();
    
    CDesCArray* array = new ( ELeave ) CDesCArrayFlat( KSIPGranularity );
	CleanupStack::PushL( array );
	TInt err(KErrNone);
    
    for ( TInt i = 0; i < list->Count(); i++ )
        {
        const TDesC8* providerName = 0;
        err = list->At(i)->GetParameter( KSIPProviderName, providerName );
        User::LeaveIfError( err );
    
        HBufC8* decodedProvider =
            EscapeUtils::EscapeDecodeL( *providerName );
        CleanupStack::PushL( decodedProvider );    
        
        HBufC* providerName16 = 
        	EscapeUtils::ConvertToUnicodeFromUtf8L( decodedProvider->Des() );
        CleanupStack::PushL( providerName16 );
        
        array->AppendL( providerName16->Des() );
        
        CleanupStack::PopAndDestroy( providerName16 );
        CleanupStack::PopAndDestroy( decodedProvider );
        }

    CleanupStack::Pop( array );
	return array;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::ProfileNameLC
// Returns the name of the queried profile
// -----------------------------------------------------------------------------
//
HBufC* CSIPSettingsModel::ProfileNameLC( TInt aIndex )
    {  
    __GSLOGSTRING("CSIPSettingsModel::ProfileNameLC" )
	const TDesC8* providerName = 0;
    TInt err = iHandler->ProfileArray()->At( aIndex )->
        GetParameter( KSIPProviderName, providerName );
    User::LeaveIfError( err );

    HBufC8* decodedProvider =
            EscapeUtils::EscapeDecodeL( *providerName );
    CleanupStack::PushL( decodedProvider );
    HBufC* buf = EscapeUtils::ConvertToUnicodeFromUtf8L( *providerName );
    CleanupStack::PopAndDestroy( decodedProvider );
    CleanupStack::PushL( buf );
  	return buf;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::NumOfProfiles
// Returns the number of the profiles
// -----------------------------------------------------------------------------
//
TInt CSIPSettingsModel::NumOfProfiles()
    {
    return MdcaCount();
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::MdcaCount
// Returns the number of list items
// -----------------------------------------------------------------------------
//
TInt CSIPSettingsModel::MdcaCount() const
    {
    return iHandler->ProfileArray()->Count();
    }
	
// -----------------------------------------------------------------------------
// CSIPSettingsModel::MdcaPoint
// Returns the list item pointed by an index
// -----------------------------------------------------------------------------
//
TPtrC16 CSIPSettingsModel::MdcaPoint(
    TInt aIndex ) const
    {
    __GSLOGSTRING("CSIPSettingsModel::MdcaPoint" )
    iBuf.Zero();
    
    // Get profile locked information.
    TBool locked( EFalse );
	TInt err = iHandler->ProfileArray()->At( aIndex )->
	    GetParameter( KSIPProfileLocked, locked );
	
	// Set icon index information for secure icon.
	if ( err == KErrNone && locked )
	    {
	    iBuf.Append( KSIPLockedIcon );
	    }
	else
		{
		iBuf.Append( KSIPEmptyIcon );
	    }
    
    const TDesC8* providerName = 0;
    err = iHandler->ProfileArray()->At( aIndex )->
        GetParameter( KSIPProviderName, providerName );
   
    HBufC8* decodedProvider = NULL;
    TRAP( err,
    	decodedProvider = EscapeUtils::EscapeDecodeL( *providerName ) );
   
    if ( err == KErrNone )
    	{
	    HBufC* name = NULL;
	    TRAP( err, name = 
    	    EscapeUtils::ConvertToUnicodeFromUtf8L( *decodedProvider ) )

    	if ( err == KErrNone && name )
    		{
        	iBuf.Append( *name );
        	delete name;
        	name = NULL;
        	}
		}
    delete decodedProvider;
    
    iBuf.Append( KTab );

    TBool bStatus( EFalse );
	err = iHandler->ProfileArray()->At( aIndex )->
		GetParameter(KSIPProfileRegistered, bStatus);
    
    if( err || iHandler->ErrorInRegistration( aIndex ) )
    	{    	
    	iBuf.Append( iErrorInReg->Des() );
    	} 
	else
		{
		if( bStatus )
			{
			iBuf.Append( iRegisteredTxt->Des() );		
			}
		else
			{
			iBuf.Append( iNotRegisteredTxt->Des() );	
			}
		}
    
    return iBuf;
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::CreateNewProfileL
// Creates new profile and prepares it for editing
// -----------------------------------------------------------------------------
//
void CSIPSettingsModel::CreateNewProfileL(
    TInt aIndex )
    {
    __GSLOGSTRING("CSIPSettingsModel::CreateNewProfileL" )
  	iHandler->CreateAndEditProfileL( aIndex );
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::EditProfileL
// Prepares the selected profile for editing
// -----------------------------------------------------------------------------
//
TBool CSIPSettingsModel::EditProfileL( TInt aIndex )
    {
    __GSLOGSTRING("CSIPSettingsModel::EditProfileL" )
 	return iHandler->EditProfileL( aIndex );
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::DeleteProfileL
// Deletes profile
// -----------------------------------------------------------------------------
//
void CSIPSettingsModel::DeleteProfileL( TInt aIndex )
    {
    __GSLOGSTRING("CSIPSettingsModel::DeleteProfileL" )
    iHandler->DeleteProfileL( aIndex );
    }

// -----------------------------------------------------------------------------
// CSIPSettingsModel::CheckProfileForDeleteLC
// Checks if profile can be deleted.
// -----------------------------------------------------------------------------
//    
void CSIPSettingsModel::CheckProfileForDeleteL( 
    TInt aIndex, TBool& aIsUse, TBool& aDefault, TBool& aLocked )
    {
	(static_cast<CGSSIPModel*>( iHandler ) )->
        CheckProfileForDeleteL( aIndex, aIsUse, aDefault, aLocked );
    }
//  End of File  
