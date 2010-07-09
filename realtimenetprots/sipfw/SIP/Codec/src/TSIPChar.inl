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
* Name          : TSIPChar.inl
* Part of       : SIP Codec
* Version       : SIP/4.0 
* TSIPChar::TSIPChar
*
*/



inline TSIPChar::TSIPChar ()
: TChar() 
	{ 
	}

// -----------------------------------------------------------------------------
// TSIPChar::TSIPChar
// -----------------------------------------------------------------------------
//
inline TSIPChar::TSIPChar (TUint aChar)
: TChar() 
	{
	SetChar(aChar);
	}

// -----------------------------------------------------------------------------
// TSIPChar::TSIPChar
// -----------------------------------------------------------------------------
//
inline TSIPChar::TSIPChar (const TChar& aChar)
: TChar() 
	{
	SetChar(aChar);
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsAlpha
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsAlpha () const
	{
	TSIPChar val = *this;
	if (val >= 'a' && val <= 'z') 
		{
		return ETrue;
		}
	return (val >= 'A' && val <= 'Z');
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsAlphaDigit
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsAlphaDigit () const
	{
	return (IsAlpha() || IsDigit());
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsReserved
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsReserved () const
	{
	TSIPChar val = *this;
	if (val == ';' || val == '/' || val == '?' || val == ':' || val == '@' ||
		val == '&' || val == '=' || val == '+' || val == '$' || val == ',')
		{
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsUnreserved
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsUnreserved () const
	{
	return (IsAlphaDigit() || IsMark());
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsMark
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsMark () const
	{
	TSIPChar val = *this;
	if (val == '-' || val == '_' || val == '.' || val == '!' || val == '~' ||
		val == '*' || val == '\'' || val == '(' || val == ')')
		{
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsTokenChar
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsTokenChar (RArray<TSIPChar>* aArray) const
	{
	if (IsAlphaDigit()) 
		{
		return ETrue;
		}
	   TInt length= 0;
	   TSIPChar val = *this;
	   //Allowing '/' token for interoperability issues
	   if (aArray)
	       length = aArray->Count();   
	   if(length)
	       {
	       for (TInt i =0; i<length;i++)
	           {
	           if(val == aArray->operator [](i))
	           return ETrue;
	           }
	       } 
	   if (val == '-' || val == '.' || val == '!' || val == '%' || val == '*' ||
		val == '_' || val == '+' || val == '\'' || val == '~' || val == '`')
	       {
	       return ETrue;
	       }
	  
	   return EFalse;
	 }

// -----------------------------------------------------------------------------
// TSIPChar::IsSeparator
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsSeparator () const
	{
	TSIPChar val = *this;
	if (val == '(' || val == ')' || val == '<' || val == '>' || val == '@' ||
		val == ',' || val == ';' || val == ':' || val == '\\' || val == '"' ||
		val == '/' || val == '[' || val == ']' || val == '?' || val == '=' ||
		val == '{' || val == '}' || val == ' ' || val == '\t')
		{
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsWordChar
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsWordChar () const
	{
	if (IsAlphaDigit()) 
		{
		return ETrue;
		}
	TSIPChar val = *this;
	if (val == '-' || val == '.' || val == '!' || val == '%' || val == '*' ||
		val == '_' || val == '+' || val == '\'' || val == '~' || val == '(' ||
		val == ')' || val == '<' || val == '>' || val == ':' || val == '\\' ||
		val == '"' || val == '/' || val == '[' || val == ']' || val == '?' ||
		val == '{' || val == '}' || val == '`')
		{
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsQuotedPairChar
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsQuotedPairChar () const
	{
	TSIPChar val = *this;
	if (val == '\n' || val == '\r') 
		{
		return EFalse;
		}
	return (val <= 127);
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsUTF8NonAsciiStartChar
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsUTF8NonAsciiStartChar () const
	{
	TSIPChar val = *this;
	if(val >= 192 && val <= 253) 
		{
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsUTF8ContChar
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsUTF8ContChar () const
	{
	TSIPChar val = *this;
	return (val >= 128 && val <= 191);
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsGdTextChar
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsGdTextChar () const
	{
	// LWS character combination is not allowed in our implementation 
	// although part of the BNF. These are stripped of in preparser 
	// so they will not cause trouble in later phases of decoding.
	TSIPChar val = *this;
	if (val == 33 || (val >= 35 && val <= 91) || (val >= 93 && val <= 126))
		{
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// TSIPChar::IsCText
// -----------------------------------------------------------------------------
//
inline TBool TSIPChar::IsCText () const
	{
	TSIPChar val = *this;
	if ((val >= 33 && val <= 39)|| (val >= 42 && val <= 91) || (val >= 93 && val <= 126))
		{
		return ETrue;
		}
	return EFalse;
	}

