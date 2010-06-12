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

// CLASS HEADER
#include "TMSRPHeaderUtil.h"

// INTERNAL INCLUDES
#include "MsrpCommon.h"
#include "CMSRPByteRangeHeader.h"


// -----------------------------------------------------------------------------
// TMSRPHeaderUtil::ConvertToNumberL
// -----------------------------------------------------------------------------
//
TInt TMSRPHeaderUtil::ConvertToNumber( const TDesC8& aString )
    {
	// checking for special characters:
	// Unknown byte range end
	if ( aString == KAsterisk )
		{
		// Range unknown
		return KUnknownRange;
		}
	TLex8 numberToConvert( aString );
	TInt convertedNumber( 0 );
	numberToConvert.Val( convertedNumber );

	return convertedNumber;
	}

// -----------------------------------------------------------------------------
// TMSRPHeaderUtil::AppendStringL
// -----------------------------------------------------------------------------
//
void TMSRPHeaderUtil::AppendStringL( const TDesC8& aString, TDes8& aDest )
	{
	TInt newLength = aString.Length() + aDest.Length();
	if ( newLength > aDest.MaxLength() )
		{
		// not enough room
		User::Leave( KErrArgument );
		}
	aDest.Append( aString );
	}

// -----------------------------------------------------------------------------
// TMSRPHeaderUtil::CheckStringValidity
// -----------------------------------------------------------------------------
//
TBool TMSRPHeaderUtil::CheckStringValidity( const TDesC8& aString )
	{
	if ( !aString.Length() )
		{
		return EFalse;
		}
	
	// Checks the syntax for the Content-Type string only. May need to extend to other headers as well
	
	TChar charToFind( KDividedCharacter );
	TInt matchLoc = aString.Locate( charToFind );
	if( matchLoc == KErrNotFound || matchLoc == (aString.Length() -1) )
	    {
        return EFalse;
	    }
	
	TPtrC8 typePtr = aString.Left(matchLoc);
	TLex8 lex( typePtr );
	TChar chr = lex.Get();
	while ( chr )
		{
		if ( !chr.IsAlphaDigit() && chr != '!' && chr != '#' && chr != '$' && chr != '%' && chr != '&' &&
		      chr != '\'' && chr != '*' && chr != '+' && chr != '-' && chr != '.' && chr != '^' && 
		      chr != '_' && chr != '`' && chr != '{' && chr != '|' && chr != '}' && chr != '~' )
			{
			return EFalse;
			}
		chr = lex.Get();
		}
	
	TPtrC8 subtypePtr = aString.Mid( matchLoc+1 );
	lex.Assign( subtypePtr );
	chr = lex.Get();
	while ( chr )
        {
        if ( !chr.IsAlphaDigit() && chr != '!' && chr != '#' && chr != '$' && chr != '%' && chr != '&' &&
              chr != '\'' && chr != '*' && chr != '+' && chr != '-' && chr != '.' && chr != '^' && 
              chr != '_' && chr != '`' && chr != '{' && chr != '|' && chr != '}' && chr != '~' )
            {
            return EFalse;
            }
        chr = lex.Get();
        }

	return ETrue;
	}

// End of File
