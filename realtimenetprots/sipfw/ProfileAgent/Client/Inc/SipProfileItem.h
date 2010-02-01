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
* Name        : sipprofileitem.h
* Part of     : SIP Profile Client
* Interface   : private
* The class providing profile caching for client
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILEITEM_H
#define CSIPPROFILEITEM_H

//  INCLUDES

#include <e32base.h>

#include "sipconcreteprofile.h"

// FORWARD DECLARATIONS

class CSIPProfile;

// CLASS DECLARATION

/**
*  The class provides profile caching and reference counting
*
*  @lib sipprofilecli.lib
*/
class CSIPProfileItem: public CBase
	{
	public : 

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSIPProfileItem* NewL();

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSIPProfileItem* NewLC();

		/**
        * Destructor.
        */
		~CSIPProfileItem();

		/**
        * Set profile id
		* @param aProfileId profile id
        */
		void SetProfileId(TUint aProfileId);

		/**
        * Set proxy profile
		* @param aProfile profile accessed by application
        */
		void SetProfile(CSIPProfile& aProfile);

		/**
        * Set concrete profile
		* @param aProfile profile to be cached
        */
		void SetConcreteProfile(CSIPConcreteProfile* aProfile);

		/**
        * Get proxy profile
		* @return proxy profile accessed by application
        */
		CSIPProfile* Profile() const;

		/**
        * Get concrete profile
		* @return concrete profile holding profile data
        */
		const CSIPConcreteProfile& ConcreteProfile() const;

		/**
        * Get concrete profile
		* @return concrete profile holding profile data
        */
		CSIPConcreteProfile& ConcreteProfile();

		/**
        * Enables and disables profile
		* @param aEnabled, ETrue if enabled
        */
		void SetEnabled(TBool aEnabled);

		/**
        * Check if enabled
		* @return ETrue if enabled
        */
		TBool IsEnabled() const;

		/**
        * Increment concrete profile reference count
		* @param aProfile instance of proxy profile pointing concrete profile
        */
		void IncrementReferenceCountL(CSIPProfile& aProfile);

		/**
        * Decrement concrete profile reference count
		* @param aProfile instance of proxy profile that is deleted
        */
		void DecrementReferenceCount(CSIPProfile& aProfile);

		/**
        * Get count of proxy profiles pontinc concrete profile
		* @return count of proxy profiles
        */
		TUint ReferenceCount() const;

		/**
        * Stores last occurred registration error
		* @param aStatus status indicating if error occurred
		*		  during registering (EURegistrationInProgress)
		*		  or deregistering (EUnregistrationInProgress)
		* @param aError occurred error code
        */
		void ErrorOccurred(CSIPConcreteProfile::TStatus aStatus, TInt aError);

		/**
        * Compare two items
		* @return ETrue, if profile id equals
        */
		static TBool Compare(const CSIPProfileItem& aItem,
				const CSIPProfileItem& aItem2);

	private:

		/**
        * Default constructor
        */
		CSIPProfileItem();

		/**
        * Symbian 2nd phase constructor
        */
		void ConstructL();

	private:
		TUint iProfileId;
		TBool iEnabled;

		CSIPConcreteProfile* iConcreteProfile;
		RPointerArray<CSIPProfile> iProfiles;
	};

#endif
