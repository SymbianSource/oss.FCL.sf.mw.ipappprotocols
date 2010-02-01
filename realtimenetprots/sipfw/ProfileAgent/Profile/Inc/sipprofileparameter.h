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
* Name        : sipprofileparameter.h
* Part of     : SIP Profile
* Interface   : private
* The class provides a container for the profile parameters
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILEPARAMETER_H
#define CSIPPROFILEPARAMETER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <s32strm.h> 

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* Class stores parameter key and value pair used by CSIPConcreteProfile
* It provides serialization and deserialization functions
*/
NONSHARABLE_CLASS(CSIPProfileParameter) : public CBase
	{
	public: // Constructors and destructor

		/**
		 * Constructs an object
		 * @param aKey parameter key
		 * @param aValue parameter value
		 * @returns new instance
		 */
		static CSIPProfileParameter* NewL(
		    const TDesC8& aKey,
		    const TDesC8& aValue);

		/**
		 * Constructs an object
		 * @param aKey parameter key
		 * @param aValue parameter value
		 * @returns new instance
		 */
		static CSIPProfileParameter* NewLC(
		    const TDesC8& aKey,
		    const TDesC8& aValue);

		/**
		 * Constructs an object
		 * @param aKey parameter key
		 * @returns new instance
		 */
		static CSIPProfileParameter* NewL(const TDesC8& aKey);

		/**
		 * Constructs an object
		 * @param aKey parameter key
		 * @returns new instance
		 */
		static CSIPProfileParameter* NewLC(const TDesC8& aKey);

		/**
		 * Destructor
		 */
		~CSIPProfileParameter();

    public: // New functions

		/**
		 * Get parameter key
		 * @return parameter key
		 */
		const TDesC8& Key() const;

		/**
		 * Get parameter value, Leave if not found
		 * @return parameter value
		 */
		const TDesC8& ValueL() const;

		/**
		 * Get parameter value, return 0 length descriptor is not found
		 * @return parameter value
		 */
		const TDesC8& Value() const;

		/** 
		 * Deserialize profile parameter from stream, ownership is transferred
		 * @param aReadStream stream holding serialized profile parameter
		 * @return new instance
		 */
		static CSIPProfileParameter* InternalizeL(RReadStream& aReadStream);

		/** 
		 * Serialize profile parameter to stream
		 * @param aWriteStream stream where the parameter is to be serialized
		 */
		void ExternalizeL(RWriteStream& aWriteStream) const;

		/** 
		 * Get serialized profile parameter size
		 * @return size needed to serialize profile parameter
		 */
		TUint ExternalizedSizeL() const;

		/** 
		 * Compare two profile parameters
		 * @param aParameter profile parameter to compare to
		 * @return ETrue if profile parameters are equal
		 */
		TBool operator==(const CSIPProfileParameter& aParameter) const;

	private: // Constructors

        /**
         * Default constructor.
         */
		CSIPProfileParameter();

        /**
         * Symbian 2nd phase constructor.
		 * @param aKey parameter key
		 * @param aValue parameter value
         */
		void ConstructL(const TDesC8& aKey, const TDesC8& aValue);

        /**
         * Symbian 2nd phase constructor.
		 * @param aKey parameter key
         */
		void ConstructL(const TDesC8& aKey);

    private: // New functions

		/** 
		 * Deserialize profile parameter from stream
		 * @param aReadStream stream holding serialized profile parameter
		 */
		void DoInternalizeL(RReadStream& aReadStream);

	private: // Data

		HBufC8*	iParameterKey;
		HBufC8*	iParameterValue;
		HBufC8*	iEmpty;
	};

#endif // CSIPPROFILEPARAMETER_H
