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
* Name          : sipsecurityserverheader.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSECURITYSERVERHEADER_H
#define CSIPSECURITYSERVERHEADER_H

//  INCLUDES
#include "sipsecurityheaderbase.h"
#include "_sipcodecdefs.h"

// CLASS DECLARATION
/**
* Class for a SIP Security-Server header.
*
* @lib sipcodec.lib
*/
class CSIPSecurityServerHeader : public CSIPSecurityHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CSIPSecurityServerHeader from textual representation 
		* of the header's value part.
		* @param aValue a value part of a "Security-Server"-header
		* @return an array containing 1..n instances of 
		* CSIPSecurityServerHeader
		*/
		IMPORT_C static RPointerArray<CSIPSecurityServerHeader> 
			DecodeL(const TDesC8& aValue);

		/**
		* Creates a new instance of CSIPSecurityServerHeader
		* @param aMechanism a Mechanism-Name parameter 
		* in the "Security-Server" header
		* @return a new instance of CSIPSecurityServerHeader
		*/
		IMPORT_C static CSIPSecurityServerHeader* 
			NewL(const TDesC8& aMechanism);

		/**
		* Creates a new instance of CSIPSecurityServerHeader 
		* and puts it to CleanupStack
		* @param aMechanism a Mechanism-Name parameter 
		* in the "Security-Server" header
		* @return a new instance of CSIPSecurityServerHeader
		*/
		IMPORT_C static CSIPSecurityServerHeader* 
			NewLC(const TDesC8& aMechanism);

		/**
		* Destructor, deletes the resources of CSIPSecurityServerHeader.
		*/
		IMPORT_C ~CSIPSecurityServerHeader();


	public: // From CSIPHeaderBase

		/**
		* From CSIPHeaderBase CloneL
		*/
		IMPORT_C CSIPHeaderBase* CloneL() const;
		
		/**
		* From CSIPHeaderBase Name
		*/		
		IMPORT_C RStringF Name() const;

	public: // New functions, for internal use

		static RPointerArray<CSIPHeaderBase> BaseDecodeL(const TDesC8& aValue);

	private: // Constructors

		CSIPSecurityServerHeader();

	private: // For testing purposes
	
		UNIT_TEST(CSIPSecurityHeaderBaseTest)
    };

#endif // CSIPSECURITYSERVERHEADER_H

// End of File
