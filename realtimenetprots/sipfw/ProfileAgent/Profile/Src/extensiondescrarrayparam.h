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
* Name        : extensiondescrarrayparam.h
* Part of     : SIP Profile
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CEXTENSIONDESCRARRAYPARAM_H
#define CEXTENSIONDESCRARRAYPARAM_H

// INCLUDES
#include <e32base.h>
#include <s32strm.h>

// FORWARD DECLARATIONS
class MDesC8Array;
class CDesC8ArrayFlat;

// CLASS DECLARATION
/**
* This class represents an extension parameter whose value 
* is a descriptor array.
*/
NONSHARABLE_CLASS(CExtensionDescrArrayParam) : public CBase
	{
	public: // Constructors and destructor

		/**
		* Constructor
		*/
		static CExtensionDescrArrayParam*
			NewLC(TUint32 aID, const MDesC8Array& aValue);

		~CExtensionDescrArrayParam();

    public: // New functions

		/**
		* De-serialize parameter from stream
		* @param aReadStream stream where data is read from
		*/
		static CExtensionDescrArrayParam*
			InternalizeL(RReadStream& aReadStream);

		/**
		* Serialize parameter to stream
		* @param aWriteStream stream where parameter is to be serialized		
		*/
		void ExternalizeL(RWriteStream& aWriteStream) const;
		
		TUint ExternalizedSizeL() const;
		
		/** 
		 * Compare two parameters
		 * @param aParameter parameter to compare to
		 * @return ETrue if parameters are equal
		 */
		TBool operator==(const CExtensionDescrArrayParam& aParam) const;

		TUint32 ID() const;
		
		const MDesC8Array& Value() const;

	private: // Constructors
	
		CExtensionDescrArrayParam(TUint32 aID);
		
		void ConstructL(const MDesC8Array& aValue);

    private: // Data

		TUint32 iID;
		CDesC8ArrayFlat* iValue; // owned

    private: // For testing purposes

        #ifdef CPPUNIT_TEST
		    friend class CExtensionDescrArrayParamTest;    
        #endif
	};

#endif // CEXTENSIONDESCRARRAYPARAM_H
