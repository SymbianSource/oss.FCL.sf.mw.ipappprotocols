
/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CSIPEXPROFILEQUERYDLG_H__
#define __CSIPEXPROFILEQUERYDLG_H__

// INCLUDES
#include    <eikdialg.h>
#include "TSIPProfileData.h"
#include <sipmanagedprofileregistry.h>
#include <sipprofileregistryobserver.h>
#include "sipmanagedprofile.h"


// FORWARD DECLARATIONS
class CSIPAddress;
class MSIPProfileRegistryObserver;
class CSIPManagedProfileRegistry;

// CLASS DECLARATION
/**
* A recipient's address query dialog for game invitation.
*/
class CSIPExProfileQueryDlg 
: public CEikDialog,
  public MSIPProfileRegistryObserver
    {
    public:

        /**
        * Static constructor
        * @param aAddr On return contains the other player's address.
        */
        static CSIPExProfileQueryDlg* NewL();

        /**
        * Static constructor. On return the instance is left to the
        * CleanupStack.
        * @param aAddr On return contains the other player's address.
        */
        static CSIPExProfileQueryDlg* NewLC( );

        /**
        * Destructor
        */
        ~CSIPExProfileQueryDlg();
        
    public: //Functions from base classes

        /** 
        * from MSIPProfileRegistryObserver 
        * SIP profile creation event
        * @since 1.0
		* @param aSIPProfileId id of new SIP Profile
		*/
		void ProfileRegistryEventOccurred(TUint32 aProfileId, TEvent aEvent);


		/**
        * from MSIPProfileRegistryObserver
		* An asynchronous error has occurred related to SIP profile		
        * @since 1.0
		* @param aSIPProfileId the id of failed profile 
		* @param aError a error code
		*/
		void ProfileRegistryErrorOccurred( TUint32 aSIPProfileId,
		    TInt aError);

        /** 
        * SIP profile creation event
		* @param aSIPProfileId id of new SIP Profile
		*/
		void ProfileCreated( TUint32 aSIPProfileId );


    private:

        /**
        * C++ default constructor
        * @param aAddr On return contains the other player's address.
        */
        CSIPExProfileQueryDlg( );
        
        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL();
    
        /**
        * from MProfileHandler
        * Saves profile's changes or returns EFalse if no changes are present.        
        */
        void SaveProfileL();

        /**
        * Copies data from iProfileData to the actual CSIPManagedProfile
        * @return Profile where the stuff was copied to.
        */
        CSIPManagedProfile* CopyDataToProfileL();

    private: // From base classes

        /**
        * From CEikDialog
        */
        TBool OkToExitL(TInt aKeyCode);

        /**
        * From CEikDialog
        */
        virtual void PreLayoutDynInitL();
        
    private: // New functions
    
        /**
        * Fills the data structure with default values
        */
        void FillDataStructWithDefaultValsL();
        
    	TBool SetProfileUserNameL();
    	TBool SetProfileCompressionL();
    	TBool SetProfileIAPIdL();
    	TBool SetProfileSecurityL();
    	TBool SetProfileProxyAddressL();
    	TBool SetProfileProxyRealmL();
    	TBool SetProfileProxyUNameL();
    	TBool SetProfileProxyPwL();
    	TBool SetProfileRegistrarAddressL();
    	TBool SetProfileRegistrarRealmL();
    	TBool SetProfileRegistrarUNameL();
    	TBool SetProfileRegistrarPwL();
    	
    	void ShowErrorInfoL(TProfileSettingItemListItems aItem);
    	
        /**
        * Checks if user typed address is valid address.
        * @param aAddr address which validity is checked
        * Returns true if valid address, otherwise returns false
        */
        TBool AddressValid( const TDesC8& aAddr );
        
        void GetAPListL();

    private:
        // Member variables

        // The profile's data, which is manipulated
        TSIPProfileData iProfileData;
        CSIPManagedProfileRegistry* iMProfileRegistry;

        // Temporary pointer storage for a new profile before
        // placing it to the permanent storage
        CSIPManagedProfile* iNewProfile;
        
        CDesCArraySeg* iAccesspointids;
        
   		// Array stores the mapping between 
		// the APs index and the UID of the APs
		RArray<TUint32> iAPsUIDs;


    };

#endif // __CSIPEXPROFILEQUERYDLG_H__

// End of file
