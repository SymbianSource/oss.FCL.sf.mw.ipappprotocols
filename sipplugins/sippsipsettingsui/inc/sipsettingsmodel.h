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
* Description:  The model of SIP Settings view
*
*/


#ifndef SIP_SETTINGS_MODEL_H
#define SIP_SETTINGS_MODEL_H

//  INCLUDES

#include <e32base.h>
#include <bamdesca.h>       // MDesCArray
#include <AknUtils.h>       // KMaxColumnDataLength
#include "tsipsettingsdata.h"

// FORWARD DECLARATIONS

class CGSSIPModel;
class MGSSIPProfileHandler;
class CGSSIPModel;

// CLASS DECLARATION

/**
*  CSIPSettingsModel model class
*  @since 3.1
*  model class for SIP Settings view
*/
class CSIPSettingsModel 
    : public CBase,
      public MDesCArray
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * (@param aHandler Pointer to profile manipulator)
        */
        static CSIPSettingsModel* NewL( MGSSIPProfileHandler* aHandler );
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettingsModel();

    public: // New functions

        /**
        * Returns the index of the default profile
        * @return The index of the default profile
        */
        TInt DefaultProfileIndex();

        /**
        * Sets new default profile
        * @param aIndex The index of the (new) default profile
        */
        void SetDefaultProfileL( TInt aIndex );

        /**
        * Returns the array containing the profile names.
        * NOTE! The ownership of the array is on the caller
        * @return Array consisting of the names of the profiles
        */
        CDesCArray* ListOfProfileNamesL();
        
        /**
        * Returns the name of a profile
        * NOTE! The returning buffers ownership changes to the caller
        * @param aIndex Index to the queried profile        
        * @return Buffer containing the name of the queried profile
        */
        HBufC* ProfileNameLC( TInt aIndex );

        /**
        * Returns the number of profiles
        * @return Number of profiles
        */
        TInt NumOfProfiles();

        /**
        * Creates a new profile and prepares it for editing
        * @param aIndex Index to the profile that is used as a basis
        */
        void CreateNewProfileL( TInt aIndex );

        /**
        * Marks a profile for editing it
        * @param aIndex Index to the edited profile
        * @return Ok to move to next view?
        */
        TBool EditProfileL( TInt aIndex );

        /**
        * Deletes a profile
        * @param aIndex Index to the deleted profile
        */
        void DeleteProfileL( TInt aIndex );

        /**
        * Checks if profile can be deleted.
        * @param aIndex Index to the deleted profile
        * @param aIsUse ETrue if profile is in use
        * @param aDefault ETrue if profile is default profile 
        * @param aLocked ETrue if profile is locked 
        */
        void CheckProfileForDeleteL( 
            TInt aIndex, TBool& aIsUse, TBool& aDefault, TBool& aLocked );
        
    protected:  // Functions from base classes
        
        /**
        * From MDesC16Array returns the number of the elements
        */	
        TInt MdcaCount() const;
	
        /**
        * From MDesC16Array Returns a list item
        */
        TPtrC16 MdcaPoint(TInt aIndex) const;
        
    private:

        /**
        * C++ default constructor.
        * @param aHandler Pointer to profile manipulator
        */
        CSIPSettingsModel( MGSSIPProfileHandler* aHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
    
        // The buffer that is returned on MdcaPoint() call
        //mutable TBuf<KMaxColumnDataLength> iBuf;   
        mutable TBuf<KMaxSIPProviderNameLength*3> iBuf;
        
        // Pointer to profile handler
        MGSSIPProfileHandler* iHandler;

        // "Registered"/"Not registered"/"Error in registration" texts
        HBufC* iRegisteredTxt;
        HBufC* iNotRegisteredTxt;
        HBufC* iErrorInReg;                
    };

#endif      // SIP_SETTINGS_MODEL_H   
            
// End of File
