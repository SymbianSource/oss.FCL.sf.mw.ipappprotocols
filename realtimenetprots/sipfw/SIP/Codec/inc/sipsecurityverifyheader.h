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
* Name          : sipsecurityverifyheader.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSECURITYVERIFYHEADER_H
#define CSIPSECURITYVERIFYHEADER_H

//  INCLUDES
#include "sipsecurityheaderbase.h"
#include "_sipcodecdefs.h"

// CLASS DECLARATION
/**
* Class for a SIP Security-Verify header.
*
* @lib sipcodec.lib
*/
class CSIPSecurityVerifyHeader : public CSIPSecurityHeaderBase
	{
	public:	// Constructors and destructor

		/**
		* Constructs a CSIPSecurityVerifyHeader from textual representation 
		* of the header's value part.
		* @param aValue a value part of a "Security-Server"-header
		* @return an array containing 1..n instances of 
		* CSIPSecurityVerifyHeader
		*/
		IMPORT_C static RPointerArray<CSIPSecurityVerifyHeader> 
			DecodeL(const TDesC8& aValue);

		/**
		* Creates a new instance of CSIPSecurityVerifyHeader
		* @param aMechanism a Mechanism-Name parameter 
		* in the "Security-Server" header
		* @return a new instance of CSIPSecurityVerifyHeader
		*/
		IMPORT_C static CSIPSecurityVerifyHeader* 
			NewL(const TDesC8& aMechanism);

		/**
		* Creates a new instance of CSIPSecurityVerifyHeader 
		* and puts it to CleanupStack
		* @param aMechanism a Mechanism-Name parameter 
		* in the "Security-Server" header
		* @return a new instance of CSIPSecurityVerifyHeader
		*/
		IMPORT_C static CSIPSecurityVerifyHeader* 
			NewLC(const TDesC8& aMechanism);

        /**
	    * Creates a deep copy of the given CSIPSecurityHeaderBase
	    * @param aHeader to be copied
	    * @returns a new instance of CSIPSecurityVerifyHeader
	    */
	    IMPORT_C static CSIPSecurityVerifyHeader* NewL(const
            CSIPSecurityHeaderBase& aHeader);

	    /**
	    * Creates a deep copy of the given CSIPSecurityHeaderBase and
	    * puts it to CleanupStack
	    * @param aHeader to be copied
	    * @return a new instance of CSIPSecurityVerifyHeader
	    */
	    IMPORT_C static CSIPSecurityVerifyHeader* NewLC(const
            CSIPSecurityHeaderBase& aHeader);

		/**
		* Destructor, deletes the resources of CSIPSecurityVerifyHeader.
		*/
		IMPORT_C ~CSIPSecurityVerifyHeader();


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

		CSIPSecurityVerifyHeader();

	private: // For testing purposes
	
		UNIT_TEST(CSIPSecurityHeaderBaseTest)
    };

#endif // CSIPSECURITYVERIFYHEADER_H

// End of File
