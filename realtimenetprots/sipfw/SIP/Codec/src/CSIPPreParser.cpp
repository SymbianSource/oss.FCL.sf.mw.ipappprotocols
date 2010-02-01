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
// Name          : CSIPPreParser.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPPreParser.h"
#include "CSIPHeaderNameValue.h"
#include "sipcodecerr.h"

_LIT8 (KCRLFCRLF, "\r\n\r\n");
_LIT8 (KCRCRLF, "\r\r\n");
_LIT8 (KLFCRLF, "\n\r\n");

// -----------------------------------------------------------------------------
// CSIPPreParser::NewL
// -----------------------------------------------------------------------------
//
CSIPPreParser* CSIPPreParser::NewL (HBufC8* aSipMessageBuf)
	{
	CSIPPreParser* self= CSIPPreParser::NewLC (aSipMessageBuf);
	CleanupStack::Pop(self);
	return self;
	} 

// -----------------------------------------------------------------------------
// CSIPPreParser::NewLC
// -----------------------------------------------------------------------------
//
CSIPPreParser* CSIPPreParser::NewLC (HBufC8* aSipMessageBuf)
	{
	__ASSERT_ALWAYS (aSipMessageBuf!=0, User::Leave(KErrSipCodecPreParser));
	__ASSERT_ALWAYS (aSipMessageBuf->Length()>0, User::Leave(KErrSipCodecPreParser));

	CSIPPreParser* self= new(ELeave)CSIPPreParser (*aSipMessageBuf);
	CleanupStack::PushL(self);
	self->ConstructL(aSipMessageBuf);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::CSIPPreParser
// -----------------------------------------------------------------------------
//
CSIPPreParser::CSIPPreParser (const TDesC8& aSipMessageDes)
 : iLexBuf(aSipMessageDes)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPPreParser::ConstructL (HBufC8* aSipMessageBuf)
	{
	CreateHeaderBufL (*aSipMessageBuf);
	ParseL ();
	// aSipMessageBuf ownership taken only after the pre-parsing has succeeded.
	iSipMessageBuf = aSipMessageBuf;
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::~CSIPPreParser
// -----------------------------------------------------------------------------
//
CSIPPreParser::~CSIPPreParser ()
	{
    iHeaderStartArray.Close();
	delete iHeaderBuf;
	delete iSipMessageBuf;
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::ParseL
// -----------------------------------------------------------------------------
//
void CSIPPreParser::ParseL ()
	{
	TChar chr = GetCharL();
	while (chr == ' ' || chr == '\t' || chr == '\r' || chr == '\n')
		{
		chr = GetCharL();
		}

	TBool headerPartEnds = EFalse;
	while (!headerPartEnds)
		{
		switch (chr)
			{
			case '\n': headerPartEnds = HandleLfL(); break;
			case '\r': headerPartEnds = HandleCrL(); break;
			default: iHeaderBuf->Des().Append(chr); break;
			}
		if (!headerPartEnds)
		    {
		    chr = GetChar();
		    }
		}
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::FirstLineL
// -----------------------------------------------------------------------------
//
TPtrC8 CSIPPreParser::FirstLineL ()
	{
	// The first line of any SIP message is a request line or a status line.
	TPtrC8 firstLine(GetLineL(0));
	return firstLine;
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::ContentL
// -----------------------------------------------------------------------------
//
HBufC8* CSIPPreParser::ContentL ()
	{
	// The ownership of the content is given to the caller.
	// The function is not re-entrant. Content can be only asked once.
	// The purpose of this is to avoid unnecessary copying.
	__ASSERT_ALWAYS (iSipMessageBuf != 0, User::Leave (KErrSipCodecPreParser));

	// Delete the header part from the original SIP message. "Delete" does not
	// free any memory. It just changes the starting position of the descriptor
	// in the original buffer.
	TInt originalLength = iSipMessageBuf->Length();
	iSipMessageBuf->Des().Delete (0, iOriginalHeaderPartLength);
	TInt contentLength = originalLength - iOriginalHeaderPartLength;

	// With relatively big content parts try to avoid unnecessary copying. 
	HBufC8* content = NULL;
	if (contentLength < iOriginalHeaderPartLength)
		{
		// Relatively small content part. Make a copy. See HBufC8::ReAllocL.
		content = iSipMessageBuf->ReAllocL (contentLength);
		}
	else
		{
		// Relatively big content part. Do not copy into smaller buffer.
		content = iSipMessageBuf;
		}
	iSipMessageBuf = 0;
	return content;
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::HeaderCount
// -----------------------------------------------------------------------------
//
TUint CSIPPreParser::HeaderCount ()
	{
	return iHeaderStartArray.Count();
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::ParseNameValueL
// -----------------------------------------------------------------------------
//
CSIPHeaderNameValue* CSIPPreParser::ParseNameValueL (TInt aIndex)
	{
	__ASSERT_ALWAYS (aIndex < iHeaderStartArray.Count(), 
					User::Leave(KErrSipCodecPreParser));

	TUint lineStartPos = iHeaderStartArray[aIndex];
    TPtrC8 line (GetLineL(lineStartPos));
	TInt colonPos = line.Locate (':'); // find the first colon

	__ASSERT_ALWAYS (colonPos > 0, User::Leave(KErrSipCodecPreParser));

	TPtrC8 name(line.Left(colonPos));
	TPtrC8 value(line.Mid(colonPos+1));

	return CSIPHeaderNameValue::NewL(name, value);
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::GetLineL
// -----------------------------------------------------------------------------
//
TPtrC8 CSIPPreParser::GetLineL (TInt aLineStartPosition)
	{
    __ASSERT_ALWAYS (aLineStartPosition < iHeaderBuf->Length(),
					User::Leave(KErrSipCodecPreParser));

	TPtrC8 lineStartBuf (iHeaderBuf->Des().Mid (aLineStartPosition));

	// All the line ends (CR,LF or CRLF) have been replaced by LF.
	TInt lineEndPosition = lineStartBuf.Locate('\n');
	
    __ASSERT_ALWAYS (lineEndPosition > 0, User::Leave (KErrSipCodecPreParser));

	return lineStartBuf.Left(lineEndPosition);
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::CreateHeaderBufL
// -----------------------------------------------------------------------------
//
void CSIPPreParser::CreateHeaderBufL (const TDesC8& aSipMessageDes)
	{
	TLex8 lex(aSipMessageDes);
	lex.SkipSpace();
    TInt headerPartEndPosition = 0;
    FindHeaderPartEndPositionL (lex.Remainder(),headerPartEndPosition);
	if (headerPartEndPosition <= 0) 
		{
		User::Leave (KErrSipCodecPreParser);
		}
	iHeaderBuf = HBufC8::NewL(headerPartEndPosition+1);
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::FindHeaderPartEndPositionL
// -----------------------------------------------------------------------------
//
void CSIPPreParser::FindHeaderPartEndPositionL (
    const TDesC8& aSipMessageDes,
    TInt& aPosition)
	{
	// The header part may end only with CRLFCRLF, CRCRLF or LFCRLF
	RArray<TInt> positions;
	CleanupClosePushL(positions);

	// CRLFCRLF
	TInt CrLfCrLfPos = aSipMessageDes.Find(KCRLFCRLF);
    if (CrLfCrLfPos >= 0) 
        {
        User::LeaveIfError(positions.Append(CrLfCrLfPos));
        }
	
	// CRCRLF
	TInt CrCrLfPos = aSipMessageDes.Find(KCRCRLF);
	if (CrCrLfPos >= 0) 
        {
        User::LeaveIfError(positions.InsertInOrder(CrCrLfPos));
        }
	
	// LFCRLF
	TInt LfCrLfPos = aSipMessageDes.Find(KLFCRLF);
	if (LfCrLfPos >= 0) 
        {
        User::LeaveIfError(positions.InsertInOrder(LfCrLfPos));
        }

	if (positions.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}

	aPosition = positions[0];
	CleanupStack::PopAndDestroy(); // positions
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::HandleLfL
// -----------------------------------------------------------------------------
//
TBool CSIPPreParser::HandleLfL ()
	{
	TChar chr = GetCharL ();
	switch (chr)
		{
		case ' ' : // LFSP 
		case '\t': // LFHTAB
			// The header continues on the next line.
			SkipSpacesAndTabsL ();
			iHeaderBuf->Des().Append(' '); // replace by one space
			break;

		case '\r': // LFCR
			chr = GetCharL ();
			if (chr == '\n') // LFCRLF
				{
				// Header part ends according to the old SIP specs.
				// Replace LFCRLF with LF to simplify name-value parsing.
				iHeaderBuf->Des().Append('\n');
				return ETrue; 
				}
			else // LFCR"AnyText" not allowed.
				{
				User::Leave (KErrSipCodecPreParser);
				}

		case '\n': // LFLF not allowed.
			User::Leave (KErrSipCodecPreParser);

		default: // LF"AnyText"
			MoveToNextLineL ();
			break;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::HandleCrL
// -----------------------------------------------------------------------------
//
TBool CSIPPreParser::HandleCrL ()
	{
	TChar chr = GetCharL ();
	switch (chr)
		{
		case ' ' : // CRSP
		case '\t': // CRHTAB
			// The header continues on the next line.
			SkipSpacesAndTabsL ();
			iHeaderBuf->Des().Append(' '); // replace by one space
			break;

		case '\r': // CRCR
			chr = GetCharL ();
			if (chr == '\n') // CRCRLF
				{
				// Header part ends according to the old SIP specs.
				// Replace CRCRLF with LF to simplify name-value parsing.
				iHeaderBuf->Des().Append('\n');
				return ETrue;
				}
			else // CRCR"AnyText" not allowed.
				{
				User::Leave (KErrSipCodecPreParser); 
				}

		case '\n': // CRLF
			chr = GetCharL ();
			switch (chr)
				{
				case '\n': // CRLFLF not allowed.
					User::Leave (KErrSipCodecPreParser);

				case ' ' : case '\t': // CRLFSP or CRLFHTAB
					// The header continues on the next line.
					SkipSpacesAndTabsL ();
					iHeaderBuf->Des().Append(' '); // replace by one space
					break;

				case '\r': // CRLFCR
					chr = GetCharL ();
					if (chr == '\n') // CRLFCRLF. Header part ends.
						{
						// Replace with LF to simplify name-value parsing.
						iHeaderBuf->Des().Append('\n');
						return ETrue;
						}
					else // CRLFCR"AnyText" not allowed.
						{
						User::Leave (KErrSipCodecPreParser); 
						}

				default: // CRLF"AnyText"
					MoveToNextLineL ();
					break;
				}
			break;
	
		default: // CR"AnyText"
			MoveToNextLineL ();
			break;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::SkipSpacesAndTabsL
// -----------------------------------------------------------------------------
//
void CSIPPreParser::SkipSpacesAndTabsL ()
	{
	TChar chr = GetCharL ();
	while (chr == ' ' || chr == '\t')
		{
		chr = GetCharL();
		}
	UnGetChar(); // Put back the last one that was not a space or a tab
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::MoveToNextLineL
// -----------------------------------------------------------------------------
//
void CSIPPreParser::MoveToNextLineL ()
	{
	UnGetChar();
	// Replace the line end (CR, LF or CRLF) with LF
	iHeaderBuf->Des().Append('\n');
	TUint nextLineStartPosition = iHeaderBuf->Length();
	User::LeaveIfError (iHeaderStartArray.Append(nextLineStartPosition));
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::GetCharL
// -----------------------------------------------------------------------------
//
inline TChar CSIPPreParser::GetCharL()
	{
    TChar chr = GetChar();
	if (chr == 0) 
        {
        User::Leave (KErrSipCodecPreParser);
        }
    return chr;
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::GetChar
// -----------------------------------------------------------------------------
//
inline TChar CSIPPreParser::GetChar()
	{
	TChar chr = iLexBuf.Get();
	if (chr != 0) iOriginalHeaderPartLength++;
    return chr;
	}

// -----------------------------------------------------------------------------
// CSIPPreParser::UnGetChar
// -----------------------------------------------------------------------------
//
inline void CSIPPreParser::UnGetChar()
	{
	iLexBuf.UnGet();
	iOriginalHeaderPartLength--;
	}
