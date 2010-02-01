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
* Name        : extensionintparam.h
* Part of     : SIP Profile
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TEXTENSIONINTPARAM_H
#define TEXTENSIONINTPARAM_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* This class represents an extension parameter whose value is integer.
*/
NONSHARABLE_CLASS(TExtensionIntParam)
	{
	public: // Constructors

		/**
		* Default constructor
		*/
		TExtensionIntParam();
	
		/**
		* Copy constructor
		*/
		TExtensionIntParam(const TExtensionIntParam& aParam);

		/**
		* Constructor
		*/
		TExtensionIntParam(TUint32 aID, TUint32 aValue);

    public: // New functions

		/**
		* De-serialize parameter from stream
		* @param aReadStream stream where data is read from
		*/
		static TExtensionIntParam InternalizeL(RReadStream& aReadStream);

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
		TBool operator==(const TExtensionIntParam& aParam) const;

		TUint32 ID() const;
		
		TUint32 Value() const;

	private: // Data

		TUint32 iID;
		TUint32 iValue;

    private: // For testing purposes

        #ifdef CPPUNIT_TEST
		    friend class CExtensionIntParamTest;    
        #endif
	};

#endif // TEXTENSIONINTPARAM_H
