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

#ifndef __TMSRPHEADERUTIL_H__
#define __TMSRPHEADERUTIL_H__

//	INCLUDES
#include <e32base.h>

//	CLASS DEFINITION
/**
 * Static utility class, used by MSRP Client
 */
class TMSRPHeaderUtil
	{

public:

	/**
	 * Convert given string to a number, leaves if string cannot be converted
	 * @param aString string to be converted
	 * @return string converted to number
	 */
    static TInt ConvertToNumber( const TDesC8& aString );

	/**
	 * Append a string from a destination to source descriptor
	 * @param aSource string to be added
	 * @param aDest destination descriptor
	 */
    static void AppendStringL( const TDesC8& aString, TDes8& aDest );

	/**
	 * Checks that given string consists of only the allowed characters
	 * @param aString string to check
	 * @return True if string valid, false if contained illegal characters
	 */
    static TBool CheckStringValidity( const TDesC8& aString );
	};

#endif	// __TMSRPHEADERUTIL_H__

// End of File
