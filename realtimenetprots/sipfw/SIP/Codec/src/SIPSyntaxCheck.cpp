// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SIPSyntaxCheck.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "SIPSyntaxCheck.h"
#include "sipcodecerr.h"
#include <uriutils.h>

_LIT8(KSIP, "SIP");
_LIT8(KTwoDots, "..");

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::Token
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::Token (const TDesC8& aValue, RArray<TSIPChar>* aArray)
	{
	
	if (aValue.Length() == 0) 
		{
		return EFalse;
		}
	TLex8 lex(aValue);
	TSIPChar sipChr = lex.Get();
	while (sipChr)
		{
		if (!sipChr.IsTokenChar( aArray ) )
			{
			return EFalse;
			}
		sipChr = lex.Get();
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::AlphaMaxSize8
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::AlphaMaxSize8 (const TDesC8& aValue)
	{
	if (aValue.Length() == 0) 
		{
		return EFalse;
		}
	TLex8 lex(aValue);
	TSIPChar sipChr = lex.Get();
	TInt count = 1;
	while (sipChr)
		{
		if (!sipChr.IsAlpha()) 
			{
			return EFalse;
			}
		sipChr = lex.Get();
		count++;
		} 
	return (count <= 9);
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::Host
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::Host (const TDesC8& aValue)
	{
    if(aValue.Length() == 0)
        {
        return EFalse;
        } 
    CSIPHostPort::TType hostType;
	return (SIPSyntaxCheck::HostType(aValue,hostType) == KErrNone);
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::Word
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::Word (const TDesC8& aValue)
	{
	if (aValue.Length() == 0) 
		{
		return EFalse;
		}
	TLex8 lex(aValue);
	TSIPChar sipChr = lex.Get();
	while (sipChr)
		{
		if (!sipChr.IsWordChar()) return EFalse;
		sipChr = lex.Get();
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::User
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::User (const TDesC8& aUser)
	{
	if (aUser.Length() == 0) 
		{
		return EFalse;
		}
	TLex8 lex(aUser);
	TSIPChar sipChr = lex.Get();
	while (sipChr)
		{
		if (!(sipChr.IsUnreserved() || sipChr == '%' || sipChr == '&' ||
			  sipChr == '=' || sipChr == '+' || sipChr == '$' ||
			  sipChr == ',' || sipChr == ';' || sipChr == '?' ||
			  sipChr == '/'))
			{
			return EFalse;
			}
		if (sipChr == '%' && !SkipAndCheckEscaped(lex)) // escaped
			{
			return EFalse;
			}
		sipChr = lex.Get();
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::Password
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::Password (const TDesC8& aPassword)
	{
	if (aPassword.Length() == 0) 
		{
		return EFalse;
		}
	TLex8 lex(aPassword);
	TSIPChar sipChr = lex.Get();
	while (sipChr)
		{
		if (!(sipChr.IsUnreserved() || sipChr == '%' || sipChr == '&' ||
			  sipChr == '=' || sipChr == '+' || sipChr == '$' ||
			  sipChr == ','))
			{
			return EFalse;
			}
		if (sipChr == '%' && !SkipAndCheckEscaped(lex)) // escaped
			{
			return EFalse;
			}
		sipChr = lex.Get();
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::StartsAndEndsWithQuotes
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::StartsAndEndsWithQuotes (const TDesC8& aValue)
    {
	// trim and check quotes
	TLex8 lex(aValue);
	lex.SkipSpace();
	TPtrC8 trimmedVal(lex.Remainder());
	const TInt KTwoQuotesLength = 2;
	if (trimmedVal.Length() < KTwoQuotesLength) 
		{
		return EFalse;
		}
	if (trimmedVal.Locate('"') != 0) 
		{
		return EFalse;
		}
	if (trimmedVal.LocateReverse('"') != trimmedVal.Length()-1) 
		{
		return EFalse;
		}
	return ETrue;    
    }

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::QuotedString
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::QuotedString (const TDesC8& aValue)
	{
	// trim and check quotes
	TLex8 lex(aValue);
	lex.SkipSpace();
	TPtrC8 trimmedVal(lex.Remainder());
	if (!StartsAndEndsWithQuotes(trimmedVal))
	    {
	    return EFalse;
	    }
	// remove quotes
	TPtrC8 withoutFirstQuote(trimmedVal.Mid(1));
	TPtrC8 withoutQuotes(withoutFirstQuote.Left(withoutFirstQuote.Length()-1));
	// check value
	return QuotedStringValue(withoutQuotes);
	}
	
// -----------------------------------------------------------------------------
// SIPSyntaxCheck::QuotedStringValue
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::QuotedStringValue(
    const TDesC8& aValue, 
    const TChar& aExtraChr)
	{
	TLex8 lex(aValue);
	TSIPChar sipChr = lex.Get();
	while (sipChr)
		{
		TBool ok = ETrue;
		if (sipChr.IsUTF8NonAsciiStartChar())
			{
			ok = SkipAndCheckNonAscii(sipChr,lex);
			}
		else if (sipChr == '\\')
			{
			sipChr = lex.Get();
			ok = sipChr.IsQuotedPairChar();
			}
		else if (sipChr.IsGdTextChar() || 
		         sipChr == ' ' || 
		         sipChr == '\t' ||
		         sipChr == aExtraChr)
			{
			ok = ETrue;
			}
		else
			{
			ok = EFalse;
			}
		if (!ok) 
			{
			return EFalse;
			}
		sipChr = lex.Get();
		}
	return ETrue;
	}	

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::SIPVersion
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::SIPVersion (const TDesC8& aValue)
	{
	// "SIP" "/" 1*DIGIT "." 1*DIGIT
	if (aValue.Length() < KSIP().Length()+4) 
		{
		return EFalse;
		}
	if (aValue.FindF(KSIP) != 0) 
		{
		return EFalse;
		}
	TLex8 lex(aValue.Mid(KSIP().Length()));
	if (lex.Get() != '/') 
		{
		return EFalse;
		}
	TSIPChar sipChr = lex.Get();
	if (!sipChr.IsDigit()) 
		{
		return EFalse;
		}
	while (sipChr.IsDigit()) 
		{
		sipChr = lex.Get();
		}
	if (sipChr != '.') 
		{
		return EFalse;
		}
	sipChr = lex.Get();
	if (!sipChr.IsDigit()) 
		{
		return EFalse;
		}
	while (sipChr.IsDigit()) 
		{
		sipChr = lex.Get();
		}
	return (sipChr == 0);
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::ReasonPhrase
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::ReasonPhrase (const TDesC8& aValue)
	{
	TLex8 lex(aValue);
	TSIPChar sipChr = lex.Get();
	while (sipChr)
		{
		TBool ok = ETrue;
		if (sipChr.IsUTF8NonAsciiStartChar())
			{
			ok = SkipAndCheckNonAscii(sipChr,lex);
			}
		else if (sipChr == '%') // escaped
			{
			ok = SkipAndCheckEscaped(lex);
			}
		else if (sipChr.IsReserved() || sipChr.IsUnreserved() || 
			     sipChr.IsUTF8ContChar() || sipChr == ' ' || sipChr == '\t')
			{
			ok = ETrue;
			}
		else
			{
			ok = EFalse;
			}
		if (!ok) 
			{
			return EFalse;
			}
		sipChr = lex.Get();
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::UInt
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::UInt (const TDesC8& aValue)
	{
	TUint tmp=0;
	return UInt(aValue,tmp);
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::UInt
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::UInt (const TDesC8& aValue, TUint& aResult)
	{
	if (aValue.Length() == 0) 
		{
		return EFalse;
		}
	TLex8 lex(aValue);
	TUint parsedValue=0;
	if (lex.Val(parsedValue) != KErrNone) 
		{
		return EFalse;
		}
	if (lex.Remainder().Length() != 0) 
		{
		return EFalse;
		}
	aResult = parsedValue;
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::Real
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::Real (const TDesC8& aValue)
	{
	TReal tmp=0;
	return Real(aValue,tmp);
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::Real
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::Real (const TDesC8& aValue, TReal& aResult)
	{
	if (aValue.Length() == 0) 
		{
		return EFalse;
		}
	TLex8 lex(aValue);
	TReal parsedValue=0;
	// SIP uses always dot as a decimal point
	const TChar KDotChr = '.';
	if (lex.Val(parsedValue,KDotChr) != KErrNone) 
		{
		return EFalse;
		}
	if (lex.Remainder().Length() != 0) 
		{
		return EFalse;
		}
	aResult = parsedValue;
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::GenericParamValue
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::GenericParamValue (const TDesC8& aValue)
	{
	// token / host / quoted-string
	if (Token(aValue)) 
		{
		return ETrue;
		}
	if (Host(aValue)) 
		{
		return ETrue;
		}
	return QuotedString(aValue);
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::QValue
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::QValue (const TDesC8& aValue)
	{
	TReal q=0;
	if (!Real(aValue,q)) 
		{
		return EFalse;
		}
	return (q >= 0 && q <= 1);
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::TtlValue
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::TtlValue (const TDesC8& aValue)
	{
	const TUint KMaxTtlValue = 255;
	TUint ttl=0;
	if (!UInt(aValue,ttl)) 
		{
		return EFalse;
		}
	return (ttl <= KMaxTtlValue);
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::HexValue
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::HexValue (const TDesC8& aValue, TInt aLength)
    {
	TLex8 lex(aValue);
	TSIPChar sipChr = lex.Get();
    TInt count = 0;
	while (sipChr.IsHexDigit())
		{
        count++;
		sipChr = lex.Get();
		}
    if (aLength < 0) 
        {
        count = aLength;
        }
	return (count == aLength && sipChr == 0);
    }

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::ExtensionHeaderValue
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::ExtensionHeaderValue (const TDesC8& aValue)
	{
	// CR or LF not allowed (line folds removed by pre-parser)
	if (aValue.Length() == 0) 
        {
        return ETrue;
        }
	TLex8 lex(aValue);
	TSIPChar sipChr = lex.Get();
	while (sipChr)
		{
		TBool ok = ETrue;
		if (sipChr.IsUTF8NonAsciiStartChar())
			{
			ok = SkipAndCheckNonAscii(sipChr,lex);
			}
		else if ((sipChr >= 33 && sipChr <= 126) || 
			     sipChr.IsUTF8ContChar() || 
			     sipChr == ' ' || 
			     sipChr == '\t' ||
			     sipChr == 2) // Used by Yahoo in authentication
			{	
			ok = ETrue;
			}
		else
			{
			ok = EFalse;
			}
		if (!ok) 
			{
			return EFalse;
			}
		sipChr = lex.Get();
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::QuotedTokenWithComma
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::QuotedTokenWithComma (const TDesC8& aValue)
    {
	if (!aValue.Length()) 
		{
		return EFalse;
		}
	TLex8 lex(aValue);
	lex.SkipSpace();
	TSIPChar sipChr = lex.Get();
	if(sipChr != '"') 
		{
		return EFalse;
		}
	sipChr = lex.Get();

	while (sipChr.IsTokenChar() || sipChr == ',' || sipChr == ' ')
		{
		sipChr = lex.Get();
		}
	lex.UnGet();
	sipChr = lex.Get();
   	if (sipChr != '"')
        {
        return EFalse;
        }
	lex.SkipSpace();
	return (lex.Get() == 0);
    }	

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::HostType
// -----------------------------------------------------------------------------
//
TInt SIPSyntaxCheck::HostType (const TDesC8& aHost, 
                               CSIPHostPort::TType& aHostType)
	{
    if (aHost.Length() == 0)
        {
        return KErrSipCodecHost;
        }
    UriUtils::TUriHostType type = UriUtils::HostType(aHost);
    if(type == UriUtils::ETextHost)
	    {
	    if(!ValidHostName(aHost))
		    {
		    aHostType = CSIPHostPort::ESIPNoHost;
		    return KErrSipCodecHost;
		    }
	    aHostType = CSIPHostPort::ESIPHostName;
	    }
    else if(type == UriUtils::EIPv4Host)
	    {
	    aHostType = CSIPHostPort::ESIPIpv4;
	    }
    else // EIPv6Host
	    {
	    aHostType = CSIPHostPort::ESIPIpv6;
	    }
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::Comment
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::Comment(const TDesC8& aComment)
    {
	// trim and check parenthesis
	TLex8 lex(aComment);
	lex.SkipSpace();
	TPtrC8 trimmedVal(lex.Remainder());
	if (trimmedVal.Length() < 2) 
		{
		return EFalse;
		}
	if (trimmedVal.Locate('(') != 0) 
		{
		return EFalse;
		}
	if (trimmedVal.LocateReverse(')') != trimmedVal.Length()-1) 
		{
		return EFalse;
		}
	// remove parenthesis
	TPtrC8 withoutFirstParenthesis(trimmedVal.Mid(1));
	TPtrC8 withoutParenthesis(withoutFirstParenthesis.Left(
                              withoutFirstParenthesis.Length()-1));
	lex.Assign(withoutParenthesis);
	// check value without parenthesis
	TSIPChar sipChr = lex.Get();
	while (sipChr)
		{
		TBool ok = ETrue;
		if (sipChr.IsUTF8NonAsciiStartChar())
			{
			ok = SkipAndCheckNonAscii(sipChr,lex);
			}
        else if ((sipChr >= 33 && sipChr <= 39) || 
                (sipChr >= 42 && sipChr <= 91) || 
                (sipChr >= 93 && sipChr <= 126))
			{
			ok = ETrue;
			}
		else if (sipChr == '\\')
			{
			sipChr = lex.Get();
			ok = sipChr.IsQuotedPairChar();
			}	
        else if (sipChr.IsSpace() || sipChr == '(' || sipChr == ')')
            {
            ok = ETrue;
            }
		else
			{
			ok = EFalse;
			}
		if (!ok) 
			{
			return EFalse;
			}
		sipChr = lex.Get();
		}
	return ETrue;    
    }

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::FeatureValue
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::FeatureValue(const TDesC8& aValue)
    {
	// trim and check quotes
	TLex8 lex(aValue);
	lex.SkipSpace();
	TPtrC8 trimmedVal(lex.Remainder());
	if (!StartsAndEndsWithQuotes(trimmedVal))
	    {
	    return EFalse;
	    }
	// remove quotes
	TPtrC8 withoutFirstQuote(trimmedVal.Mid(1));
	TPtrC8 withoutQuotes(withoutFirstQuote.Left(withoutFirstQuote.Length()-1));
	lex.Assign(withoutQuotes);
    // check value without quotes
    TSIPChar sipChr = lex.Get();
    while(sipChr)
        {
        TBool ok = ETrue;
        if(sipChr.IsTokenChar())
            {
            ok = ETrue;
            }
		else if(sipChr.IsGdTextChar() || 
                sipChr == '#' || 
                sipChr == '=' || 
                sipChr == ':')
			{
			ok = ETrue;
			}
		else
			{
			ok = EFalse;
			}
		if (!ok) 
			{
			return EFalse;
			}
        sipChr = lex.Get();
        }

    lex.SkipSpace();
    return (lex.Get() == 0);
    }

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::Base64Encoded
// -----------------------------------------------------------------------------
//    
TBool SIPSyntaxCheck::Base64Encoded(const TDesC8& aValue)
    {
    TBool valid = ETrue;
    TLex8 lex(aValue);
	TSIPChar sipChr = lex.Get();
	while (sipChr && valid)
		{
		if (sipChr.IsAlphaDigit() || 
		    sipChr == '+' ||
		    sipChr == '/' ||
		    sipChr == '-' ||
		    sipChr == '_' ||
		    sipChr == '=')
			{
			sipChr = lex.Get();
			}
	    else
	        {
	        valid = EFalse;
	        }
		}
    return valid;  
    }

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::SkipAndCheckEscaped
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::SkipAndCheckEscaped (TLex8& aLex)
	{
	if (!aLex.Get().IsHexDigit()) 
		{
		return EFalse;
		}
	return (aLex.Get().IsHexDigit());
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::SkipAndCheckNonAscii
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::SkipAndCheckNonAscii (const TChar& aChr, TLex8& aLex)
	{
	if (aChr >= 192 && aChr <= 253)
		{
		if (aChr <= 223)
			{
			return SkipAndCheckContChars(aLex,1);
			}
		else if (aChr <= 239)
			{
			return SkipAndCheckContChars(aLex,2);
			}
		else if (aChr <= 247)
			{
			return SkipAndCheckContChars(aLex,3);
			}
		else if (aChr <= 251)
			{
			return SkipAndCheckContChars(aLex,4);
			}
		else
			{
			return SkipAndCheckContChars(aLex,5);
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::SkipAndCheckContChars
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::SkipAndCheckContChars (TLex8& aLex, TInt aCount)
	{
	TInt counter = 0;
	TSIPChar chr = 0;
	while (aCount > counter++)
		{
		chr = aLex.Get();
		if (!chr.IsUTF8ContChar())
			{
			return EFalse;
			}
		}
	return ETrue;
	}
    
// -----------------------------------------------------------------------------
// SIPSyntaxCheck::ValidHostName
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::ValidHostName(const TDesC8& aHost)
	{
	if (aHost.Length() == 0 || aHost.Find(KTwoDots) >= 0) 
	    {
	    return EFalse;
	    }
	TLex8 hostLex(aHost);
	TPtrC8 label;
	if (!NextHostLabelOk(hostLex,label))
	    {
	    return EFalse;
	    }
	TPtrC8 lastLabel;
	while (label.Length() > 0)
		{
		lastLabel.Set (label);
		if (!NextHostLabelOk(hostLex,label))
		    {
		    return EFalse;
		    }
		}
	TLex8 lastLabelLex(lastLabel);
	return (lastLabelLex.Peek().IsAlpha());
	}

// -----------------------------------------------------------------------------
// SIPSyntaxCheck::NextHostLabelOk
// -----------------------------------------------------------------------------
//
TBool SIPSyntaxCheck::NextHostLabelOk(TLex8& aLex, TPtrC8& aLabel)
	{
	TUint chrCount=0;
	aLex.Mark();
	TChar chr = aLex.Get();
	if (chr == '-') 
	    {
	    return EFalse;
	    }
	TChar lastChr = chr;
	while (chr != 0 && chr != '.')
		{
		if (!(chr.IsAlphaDigit() || chr == '-'))
		    {
		    return EFalse;
		    }
		chrCount++;
		lastChr = chr;
		chr = aLex.Get();
		}
	if (lastChr == '-')
	    {
	    return EFalse;
	    }
	aLabel.Set(aLex.RemainderFromMark().Left(chrCount));
	return ETrue;
	}    
