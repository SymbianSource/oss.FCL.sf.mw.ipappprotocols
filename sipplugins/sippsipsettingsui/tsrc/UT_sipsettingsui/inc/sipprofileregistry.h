/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class for using SIP profiles
*
*/


#ifndef CSIPPROFILEREGISTRY_H
#define CSIPPROFILEREGISTRY_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <badesca.h>
#include <sipprofileregistryobserver.h>
#include "sipprofileregistrybase.h"


// FORWARD DECLARATIONS
class CSIP;
class CSIPConnection;
class CSIPProfile;
class MSIPProfileRegistryObserver;
class MSIPConnectionObserver;

// STUB stuff

typedef TInt TSIPProfileBehavior;
const TSIPProfileBehavior KRegistryLeaveAtConstruct = 1;
const TSIPProfileBehavior KSIPProfileIdFetchFails = 2;
const TSIPProfileBehavior KSIPIapIdFetchFails = 3;
const TSIPProfileBehavior KSIPConnectionLeaveAtConstruct = 4;

class CSIPProfileBehaviorTls : public CBase
    {
public:
    static void OpenL();
    static void Close();
	
    static CSIPProfileBehaviorTls* Storage();
    
    void Reset();

private:

    CSIPProfileBehaviorTls();	
    ~CSIPProfileBehaviorTls();

public:

    TSIPProfileBehavior iRegistryBehavior;
    TSIPProfileBehavior iProfileBehavior;    
    TSIPProfileBehavior iConnectionBehavior;
    };
	


// CLASS DECLARATION
/**
*  @publishedAll
*  @released
*
*  The class for retrieving SIP profiles from permanent storage.
*  This class provides services for retreiving SIP profiles
*  and enabling/disabling them for the usage.
*
*  The user can create only one instance of this class (a singleton class).
*
*  @lib sipprofilecli.lib
*/
class CSIPProfileRegistry: public CSIPProfileRegistryBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * This constructor should be used if the client intends
        * to use SIP services with profiles.
        * @param aSip a SIP client providing connection to SIP stack.
        * @param aObserver a observer for SIP profile change events.
        */
        IMPORT_C static CSIPProfileRegistry* NewL(
                                        CSIP& aSip,
                                        MSIPProfileRegistryObserver& aObserver);

        /**
        * Two-phased constructor.
        * This constructor should be used if the client intends
        * to use SIP services with profiles.
        * Constructs an object and adds the pointer to the cleanup stack;
        * @param aSip a SIP client providing connection to SIP stack.
        * @param aObserver a observer for SIP profile change events.
        */
        IMPORT_C static CSIPProfileRegistry* NewLC(
                                        CSIP& aSip,
                                        MSIPProfileRegistryObserver& aObserver);

        /**
        * Destructor
        */
        IMPORT_C ~CSIPProfileRegistry();

    public: // New functions
        /**
        * Gets handle to the SIP server
        * @return handle to the SIP server
        */
        IMPORT_C CSIP& SIP() const;

        /**
        * Gets the SIP connection to be used with this SIP profile.
        * @pre IsEnabled() == ETrue
        * @param aProfile a sip profile
        * @return a SIP connection to be used; the owneship is transfered
        * @leave KErrNoMemory if out of memory
        * @capability NetworkServices
        */
        IMPORT_C CSIPConnection* ConnectionL(
                        CSIPProfile& aProfile);

        /**
        * Enables the SIP profile for use.
        * Enabling the SIP profile will cause the SIP profile to
        * be registered if its status was unregistered.
        * The user must check the profile status after calling this
        * function. In case the profile is not registered the user must
        * wait until the it is notified about profile registration 
        * on MSIPProfileRegistryObserver-callback interface.
        * @param aProfile a sip profile to enable
        * @param aObserver a observer for SIP connection events
        * @leave KErrNotFound if non-existing profile is provided
        * @capability NetworkServices
        */
        IMPORT_C void EnableL(
                        CSIPProfile& aProfile,
                        MSIPConnectionObserver &aObserver);
        
        /**
        * Disables the usage of SIP profile
        * @param aProfile a sip profile to disable
        * @return KErrNone if SIP profile was successfully disabled; 
        *         system wide error otherwise
        * @capability NetworkServices
        */
        IMPORT_C TInt Disable(CSIPProfile& aProfile);

        /**
        * Tests is the SIP profile enabled for the use
        * @param aProfile a SIP profile to be checked
        * @return ETrue if SIP profile is enabled; EFalse otherwise
        */
        IMPORT_C TBool IsEnabled(const CSIPProfile& aProfile) const;

    public: 

        CSIPProfile* NewInstanceL();
    
    private:
        
        CSIPProfileRegistry();
        CSIPProfileRegistry(CSIP& aSip, MSIPProfileRegistryObserver& aObserver);

        void ConstructL();
        
    public:
    
       TBool iProfileDisabled;
       mutable TBool iProfileEnabled;
        
    private: 

        CSIP& iSip;
        MSIPConnectionObserver* iConnectionObserver;
        
    };



// Constants
_LIT8 ( KProviderNameValue, "MyProvider");
_LIT8 ( KUserAorValue, "i@127.0.0.1" );
const TUint KSnapIdValue (10);
const TUint KAccessPointIdValue (2);
const TBool  KSigCompValue (EFalse);
const TBool  KAutoRegistrationValue (ETrue);
const TBool  KAllowAutoRegistrationOnlyOnHomeNetworkValue (ETrue);
const TBool  KSecurityNegotiationValue (EFalse);


class CSIPProfileRegistryObserver : public CBase, public MSIPProfileRegistryObserver
{
public:

    static CSIPProfileRegistryObserver* NewLC();
    static CSIPProfileRegistryObserver* NewL();
    virtual ~CSIPProfileRegistryObserver();

	void ProfileRegistryEventOccurred (TUint32 aProfileId, TEvent aEvent);
	void ProfileRegistryErrorOccurred (TUint32 aProfileId, TInt aError);
	
private:
    CSIPProfileRegistryObserver(); 
    void ConstructL();
};

#endif // CSIPPROFILEREGISTRY_H
