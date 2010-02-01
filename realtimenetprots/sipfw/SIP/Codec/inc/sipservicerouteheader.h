/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipservicerouteheader.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSERVICEROUTEHEADER_H
#define CSIPSERVICEROUTEHEADER_H

//  INCLUDES
#include "siprouteheaderbase.h"
#include "_sipcodecdefs.h"

// CLASS DECLARATION
/**
* Class for a SIP Service-Route header.
*
* @lib sipcodec.lib
*/
class CSIPServiceRouteHeader : public CSIPRouteHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CSIPServiceRouteHeader from textual representation 
		* of the header's value part.
		* @param aValue a value part of a "Service-Route"-header
		* @return an array containing 1..n instances of CSIPServiceRouteHeader
		*/
		IMPORT_C static RPointerArray<CSIPServiceRouteHeader> 
			DecodeL(const TDesC8& aValue);

		/**
		* Creates a new instance of CSIPServiceRouteHeader
		* @pre aSIPAddress != 0
		* @param aSIPAddress a name-address, the ownership is transferred 
		* @returns a new instance of CSIPServiceRouteHeader
		*/
		IMPORT_C static CSIPServiceRouteHeader* NewL(CSIPAddress* aSIPAddress);

		/**
		* Creates a new instance of CSIPServiceRouteHeader 
		* and puts it to CleanupStack
		* @pre aSIPAddress != 0
		* @param aSIPAddress a name-address, the ownership is transferred 
		* @returns a new instance of CSIPServiceRouteHeader
		*/
		IMPORT_C static CSIPServiceRouteHeader* NewLC(CSIPAddress* aSIPAddress);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPServiceRouteHeader();


	public: // From CSIPHeaderBase

		/**
		* From CSIPHeaderBase CloneL
		*/
		IMPORT_C CSIPHeaderBase* CloneL() const;
		
		/**
		* From CSIPHeaderBase Name
		*/		
		IMPORT_C RStringF Name() const;

		/**
		* From CSIPHeaderBase ExternalizeSupported
		*/
		IMPORT_C TBool ExternalizeSupported() const;


	public: // New functions, for internal use

		static RPointerArray<CSIPHeaderBase> BaseDecodeL(const TDesC8& aValue);

	private: // Constructors 

		CSIPServiceRouteHeader();
	};

#endif // CSIPSERVICEROUTEHEADER_H

// End of File
