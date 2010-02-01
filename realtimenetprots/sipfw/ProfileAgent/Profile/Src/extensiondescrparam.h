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
* Name        : extensiondescrparam.h
* Part of     : SIP Profile
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CEXTENSIONDESCRPARAM_H
#define CEXTENSIONDESCRPARAM_H

// INCLUDES
#include <e32base.h>
#include <s32strm.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* This class represents an extension parameter whose value is a descriptor.
*/
NONSHARABLE_CLASS(CExtensionDescrParam) : public CBase
	{
	public: // Constructors and destructor

		/**
		* Constructor
		*/
		static CExtensionDescrParam* NewLC(TUint32 aID, const TDesC8& aValue);

		~CExtensionDescrParam();

    public: // New functions

		/**
		* De-serialize parameter from stream
		* @param aReadStream stream where data is read from
		*/
		static CExtensionDescrParam* InternalizeL(RReadStream& aReadStream);

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
		TBool operator==(const CExtensionDescrParam& aParam) const;

		TUint32 ID() const;
		
		const TDesC8& Value() const;

	private: // Constructors
	
		CExtensionDescrParam(TUint32 aID);		
		void ConstructL(const TDesC8& aValue);

    private: // Data

		TUint32 iID;
		HBufC8* iValue; // owned
	
    private: // For testing purposes
    		
        #ifdef CPPUNIT_TEST
		    friend class CExtensionDescrParamTest;    
        #endif
	};

#endif // CEXTENSIONDESCRPARAM_H
