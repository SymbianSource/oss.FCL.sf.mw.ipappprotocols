/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofileitc.h
* Part of     : SIP Profile Client
* Interface   : private
* The class for simplifying ITC access
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILEITC_H
#define CSIPPROFILEITC_H

//  INCLUDES

#include <e32base.h>
#include <s32mem.h>
#include <bamdesca.h>
#include "sipprofilecs.h"
#include "SIPProfileITCHelper.h"
#include "sipprofiletypeinfo.h"

// FORWARD DECLARATIONS

class RSIPProfile;
class CSIPConcreteProfile;
class CSIPConcreteProfileHolder;
class CSIPProfilePlugins;

// CLASS DECLARATION

/**
*  The class provides simplified ITC access for client
*
*  @lib sipprofilecli.lib
*/
class CSIPProfileITC : public CBase
	{
	public:

	    /**
        * Two-phased constructor.
		* @param aSipProfile a client providing connection to SIP profile server.
		* @return new instance
        */
		static CSIPProfileITC* NewL(RSIPProfile& aSipProfile);

	    /**
        * Two-phased constructor.
		* @param aSipProfile a client providing connection to SIP profile server.
		* @return new instance
        */
		static CSIPProfileITC* NewLC(RSIPProfile& aSipProfile);

	    /**
        * Destructor.
        */
		~CSIPProfileITC();

	    /**
        * Retrieve profile plugins from profile server.
		* @return profile plugins containing list of plugin types
        */
		CSIPProfilePlugins* SIPProfilePluginsL();

	    /**
        * Add profile into persistent storage of profile server.
		* @pre aProfile.Id() = 0
		* @post aProfile.Id() > 0
		* @param aProfile profile to be added
        */
		void AddSIPProfileL(CSIPConcreteProfile& aProfile);

	    /**
        * Update profile into persistent storage of profile server.
		* @pre aProfile.Id() > 0
		* @param aProfile profile to be added
        */
		void UpdateSIPProfileL(CSIPConcreteProfile& aProfile);

	    /**
        * Remove profile from persistent storage of profile server.
		* @param aProfileId id of the profile to be removed
        */
		void RemoveSIPProfileL(TUint32 aProfileId);

	    /**
        * Retrieve id and size of default profile from profile server
		* @param aSize returned size of default profile
		* @param aProfileId returned id of default profile
        */
		void SIPProfileDefaultL(TUint& aSize, TUint32& aProfileId);

	    /**
        * Retrieve single SIP profile from profile server
		* @param aProfileId id of retrieved profile
		* @param aSize returned size of retrieved profile
        */
		CSIPConcreteProfile* SIPProfileL(TUint32 aProfileId, TUint aSize = 0);

	    /**
        * Refresh already previously retrieved SIP profile from profile server
		* @param aProfileId id of retrieved profile
		* @param aSize returned size of retrieved profile
        */
		CSIPConcreteProfile* SIPProfileRefreshL(TUint32 aProfileId, TUint aSize = 0);

	    /**
        * Retrieve id of default profile from profile server
		* @return id of default profile
        */
		TUint SIPProfileDefaultIdL();
	
	    /**
        * Retrieve all SIP profiles from profile server
		* @return holder containing array of profiles
        */
		CSIPConcreteProfileHolder* SIPProfilesL();

	    /**
        * Retrieve multiple SIP profiles based on AOR from profile server
		* @param aAOR AOR of retrived profiles
		* @return holder containing array of profiles
        */
		CSIPConcreteProfileHolder* SIPProfilesByAORL(const TDesC8& aAOR);

	    /**
        * Retrieve multiple SIP profiles based on type from profile server
		* @param aType type of retrived profiles
		* @return holder containing array of profiles
        */
		CSIPConcreteProfileHolder* SIPProfilesByTypeL(const TSIPProfileTypeInfo& aType);

	    /**
        * Inform profile server that local instance of profile is deleted
		* @param aProfileId id of retrieved profile
        */
		void DeleteSIPProfileL(TUint32 aProfileId);

	    /**
        * Enable the profile for use
		* @param aProfileId id of retrieved profile
		* @return Status of the profile
        */
		TInt EnableSIPProfileL(TUint32 aProfileId);
		
	    /**
        * Disable the profile after being used
		* @param aProfileId id of retrieved profile
		* @return Status of the profile
        */
		TInt DisableSIPProfileL(TUint32 aProfileId);

		/**
		* Force disable the profile after being used
		* @param aProfileId id of retrieved profile
		* @return Status of the profile
		*/
		TInt ForceDisableSIPProfileL(TUint32 aProfileId);
		
	    /**
        * Retrieve number of users of a profile
		* @param aProfileId id of retrieved profile
		* @return number of clients that have the profile enabled
        */
		TInt SIPProfileUsageL(TUint32 aProfileId);

	    /**
        * Retrieve new empty profile of given type
		* with default values. Profile is not stored into
		* server.
		* @param aType type of retrived profiles
		* @return new instance of concrete profile
        */
		CSIPConcreteProfile* SIPCreateProfileL(
			const TSIPProfileTypeInfo& aType);

		/**
		* Updates SIP profile value for negotiated security mechanism; leaves on failure
		* @param aProfile a SIP profile
		*/
		void NegotiatedSecurityMechanismL(CSIPConcreteProfile& aProfile);

		/**
		* Updates SIP profile value for registered AORs; leaves on failure
		* @param aProfile a SIP profile
		*/
		void RegisteredAORsL(CSIPConcreteProfile& aProfile);
		
		
		/**
		* Allows SIP Profile Agent to start the migration 
		* from the old IAP to the new one.
		* @param aProfileId the SIP profile id
		* @param aIapId the new IAP id. 
		*/
		void AllowMigrationL(TUint32 aProfileId, TUint32 aIapId);

		/**
		* Disallows SIP Profile Agent to start the migration 
		* from the old IAP to the new one.
		* @param aProfileId the SIP profile id
		* @param aIapId the new IAP id. 
		*/
		void DisallowMigrationL(TUint32 aProfileId, TUint32 aIapId);
		
		/**
        * Informs the server side that ALR controller
        * has been created. 
        */
		void AddALRObserverL();

		/**
        * Informs the server side that ALR controller
        * has been destroyed. 
        */
		void RemoveALRObserver();
		
		/**
        * Refreshes the list of available IAPs for a profile.
        */
		void RefreshIapAvailabilityL( TUint32 aProfileId );



	private:

	    /**
        * Constructor
		* @param aSip a client providing connection to SIP profile server.
        */
		CSIPProfileITC(RSIPProfile& aSip);

	    /**
        * Symbian 2nd phase constructor
        */
		void ConstructL();

	private:

		CSIPProfileITCHelper iHelper;
		TIpcArgs iITCMsgArgs;
		HBufC8* iEmptyContent;

	};

#endif
