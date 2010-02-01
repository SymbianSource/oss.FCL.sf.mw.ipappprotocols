/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipconcreteprofileholder.h
* Part of     : SIP / SIP Profile Agent / SIP Concrete Profile
* Version     : %version: 2.1.2 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCONCRETEPROFILEHOLDER_H
#define CSIPCONCRETEPROFILEHOLDER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "sipconcreteprofile.h"

// FORWARD DECLARATIONS
class CSIPConcreteProfileHolderTest;

// CLASS DECLARATION
/**
* Class is a holder for multiple CSIPConcreteProfile instances
* Provides serialization and deserialization functions
*/
class CSIPConcreteProfileHolder: public CBase
    {
    public: // Constructors and destructor

        /**
        * Constructs an object
        * @returns new instance
        */
        IMPORT_C static CSIPConcreteProfileHolder* NewL();

        /**
        * Constructs an object and adds the pointer to the cleanup stack;
        * leaves on failure
        * @return new instance
        */
        IMPORT_C static CSIPConcreteProfileHolder* NewLC();

        /**
        * Destructor. 
        */            
        IMPORT_C ~CSIPConcreteProfileHolder();

    public: // New functions

        /** 
        * Deserialize profile holder from stream, ownership is transferred
        * @param aReadStream stream holding serialized profile holder
        * @return new instance
        */
        IMPORT_C static CSIPConcreteProfileHolder* InternalizeL(
            RReadStream& aReadStream);

        /**
        * Get hold array of profiles 
        * @return array of profiles
        */
        IMPORT_C RPointerArray<CSIPConcreteProfile>& SIPProfiles();

        /** 
        * Serialize profile holder to stream
        * @param aWriteStream stream where profile holder is to be serialized
        */
        IMPORT_C void ExternalizeL(
            RWriteStream& aWriteStream) const;

        /** 
        * Get serialized profile holder size
        * @return size needed to serialize profile holder
        */
        IMPORT_C TUint ExternalizedSizeL() const;

    private: // Constructors

        CSIPConcreteProfileHolder();
        void ConstructL();

	private: // For internal use
		/**
		* Cleans proxy- and registrar server passwords from the profile.
		* Leaves copy of the profile into the cleanup stack.
		* @param aProfile Profile whose password is deleted
		* @return Copy of the profile without passwords
		*/
        static CSIPConcreteProfile* CloneProfileWithoutPasswordsLC(
        	const CSIPConcreteProfile* aProfile);

    private: // New functions
        void DoInternalizeL(RReadStream& aReadStream);
    
    private: // Data

        RPointerArray<CSIPConcreteProfile> iSIPProfiles;
        TBool iInternalized;
	private: //For testing purposes
#ifdef CPPUNIT_TEST	
    friend class CSIPConcreteProfileHolderTest;
#endif
    };

#endif // CSIPCONCRETEPROFILEHOLDER_H
