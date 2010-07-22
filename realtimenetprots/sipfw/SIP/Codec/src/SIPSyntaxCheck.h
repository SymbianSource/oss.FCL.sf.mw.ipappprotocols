/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : SIPSyntaxCheck.h
* Part of       : Codec
* Parsing utilities class 
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef SIPSYNTAXCHECK_H
#define SIPSYNTAXCHECK_H

#include "siphostport.h"
#include "TSIPChar.h"
#include <e32std.h>


// Parsing utilities class
class SIPSyntaxCheck
	{
public:

	static TBool Token (const TDesC8& aValue , RArray<TSIPChar>* aArray = NULL );
	static TBool AlphaMaxSize8 (const TDesC8& aValue);
	static TBool Host (const TDesC8& aValue);
	static TBool Word (const TDesC8& aValue);
	static TBool User (const TDesC8& aUser);
	static TBool Password (const TDesC8& aPassword);
	static TBool StartsAndEndsWithQuotes (const TDesC8& aValue);
	static TBool QuotedString (const TDesC8& aValue);
	static TBool QuotedStringValue (const TDesC8& aValue, 
	                                const TChar& aExtraChr = 0);
	static TBool SIPVersion (const TDesC8& aValue);
	static TBool ReasonPhrase (const TDesC8& aValue);
	static TBool UInt (const TDesC8& aValue);
	static TBool UInt (const TDesC8& aValue, TUint& aResult);
	static TBool Real (const TDesC8& aValue);
	static TBool Real (const TDesC8& aValue, TReal& aResult);
	static TBool GenericParamValue (const TDesC8& aValue);
	static TBool QValue (const TDesC8& aValue);
	static TBool TtlValue (const TDesC8& aValue);
	static TBool HexValue (const TDesC8& aValue, TInt aLength=-1);
	static TBool ExtensionHeaderValue (const TDesC8& aValue);
	static TBool QuotedTokenWithComma (const TDesC8& aValue);
    static TInt HostType (const TDesC8& aHost, CSIPHostPort::TType& aHostType);
    static TBool Comment(const TDesC8& aComment);
    static TBool FeatureValue(const TDesC8& aValue);
    static TBool Base64Encoded(const TDesC8& aValue);

private:

	static TBool SkipAndCheckEscaped (TLex8& aLex);
	static TBool SkipAndCheckNonAscii (const TChar& aChr, TLex8& aLex);
	static TBool SkipAndCheckContChars (TLex8& aLex, TInt aCount);
	static TBool ValidHostName(const TDesC8& aHost);
	static TBool NextHostLabelOk(TLex8& aLex, TPtrC8& aLabel);

private: // For testing purposes

	UNIT_TEST(CSIPSyntaxCheckTest)
	};

#endif // SIPSYNTAXCHECK_H

// End of File
