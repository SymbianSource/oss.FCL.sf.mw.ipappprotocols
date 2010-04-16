/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


//  INTERNAL INCLUDES

#include "ut_gssipprofileutil.h"
#include "gssipprofileutil.h"
#include "sipprofileregistry.h"
#include "sipprofile.h"
#include "TSIPSettingsData.h"
#include "TGSSIPServerData.h"
#include <sipmanagedprofile.h>
#include <SIPManagedProfileRegistry.h>
#include <sipprofiletypeinfo.h>

//  SYSTEM INCLUDES
#include <digia/eunit/eunitmacros.h>



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_GSSIPProfileUtil* UT_GSSIPProfileUtil::NewL()
    {
    UT_GSSIPProfileUtil* self = UT_GSSIPProfileUtil::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_GSSIPProfileUtil* UT_GSSIPProfileUtil::NewLC()
    {
    UT_GSSIPProfileUtil* self = new( ELeave ) UT_GSSIPProfileUtil();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
UT_GSSIPProfileUtil::~UT_GSSIPProfileUtil()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
UT_GSSIPProfileUtil::UT_GSSIPProfileUtil()
    {
    // NOP
    }


// -----------------------------------------------------------------------------
// Second phase construct
// -----------------------------------------------------------------------------
//
void UT_GSSIPProfileUtil::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_GSSIPProfileUtil::SetupL()
    {
    iProfileUtil = new(ELeave)GSSIPProfileUtil;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void UT_GSSIPProfileUtil::Teardown()
    {
    if (iProfileUtil != NULL)
        {
        delete iProfileUtil;
        iProfileUtil = NULL;
        }
    }



// TEST CASES

// -----------------------------------------------------------------------------
// UT_GSSIPProfileUtil_ProfileModificationsLL()
// -----------------------------------------------------------------------------
//
void UT_GSSIPProfileUtil::UT_GSSIPProfileUtil_ProfileModificationsLL()
    {
    CSIPProfileRegistryObserver* registryObserver = 
                CSIPProfileRegistryObserver::NewLC();
    
    CSIPManagedProfileRegistry* profileRegistry = 
    		CSIPManagedProfileRegistry::NewLC(*registryObserver);

    CSIPManagedProfile* managedProfile = CSIPManagedProfile::NewLC(profileRegistry);

    TSIPSettingsData settingsData;

	// Test case 1: No changes
	{	
		HBufC16* providerName = HBufC16::NewLC(25);
		TPtr namePtr (providerName->Des());
		namePtr.Copy( KProviderNameValue );
	    settingsData.iProviderName = *providerName;
		CleanupStack::PopAndDestroy (providerName);

		HBufC16* userAor = HBufC16::NewLC(25);
		TPtr aorPtr (userAor->Des());
		aorPtr.Copy( KUserAorValue );
	    settingsData.iUsername = *userAor;
		CleanupStack::PopAndDestroy (userAor);

		settingsData.iSNAPIndex = KDefaultSNAP;
		settingsData.iAPIndex = KAccessPointIdValue;
		settingsData.iCompression = KSigCompValue;
		settingsData.iSecurity = KSecurityNegotiationValue;
		settingsData.iServiceProfile.iSIPProfileName = KProviderNameValue;
		settingsData.iServiceProfile.iSIPProfileClass = 
			TSIPProfileTypeInfo::EInternet;
		managedProfile->SetEnabled(EFalse); // Disable SNAP

	    TBool hasChanged;
	    hasChanged = 
	    	iProfileUtil->ProfileModificationsL(managedProfile, settingsData);

		EUNIT_ASSERT_EQUALS( hasChanged, EFalse ); 
	}

	
	// Test case 2: SNAP is used, IAP changes
 	{
		HBufC16* providerName = HBufC16::NewLC(25);
		TPtr namePtr (providerName->Des());
		namePtr.Copy( KProviderNameValue );
	    settingsData.iProviderName = *providerName;
		CleanupStack::PopAndDestroy (providerName);

		HBufC16* userAor = HBufC16::NewLC(25);
		TPtr aorPtr (userAor->Des());
		aorPtr.Copy( KUserAorValue );
	    settingsData.iUsername = *userAor;
		CleanupStack::PopAndDestroy (userAor);

		settingsData.iSNAPIndex = KSnapIdValue;
		settingsData.iAPIndex = KAccessPointIdValue + 1;
		settingsData.iCompression = KSigCompValue;
		settingsData.iSecurity = KSecurityNegotiationValue;
		settingsData.iServiceProfile.iSIPProfileName = KProviderNameValue;
		settingsData.iServiceProfile.iSIPProfileClass = 
										TSIPProfileTypeInfo::EInternet;
				
	 	managedProfile->SetEnabled(ETrue); // Enable SNAP
		
	    TBool hasChanged;
	    hasChanged = iProfileUtil->ProfileModificationsL(managedProfile, settingsData);
		EUNIT_ASSERT_EQUALS( hasChanged, EFalse ); 
 	}

	// Test case 3: SNAP isn't used, IAP changes
 	{
		HBufC16* providerName = HBufC16::NewLC(25);
		TPtr namePtr (providerName->Des());
		namePtr.Copy( KProviderNameValue );
	    settingsData.iProviderName = *providerName;
		CleanupStack::PopAndDestroy (providerName);

		HBufC16* userAor = HBufC16::NewLC(25);
		TPtr aorPtr (userAor->Des());
		aorPtr.Copy( KUserAorValue );
	    settingsData.iUsername = *userAor;
		CleanupStack::PopAndDestroy (userAor);

		settingsData.iSNAPIndex = KDefaultSNAP;
		settingsData.iAPIndex = KAccessPointIdValue + 1;
		settingsData.iCompression = KSigCompValue;
		settingsData.iSecurity = KSecurityNegotiationValue;
		settingsData.iServiceProfile.iSIPProfileName = KProviderNameValue;
		settingsData.iServiceProfile.iSIPProfileClass = TSIPProfileTypeInfo::EInternet;
		
	    TBool hasChanged;
		managedProfile->SetEnabled(EFalse); // Disable SNAP
	    hasChanged = iProfileUtil->ProfileModificationsL(managedProfile, settingsData);
		EUNIT_ASSERT_EQUALS( hasChanged, ETrue ); 
 	}
	

	// Test case 4: SNAP is used and SNAP id changes
 	{
		HBufC16* providerName = HBufC16::NewLC(25);
		TPtr namePtr (providerName->Des());
		namePtr.Copy( KProviderNameValue );
	    settingsData.iProviderName = *providerName;
		CleanupStack::PopAndDestroy (providerName);

		HBufC16* userAor = HBufC16::NewLC(25);
		TPtr aorPtr (userAor->Des());
		aorPtr.Copy( KUserAorValue );
	    settingsData.iUsername = *userAor;
		CleanupStack::PopAndDestroy (userAor);

		settingsData.iSNAPIndex = KSnapIdValue + 1;
		settingsData.iAPIndex = KAccessPointIdValue + 1;
		settingsData.iCompression = KSigCompValue;
		settingsData.iSecurity = KSecurityNegotiationValue;
		settingsData.iServiceProfile.iSIPProfileName = KProviderNameValue;
		settingsData.iServiceProfile.iSIPProfileClass = TSIPProfileTypeInfo::EInternet;
	 	managedProfile->SetEnabled(ETrue); // Enable SNAP
		 
	    TBool hasChanged;
	    hasChanged = iProfileUtil->ProfileModificationsL(managedProfile, settingsData);
		EUNIT_ASSERT_EQUALS( hasChanged, ETrue ); 
 	}
 

	// Test case 5: All values has changed
	{
		_LIT (KAnotherProvider, "AnotherProvider");
	    settingsData.iProviderName = KAnotherProvider;

		_LIT (KAnotheruserAor, "au@1.2.3.4");
	    settingsData.iUsername = KAnotheruserAor;

		settingsData.iSNAPIndex = KDefaultSNAP + 1;
		settingsData.iAPIndex = KAccessPointIdValue + 1;
		settingsData.iCompression = !KSigCompValue;
		settingsData.iRegistrationMode = EGSAlwaysOn;
		settingsData.iSecurity = !KSecurityNegotiationValue;

		_LIT8 (KAnotherProfile, "AnotherProfile");	
		settingsData.iServiceProfile.iSIPProfileName = KAnotherProfile;
		settingsData.iServiceProfile.iSIPProfileClass = TSIPProfileTypeInfo::EIms;
	 	managedProfile->SetEnabled(EFalse); // Disable SNAP
		
	    TBool hasChanged;
	    hasChanged = iProfileUtil->ProfileModificationsL(managedProfile, settingsData);
		EUNIT_ASSERT_EQUALS( hasChanged, ETrue ); 
	}

	// Test case 6: Test also EGSWhenNeeded -registration mode
	// to increase the test coverage. 
	{	
		HBufC16* providerName = HBufC16::NewLC(25);
		TPtr namePtr (providerName->Des());
		namePtr.Copy( KProviderNameValue );
	    settingsData.iProviderName = *providerName;
		CleanupStack::PopAndDestroy (providerName);

		HBufC16* userAor = HBufC16::NewLC(25);
		TPtr aorPtr (userAor->Des());
		aorPtr.Copy( KUserAorValue );
	    settingsData.iUsername = *userAor;
		CleanupStack::PopAndDestroy (userAor);

		settingsData.iSNAPIndex = KDefaultSNAP;
		settingsData.iAPIndex = KAccessPointIdValue;
		settingsData.iCompression = KSigCompValue;
		settingsData.iRegistrationMode = EGSWhenNeeded;
		settingsData.iSecurity = KSecurityNegotiationValue;
		settingsData.iServiceProfile.iSIPProfileName = KProviderNameValue;
		settingsData.iServiceProfile.iSIPProfileClass = TSIPProfileTypeInfo::EInternet;
		managedProfile->SetEnabled(EFalse); // Disable SNAP

	    TBool hasChanged;
	    hasChanged = iProfileUtil->ProfileModificationsL(managedProfile, settingsData);
		EUNIT_ASSERT_EQUALS( hasChanged, ETrue ); 
	}
	
	CleanupStack::PopAndDestroy(managedProfile);
	CleanupStack::PopAndDestroy(profileRegistry);
	CleanupStack::PopAndDestroy(registryObserver);
    }
    
// -----------------------------------------------------------------------------
// UT_GSSIPProfileUtil_CopyServerAttributesLL()
// -----------------------------------------------------------------------------
//

void UT_GSSIPProfileUtil::UT_GSSIPProfileUtil_CopyServerAttributesLL()
    {
    CSIPProfileRegistryObserver* registryObserver = 
                CSIPProfileRegistryObserver::NewLC();
    
    CSIPManagedProfileRegistry* profileRegistry = 
    		CSIPManagedProfileRegistry::NewLC(*registryObserver);

    CSIPManagedProfile* managedProfile = CSIPManagedProfile::NewLC(profileRegistry);

    TGSSIPServerData serverData;


	// Password has not been changed
	_LIT8(KPassword,"MyPassword");
	HBufC16* pwCopy = HBufC16::NewLC(25);
	TPtr pwPtr (pwCopy->Des());
	pwPtr.Copy( KPassword );

	serverData.iPassword = *pwCopy;
	CleanupStack::PopAndDestroy ( pwCopy );
	
	serverData.iPasswordChanged = EFalse;
   	iProfileUtil->CopyServerAttributesL( managedProfile, KSIPOutboundProxy, serverData );

    const TDesC8* fetchedPassword;
  	managedProfile->GetParameter ( KSIPOutboundProxy, KSIPDigestPassword, fetchedPassword );
	EUNIT_ASSERT_NOT_EQUALS ( *fetchedPassword, KPassword ); 

	// Password has been changed
	serverData.iPasswordChanged = ETrue;
   	iProfileUtil->CopyServerAttributesL( managedProfile, KSIPOutboundProxy, serverData );
    
	managedProfile->GetParameter ( KSIPOutboundProxy, KSIPDigestPassword, fetchedPassword );
	EUNIT_ASSERT_EQUALS ( *fetchedPassword, KPassword ); 
	
		
	CleanupStack::PopAndDestroy(managedProfile);
	CleanupStack::PopAndDestroy(profileRegistry);
	CleanupStack::PopAndDestroy(registryObserver);
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_GSSIPProfileUtil,
    "UT_GSSIPProfileUtil",
    "UNIT" )

EUNIT_TEST(
    "ProfileModificationsL - test ",
    "GSSIPProfileUtil",
    "ProfileModificationsL",
    "FUNCTIONALITY",
    SetupL, UT_GSSIPProfileUtil_ProfileModificationsLL, Teardown)
    
EUNIT_TEST(
    "CopyServerAttributesL - test ",
    "GSSIPProfileUtil",
    "CopyServerAttributesL",
    "FUNCTIONALITY",
    SetupL, UT_GSSIPProfileUtil_CopyServerAttributesLL, Teardown)
    
        
EUNIT_END_TEST_TABLE

//  END OF FILE


