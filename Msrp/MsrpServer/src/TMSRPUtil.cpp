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

// INCLUDES
#include <Uri8.h> 

// CLASS HEADER
#include "TMSRPUtil.h"

// INTERNAL INCLUDES
#include "MsrpCommon.h"

// -----------------------------------------------------------------------------
// TMSRPUtil::ConvertToNumberL
// -----------------------------------------------------------------------------
//
TInt TMSRPUtil::ConvertToNumber( const TDesC8& aString )
    {
	// checking for special characters:
	// Unknown byte range end
	if ( aString == KAsterisk )
		{
		// Range unknown
		return KUnknownRange;
		}
	TLex8 numberToConvert( aString );
	TUint convertedNumber( 0 );
	numberToConvert.Val( convertedNumber, EDecimal );

	return convertedNumber;
	}

// -----------------------------------------------------------------------------
// CMSRPListeners::ExractSesssionIdFromToPath
// -----------------------------------------------------------------------------
//
HBufC8* TMSRPUtil::ExtractSessionIdFromPathLC( TDesC8& aMsrpPath )
	{
	// parsing the descriptor URL
	// first is the scheme

	TUriParser8 parser;
    User::LeaveIfError( parser.Parse( aMsrpPath ) ); 
    
	if(!parser.IsPresent(EUriPort))
		{
		User::Leave(KErrArgument);
		}

    const TDesC8& remPort = parser.Extract( EUriPort );
    
	TUint portNum;
    TLex8 portLex(remPort);
    User::LeaveIfError( portLex.Val( portNum ) ); //Just validation.Port number is not used

    
    if(!parser.IsPresent(EUriPath))
		{
		User::Leave(KErrArgument);
		}

    const TDesC8& remotePath = parser.Extract( EUriPath );
    
    TInt loc = remotePath.Find( KSemicolon );
	if(loc == KErrNotFound)
		{
		User::Leave(KErrArgument);
		}

    TPtrC8 sessionIDPtr;
	sessionIDPtr.Set( remotePath.Mid( 1, loc-1) );
	User::LeaveIfError(!sessionIDPtr.Length()?KErrArgument:KErrNone); //No Session ID. ; at the beginning.
	
	TInt loc2 = remotePath.LocateReverse(KSemiColonCharacter);
	User::LeaveIfError(loc2!=loc?KErrArgument:KErrNone); //Two ; in Path
	
	HBufC8* sessionId = HBufC8::NewLC( sessionIDPtr.Length());
	*sessionId = sessionIDPtr;

	return sessionId;
	}

// End of File
