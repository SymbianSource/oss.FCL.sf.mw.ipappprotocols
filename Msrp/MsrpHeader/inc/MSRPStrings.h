/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef MSRPSTRINGS_H
#define MSRPSTRINGS_H

// EXTERNAL INCLUDES
#include <e32def.h>
#include <stringpool.h>

// FORWARD DECLARATIONS
class CMSRPStrings;

// CLASS DECLARATION
class MSRPStrings
	{
	public:	// Constructors and destructor

		/**
		* Opens MSRP string pool, and also provides a 3rd party string pool
		* to open MSRP string pool (when MSRP strings are needed elsewhere)
		* @param aStringpool another string pool where MSRP string pool is added
		* Implemented with a reference count.
		*/
		IMPORT_C static void OpenL( RStringPool& aStringPool );

		/**
		* Opens MSRP string pool.
		* Implemented with a reference count.
		*/
		IMPORT_C static void OpenL();

		/**
		* Closes MSRP string pool.
		* In other words decrements reference count and if it reaches
		* zero, closes the string pool.
		* The user must not call Close() if it has not called OpenL().
		*/
		IMPORT_C static void Close();


	public: // New functions

		/**
		* Gets a case-insensitive string specified
		* in the original string table.
		*
		* @param aIndex The string table enumeration value
		* @return Initialized RStringF object
		*/
		IMPORT_C static RStringF StringF( TInt aIndex );

		/**
		* Gets the string pool used by MSRP Client
		*
		* @return RStringPool: A handle to a string pool
		*/
		IMPORT_C static RStringPool Pool();

		/**
		* Gets the string table used by MSRP client
		*
		* @return TStringTable&: The string pool table
		*/
		IMPORT_C static const TStringTable& Table();

	private:

        /**
        * Creates new CMSRPStrings object
        */
		static CMSRPStrings* Strings();
		};

#endif // MSRPSTRINGS_H

// End of File
