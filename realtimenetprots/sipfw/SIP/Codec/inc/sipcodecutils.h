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
* Name        : sipcodecutils.h
* Part of     : SIP Codec
* Interface   : SDK API, SIP Codec API
* Version     : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef SIPCODECUTILS_H
#define SIPCODECUTILS_H

//  INCLUDES
#include <e32base.h>
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPRouteHeaderBase;
class CSIPRouteHeader;
class CSIPRecordRouteHeader;
class CSIPAddress;
class CURIContainer;
class RStringF;
class RWriteStream;

// CLASS DECLARATION
/**
 * Class providing utilities that do not logically belong to
 * any API class and utility functions that are EXPORTed but not published 
 * as part of the API.
 *
 * @lib sipcodec.lib
 */
class SIPCodecUtils
	{
	public:	// New functions
 		
		/**
		* Creates a deep copy of the given CSIPRouteHeaderBase instance
		* and puts it to CleanupStack
		* @param aHeader Route-, Record-Route- or Service-Route-header 
		*        to be copied
		* @returns a new instance of CSIPRouteHeader
		*/
		IMPORT_C static CSIPRouteHeader* 
			CreateRouteHeaderLC(const CSIPRouteHeaderBase& aHeader);
			
		/**
		* Creates a deep copy of the given CSIPRouteHeaderBase instance
		* and puts it to CleanupStack		
		* @param aHeader Route-, Record-Route- or Service-Route-header 
		*        to be copied
		* @returns a new instance of CSIPRecordRouteHeader
		*/
		IMPORT_C static CSIPRecordRouteHeader* 
			CreateRecordRouteHeaderLC(const CSIPRouteHeaderBase& aHeader);
			
		/**
		* Creates a new CSIPAddress instance and puts it to CleanupStack	
		* @param aURI the ownership is transferred
		* @returns a new instance of CSIPAddress
		*/
		IMPORT_C static CSIPAddress* CreateSIPAddressLC(CURIContainer* aURI);				
						
			
    public: // New functions, not imported

		/**
		* Checks the token's syntax and creates RStringF from it.
		* @param aToken A token to be checked.
        * @param aLeaveCode Leaves with this code if the check fails.
		* @returns An openned RStringF. 
        *          RStringF::Close() should be called for this.
		*/	
        static RStringF 
            CheckAndCreateTokenL(const TDesC8& aToken, TInt aLeaveCode);

		/**
		* Removes quotes from the quoted string.
		* @param aValue A quoted string.
		* @param aWithoutQuotes On return contains the string without quotes.
        * @returns KErrNone or KErrArgument error code if the aValue is invalid
		*/	
        static TInt 
            ValueWithoutQuotes(const TDesC8& aValue, TPtrC8& aWithoutQuotes);

		/**
		* Checks the token's syntax and assigns the right value to aBuffer.
        * @param aBuffer On return contains the checked string.  
        * @param aValue A token value.		
        * @param aErr Leaves with this code if the check fails.
		*/	
        static void 
            CheckAndSetValueL(HBufC8*& aBuffer,const TDesC8& aValue,TInt aErr);

		/**
		* Reads the descriptor length and value from the stream.
        * @param aReadStream A stream where to read.		
        * @returns a pointer to the descriptor that was read
        *          from the stream. The ownership is transferred. 
		*/	
        static HBufC8* ReadDescFromStreamL(RReadStream& aReadStream);

    private: // Contructors

		SIPCodecUtils();
		};

#endif // SIPCODECUTILS_H

// End of File
