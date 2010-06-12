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


#ifndef CMSRPHEADERBASE_H
#define CMSRPHEADERBASE_H

// EXTERNAL INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include <stringpool.h>

// CONSTANTS
_LIT8( KColonAndSpace, ": " );

// CLASS DECLARATION
/**
* @publishedAll
*
* Class provides a generic interface for all the MSRP headers.
*
*  @lib msrpclient.lib
*/
class CMSRPHeaderBase : public CBase
	{
	public: // Constructors and destructors

		/**
		* Destructor, deletes the resources of CMSRPHeaderBase.
		*/
		IMPORT_C virtual ~CMSRPHeaderBase();

	public: // New functions


		/**
		* Encodes the header (name and value) into its textual representation.
		* @return a textual representation of the complete header,
		*         the ownership is transferred
		*/
		IMPORT_C HBufC8* ToTextL() const;

		/**
		* Encodes the header (name and value) into its textual representation
		* and pushes it to the CleanupStack.
		* @return a textual representation of the complete header,
		*         the ownership is transferred
		*/
		IMPORT_C HBufC8* ToTextLC() const;

		/**
		* Encodes the header's value into its textual representation
		* and pushes it to the CleanupStack.
		* @return a textual representation of the header's value,
		*         the ownership is transferred
		*/
		IMPORT_C HBufC8* ToTextValueLC() const;
		
	protected: // pure virtuals

		/**
		* Gets the full name of the header
		* The function is implemented in each of the sub-classes.
		* @return the full name of the header for example "From-Path"
		*/
		virtual RStringF Name() const = 0;

		/**
		* Encodes the header's value into its textual representation.
		* @return a textual representation of the header's value,
		*         the ownership is transferred
		*/
		virtual HBufC8* ToTextValueL() const = 0;

	protected: // Constructors

		CMSRPHeaderBase();
	};

#endif // CMSRPHEADERBASE_H

// End of File
