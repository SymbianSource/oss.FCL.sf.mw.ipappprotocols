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

#ifndef __TMSRPUTIL_H__
#define __TMSRPUTIL_H__

//	INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CMSRPToPathHeader;

//	CLASS DEFINITION
/**
 * Static utility class, used by MSRP Client
 */
class TMSRPUtil
	{

public:

	/**
	 * Convert given string to a number, leaves if string cannot be converted
	 * @param aString string to be converted
	 * @return string converted to number
	 */
    static TInt ConvertToNumber( const TDesC8& aString );

    /**
    * Extract session-id from To-Path or From-path value field
	* @param aValueField from-path or to-path value field
	* @return buffer containing the session-id, ownership is transferred
    */
	static HBufC8* ExtractSessionIdFromPathLC( TDesC8& aValueField );
	};

#endif	// __TMSRPUTIL_H__

// End of File
