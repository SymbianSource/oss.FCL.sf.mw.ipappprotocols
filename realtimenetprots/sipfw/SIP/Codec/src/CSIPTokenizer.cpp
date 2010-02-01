// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : CSIPTokenizer.cpp
// Part of       : Codec
// Version       : SIP/3.0 
//




#include "CSIPTokenizer.h"
#include "sipcodecerr.h"

// -----------------------------------------------------------------------------
// CSIPTokenizer::NewL
// -----------------------------------------------------------------------------
//
CSIPTokenizer* CSIPTokenizer::NewL(
    const TDesC8& aValue,
    const TChar& aSeparator,
    TBool aIgnoreParenthesis,
    TBool aIgnoreLastEmptyValue,
    TBool aSkipSeparatorsBeforeAt)
	{
	CSIPTokenizer* self = 
        CSIPTokenizer::NewLC( aValue,aSeparator, aIgnoreParenthesis,
        					  aIgnoreLastEmptyValue, aSkipSeparatorsBeforeAt );
	CleanupStack::Pop(self);
	return self;
	} 

// -----------------------------------------------------------------------------
// CSIPTokenizer::NewLC
// -----------------------------------------------------------------------------
//
CSIPTokenizer* CSIPTokenizer::NewLC(
    const TDesC8& aValue,
    const TChar& aSeparator,
    TBool aIgnoreParenthesis,
    TBool aIgnoreLastEmptyValue,
    TBool aSkipSeparatorsBeforeAt)
	{
    __ASSERT_ALWAYS (aValue.Length() > 0, User::Leave (KErrSipCodecTokenizer));
	__ASSERT_ALWAYS (aSeparator != '\\', User::Leave (KErrSipCodecTokenizer));
	__ASSERT_ALWAYS (aSeparator != '"', User::Leave (KErrSipCodecTokenizer));
	__ASSERT_ALWAYS (aSeparator != '<', User::Leave (KErrSipCodecTokenizer));
	__ASSERT_ALWAYS (aSeparator != '>', User::Leave (KErrSipCodecTokenizer));
    if(!aIgnoreParenthesis)
        {
        __ASSERT_ALWAYS (aSeparator != '(', User::Leave(KErrSipCodecTokenizer));
        __ASSERT_ALWAYS (aSeparator != ')', User::Leave(KErrSipCodecTokenizer));
        }

	CSIPTokenizer* self = 
        new(ELeave)CSIPTokenizer(aValue,aSeparator,aIgnoreParenthesis,
        						  aIgnoreLastEmptyValue, 
        						  aSkipSeparatorsBeforeAt );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::CSIPTokenizer
// -----------------------------------------------------------------------------
//
CSIPTokenizer::CSIPTokenizer(const TDesC8& aValue, 
                             const TChar& aSeparator,
                             TBool aIgnoreParenthesis,
                             TBool aIgnoreLastEmptyValue,
                             TBool aSkipSeparatorsBeforeAt)
: iOriginalValue (aValue),
  iLexer( aValue ),
  iSeparator (aSeparator),
  iPreviousWasBackSlash (EFalse),
  iInQuotes (EFalse),
  iInAngleBrackets  (EFalse),
  iInParenthesis (EFalse),
  iIgnoreParenthesis (aIgnoreParenthesis),
  iIgnoreLastEmptyValue ( aIgnoreLastEmptyValue ),
  iSkipSeparatorsBeforeAt ( aSkipSeparatorsBeforeAt )
	{
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPTokenizer::ConstructL()
	{
	iTokens = new(ELeave)CPtrC8Array(1);
	TokenizeL();
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::~CSIPTokenizer
// -----------------------------------------------------------------------------
//
CSIPTokenizer::~CSIPTokenizer()
	{
    delete iTokens;
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::Tokens
// -----------------------------------------------------------------------------
//
CPtrC8Array& CSIPTokenizer::Tokens()
	{
	return *iTokens;
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::TokenizeL
// -----------------------------------------------------------------------------
//
void CSIPTokenizer::TokenizeL()
	{
	TChar chr = iLexer.Get();

	while (chr)
		{
		iParsedSoFar++; iTokenLength++;
		if (chr == '\\')
			{
			// If the previous character was already a backslash,
			// then it is quoting the current backslash.
			iPreviousWasBackSlash = !iPreviousWasBackSlash;
			}
		else
			{
			switch (chr)
				{
				case '"' : HandleQuote(); break;
				case '<' : HandleOpeningAngleBracketL(); break;
				case '>' : HandleClosingAngleBracketL(); break;
                case '(' : HandleOpeningParenthesis(); break;
                case ')' : HandleClosingParenthesis(); break;
				default: HandleOtherCharacterL(chr); break;
				}
			iPreviousWasBackSlash = EFalse;
			}
		chr = iLexer.Get();
		}

	CheckMatchingQuotesBracketsL();
	AddLastTokenL(); // Add the last token after all the other tokens.
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::HandleQuote
// -----------------------------------------------------------------------------
//
void CSIPTokenizer::HandleQuote()
	{
	if (iPreviousWasBackSlash) 
		{
		return;
		}

	iInQuotes = !iInQuotes;
	iQuoteCount++;
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::HandleOpeningAngleBracketL
// -----------------------------------------------------------------------------
//
void CSIPTokenizer::HandleOpeningAngleBracketL()
	{
	if (iInQuotes) 
		{
		return; // ignore
		}

	// Nested angle brackets are not allowed
	__ASSERT_ALWAYS (!iInAngleBrackets,
		             User::Leave (KErrSipCodecTokenizer));
	
	iInAngleBrackets = ETrue;
	iAngleBracketCount++;
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::HandleClosingAngleBracketL
// -----------------------------------------------------------------------------
//
void CSIPTokenizer::HandleClosingAngleBracketL()
	{
	if (iInQuotes) 
		{
		return; // ignore
		}

	// Matching opening angle bracket has to exist
	__ASSERT_ALWAYS (iInAngleBrackets,
		             User::Leave (KErrSipCodecTokenizer));
	
	iInAngleBrackets = EFalse;
	iAngleBracketCount++;
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::HandleOpeningParenthesis
// -----------------------------------------------------------------------------
//
void CSIPTokenizer::HandleOpeningParenthesis()
	{
	if (iInQuotes) 
		{
		return; // ignore
		}
	
	iInParenthesis = ETrue;
	iParenthesisCount++;
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::HandleClosingParenthesis
// -----------------------------------------------------------------------------
//
void CSIPTokenizer::HandleClosingParenthesis()
	{
	if (iInQuotes) 
		{
		return; // ignore
		}

	iInParenthesis = EFalse;
	iParenthesisCount++;
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::CheckMatchingQuotesBracketsL
// -----------------------------------------------------------------------------
//
void CSIPTokenizer::CheckMatchingQuotesBracketsL()
	{
	__ASSERT_ALWAYS (iQuoteCount%2 == 0,
		             User::Leave (KErrSipCodecTokenizer));

	__ASSERT_ALWAYS (iAngleBracketCount%2 == 0,
		             User::Leave (KErrSipCodecTokenizer));

    if(!iIgnoreParenthesis)
        {
        __ASSERT_ALWAYS (iParenthesisCount%2 == 0,
                     User::Leave (KErrSipCodecTokenizer));
        }
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::HandleOtherCharacterL
// -----------------------------------------------------------------------------
//
void CSIPTokenizer::HandleOtherCharacterL(const TChar& aChr)
	{
	__ASSERT_ALWAYS (iTokenLength > 0,
		             User::Leave (KErrSipCodecTokenizer));

	if (iInQuotes || 
	    iInAngleBrackets || 
	    (iInParenthesis && !iIgnoreParenthesis))
		{
		return;
		}	
	
	if (aChr != iSeparator) 
		{
		return;
		}	
	
	TPtrC8 remainder(iLexer.Remainder());
	if (aChr == iSeparator && 
	    iSkipSeparatorsBeforeAt &&
	    remainder.Locate('@') >= 0)
	    {
	    CSIPTokenizer* atChrTokenizer = 
	        CSIPTokenizer::NewLC(
	            remainder, '@', EFalse, ETrue, EFalse);
	    
	    TInt atCount = atChrTokenizer->Tokens().Count() - 1;	    
	    
	    CleanupStack::PopAndDestroy(atChrTokenizer);
	    
	    if (atCount > 0)
	        {
	        return; // skip the separators before '@'
	        }
	    }	

    TUint startPos = iParsedSoFar - iTokenLength;
	TPtrC8 token (iOriginalValue.Mid(startPos, iTokenLength-1));
	iTokens->AppendL(token);

	// Start a new token
	iTokenLength = 0;
	}

// -----------------------------------------------------------------------------
// CSIPTokenizer::AddLastTokenL
// -----------------------------------------------------------------------------
//
void CSIPTokenizer::AddLastTokenL()
	{
	if ( iIgnoreLastEmptyValue && !(iTokenLength > 0))
		{
		return;
		}
		
	__ASSERT_ALWAYS (iTokenLength > 0,
		             User::Leave (KErrSipCodecTokenizer));

    TUint startPos = iParsedSoFar - iTokenLength;
	TPtrC8 token (iOriginalValue.Mid(startPos, iTokenLength));

	iTokens->AppendL(token);
	}
