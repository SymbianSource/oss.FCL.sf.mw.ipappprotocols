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
* Name          : CSIPTokenizer.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPTOKENIZER_H
#define CSIPTOKENIZER_H

#include <e32base.h>
#include <badesca.h>
#include "_sipcodecdefs.h"


class CSIPTokenizer : public CBase
	{
public:

	static CSIPTokenizer* NewL (const TDesC8& aValue, 
                                const TChar& aSeparator, 
                                TBool aNoticeParenthesis = EFalse,
                                TBool aIgnoreLastEmpty   = EFalse,
                                TBool aSkipSeparatorsBeforeAt = EFalse);
	static CSIPTokenizer* 
		NewLC (const TDesC8& aValue, 
               const TChar& aSeparator,
               TBool aNoticeParenthesis = EFalse,
               TBool aIgnoreLastEmpty   = EFalse,
               TBool aSkipSeparatorsBeforeAt = EFalse);

    ~CSIPTokenizer ();

	CPtrC8Array& Tokens ();

private:

    CSIPTokenizer (const TDesC8& aValue, 
                   const TChar& aSeparator,
                   TBool aNoticeParenthesis,
                   TBool aIgnoreLastEmpty,
                   TBool aSkipSeparatorsBeforeAt);
	void ConstructL ();

	void TokenizeL ();
	void HandleQuote ();
	void HandleOpeningAngleBracketL ();
	void HandleClosingAngleBracketL ();
    void HandleOpeningParenthesis();
    void HandleClosingParenthesis();
	void CheckMatchingQuotesBracketsL ();
	void HandleOtherCharacterL (const TChar& aChr);
	void AddLastTokenL ();

private: // Data

	CPtrC8Array* iTokens;
	TPtrC8 iOriginalValue;
	TLex8 iLexer;
	TChar iSeparator;
	TUint iParsedSoFar;
	TUint iTokenLength;
	TBool iPreviousWasBackSlash;
	TBool iInQuotes;
	TBool iInAngleBrackets;
    TBool iInParenthesis;
	TUint iQuoteCount;
	TUint iAngleBracketCount;
    TUint iParenthesisCount;
    TBool iIgnoreParenthesis;
    TBool iIgnoreLastEmptyValue;
    TBool iSkipSeparatorsBeforeAt;

private: // For testing purposes

    UNIT_TEST(CSIPTokenizerTest)
	};

#endif // CSIPTOKENIZER_H

// End of File
