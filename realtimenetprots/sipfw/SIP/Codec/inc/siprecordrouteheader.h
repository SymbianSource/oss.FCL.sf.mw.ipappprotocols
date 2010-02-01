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
* Name          : siprecordrouteheader.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPRECORDROUTEHEADER_H
#define CSIPRECORDROUTEHEADER_H

//  INCLUDES
#include "siprouteheaderbase.h"
#include "_sipcodecdefs.h"

// CLASS DECLARATION
/**
* Class for a SIP Record-Route header.
*
* @lib sipcodec.lib
*/
class CSIPRecordRouteHeader : public CSIPRouteHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CSIPRecordRouteHeader from textual representation 
		* of the header's value part.
		* @param aValue a value part of a "Record-Route"-header
		* @return an array containing 1..n instances of CSIPRecordRouteHeader
		*/
		IMPORT_C static RPointerArray<CSIPRecordRouteHeader> 
			DecodeL(const TDesC8& aValue);

		/**
		* Creates a new instance of CSIPRecordRouteHeader
		* @pre aSIPAddress != 0
		* @param aSIPAddress a name-address, the ownership is transferred 
		* @returns a new instance of CSIPRecordRouteHeader
		*/
		IMPORT_C static CSIPRecordRouteHeader* NewL(CSIPAddress* aSIPAddress);

		/**
		* Creates a new instance of CSIPRecordRouteHeader 
		* and puts it to CleanupStack
		* @pre aSIPAddress != 0
		* @param aSIPAddress a name-address, the ownership is transferred 
		* @returns a new instance of CSIPRecordRouteHeader
		*/
		IMPORT_C static CSIPRecordRouteHeader* NewLC(CSIPAddress* aSIPAddress);

		/**
		* Destructor
		*/
		IMPORT_C ~CSIPRecordRouteHeader();


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
		static CSIPRecordRouteHeader* NewL(const CSIPRouteHeaderBase& aHeader);

	private: // Constructors 

		CSIPRecordRouteHeader();
	};

#endif // CSIPRECORDROUTEHEADER_H

// End of File
