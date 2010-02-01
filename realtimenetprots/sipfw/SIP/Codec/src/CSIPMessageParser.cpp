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
// Name          : CSIPMessageParser.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipmessageparser.h"
#include "CSIPPreParser.h"
#include "TSIPHeaderNameValueIter.h"
#include "CSIPHeaderNameValue.h"
#include "SIPHeaderLookup.h"
#include "sipcodecerr.h"
#include "sipmessage.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipheaderbase.h"
#include "sipuri.h"
#include "uricontainer.h"
#include "_sipcodecdefs.h"

_LIT8(KSIPWithSlash, "SIP/");


// -----------------------------------------------------------------------------
// CSIPMessageParser::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPMessageParser* CSIPMessageParser::NewL ()
	{
	CSIPMessageParser* self= CSIPMessageParser::NewLC ();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPMessageParser::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPMessageParser* CSIPMessageParser::NewLC ()
	{
	CSIPMessageParser* self=new(ELeave) CSIPMessageParser;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPMessageParser::CSIPMessageParser
// -----------------------------------------------------------------------------
//
CSIPMessageParser::CSIPMessageParser()
	{
	}

// -----------------------------------------------------------------------------
// CSIPMessageParser::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPMessageParser::ConstructL ()
	{
	SIPHeaderLookup::OpenL ();
	}

// -----------------------------------------------------------------------------
// CSIPMessageParser::~CSIPMessageParser
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPMessageParser::~CSIPMessageParser()
	{
	SIPHeaderLookup::Close ();
	}

// -----------------------------------------------------------------------------
// CSIPMessageParser::FromText
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPMessageParser::FromText (HBufC8* aInputSIPMessage,
										   CSIPMessage** aSIPMessage)
	{
	TInt parseErrorInHeaders=KErrNone;
	CSIPMessage* sipMessage = 0;
	TRAPD (err, sipMessage = FromTextL (aInputSIPMessage,parseErrorInHeaders));
	*aSIPMessage = sipMessage;
	if (err == KErrNone)
	    {
	    err = parseErrorInHeaders;    
	    }
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPMessageParser::FromTextL
// -----------------------------------------------------------------------------
//
CSIPMessage* CSIPMessageParser::FromTextL (HBufC8* aInputSIPMessage,
										   TInt& aParseErrorInHeaders)
	{
	__ASSERT_ALWAYS (aInputSIPMessage != 0, User::Leave(KErrArgument));

	aParseErrorInHeaders = KErrNone;
	CSIPPreParser* preParser = CSIPPreParser::NewL(aInputSIPMessage);
	CleanupStack::PushL(preParser);
	CSIPMessage* sipMessage = CreateSIPMessageLC (preParser->FirstLineL());

	TSIPHeaderNameValueIter iter (*preParser);
	while (!iter.End())
		{
		CSIPHeaderNameValue* header = iter.NextL ();
		RPointerArray<CSIPHeaderBase> headerArray;
		TRAPD (err, headerArray = 
			SIPHeaderLookup::CreateHeaderL(header->Name(),header->Value()));
		delete header;
		if (err == KErrNoMemory) 
			{
			User::Leave(KErrNoMemory);
			}
		if (err == KErrNone)
			{
			CSIPHeaderBase::PushLC(&headerArray);
			for (TInt i=0; i < headerArray.Count(); i++)
				{
				sipMessage->AddHeaderL (headerArray[i]);
				headerArray[i] = 0;
				}
			CleanupStack::PopAndDestroy(1); // headerArray
			}
		else // ignore header with syntax error
			{
			// Set the first error 
			if (aParseErrorInHeaders == KErrNone) 
				{
				aParseErrorInHeaders = err;
				}
			}
		}

	HBufC8* content = preParser->ContentL();
	sipMessage->SetContent (content);
	CleanupStack::Pop(sipMessage);
	CleanupStack::PopAndDestroy(preParser);
	return sipMessage;
	}

// -----------------------------------------------------------------------------
// CSIPMessageParser::CreateSIPMessageLC
// -----------------------------------------------------------------------------
//
CSIPMessage* CSIPMessageParser::CreateSIPMessageLC (const TDesC8& aFirstLine)
	{
	CSIPMessage* sipMessage = NULL;

	if (aFirstLine.FindF(KSIPWithSlash) == 0)
		{
		sipMessage = CreateSIPResponseLC(aFirstLine);
		}
	else
		{
		sipMessage = CreateSIPRequestLC(aFirstLine);
		}

	return sipMessage;
	}

// -----------------------------------------------------------------------------
// CSIPMessageParser::CreateSIPRequestLC
// -----------------------------------------------------------------------------
//
CSIPRequest* CSIPMessageParser::CreateSIPRequestLC (const TDesC8& aRequestLine)
	{
	__ASSERT_ALWAYS (aRequestLine.Length()>0, 
	                 User::Leave(KErrSipCodecRequestLine));

	//Method SP Request-URI SP SIP-Version
	CSIPRequest* sipRequest = CSIPRequest::NewLC();
	TLex8 lex(aRequestLine);

	//Method
	TPtrC8 method = lex.NextToken();
	sipRequest->SetMethodL(method);

    //Request-URI
    CURIContainer* uri = CURIContainer::DecodeL(lex.NextToken());
    CleanupStack::PushL(uri);
    sipRequest->SetRequestURIL (uri);
    CleanupStack::Pop(uri);

	//SIP-Version
	sipRequest->SetSIPVersionL(lex.NextToken());

	return sipRequest;
	}

// -----------------------------------------------------------------------------
// CSIPMessageParser::CreateSIPResponseLC
// -----------------------------------------------------------------------------
//
CSIPResponse* CSIPMessageParser::CreateSIPResponseLC (const TDesC8& aStatusLine)
	{
	__ASSERT_ALWAYS (aStatusLine.Length()>0, 
	                 User::Leave(KErrSipCodecResponseLine));

	//SIP-Version SP Status-Code SP Reason-Phrase
	CSIPResponse* sipResponse = CSIPResponse::NewL();
	CleanupStack::PushL (sipResponse);
	TLex8 lex(aStatusLine);

	sipResponse->SetSIPVersionL (lex.NextToken());
	sipResponse->SetResponseCodeL (ParseResponseCodeL(lex.NextToken()));
	if (lex.Get() != ' ')
		{
		User::Leave(KErrSipCodecResponseLine);
		}
	sipResponse->SetReasonPhraseL (lex.Remainder());

	return sipResponse;
	}

// -----------------------------------------------------------------------------
// CSIPMessageParser::ParseResponseCodeL
// -----------------------------------------------------------------------------
//
TUint CSIPMessageParser::ParseResponseCodeL (const TDesC8& aResponseCode)
	{
	__ASSERT_ALWAYS (aResponseCode.Length() > 0,
					 User::Leave (KErrSipCodecResponseCode));

	TLex8 lex(aResponseCode);
	TUint chrCount=0;
	lex.Mark();
	TChar chr = lex.Get();
	while (chr)
		{
		chrCount++;
		chr = lex.Get();
		}
    const TInt KCharactersInResponseCode = 3;
	if (chrCount != KCharactersInResponseCode) 
		{
		User::Leave (KErrSipCodecResponseCode);
		}
	lex.UnGetToMark();
	TUint value=0;
	if (lex.Val(value) != KErrNone) 
		{
		User::Leave (KErrSipCodecResponseCode);
		}
	return value;
	}
