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
// Name          : CSIPResponse.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipresponse.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"

_LIT8 (KSpace, " ");
_LIT8 (KTUintFormat, "%u");
const TUint KMinimumResponseCode = 100;
const TUint K200ResponseCode = 200;
const TUint K300ResponseCode = 300;
const TUint K400ResponseCode = 400;
const TUint K500ResponseCode = 500;
const TUint K600ResponseCode = 600;
const TUint KMaximumResponseCode = 699;

// -----------------------------------------------------------------------------
// CSIPResponse::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPResponse* CSIPResponse::NewL(TUint aResponseCode,
										  RStringF aReasonPhrase)
	{
	CSIPResponse* self = CSIPResponse::NewLC (aResponseCode, aReasonPhrase);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPResponse::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPResponse* CSIPResponse::NewLC(TUint aResponseCode,
										   RStringF aReasonPhrase)
	{
	CSIPResponse* self = new(ELeave)CSIPResponse;
	CleanupStack::PushL(self);
	self->ConstructL(aResponseCode, aReasonPhrase);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPResponse::NewL
// -----------------------------------------------------------------------------
//
CSIPResponse* CSIPResponse::NewL()
	{
	CSIPResponse* self = CSIPResponse::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPResponse::NewLC
// -----------------------------------------------------------------------------
//
CSIPResponse* CSIPResponse::NewLC()
	{
	CSIPResponse* self = new(ELeave)CSIPResponse;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPResponse::CSIPResponse
// -----------------------------------------------------------------------------
//
CSIPResponse::CSIPResponse()
	{
	}

// -----------------------------------------------------------------------------
// CSIPResponse::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPResponse::ConstructL(TUint aResponseCode, RStringF aReasonPhrase)
	{
	CSIPMessage::ConstructL();
	SetResponseCodeL(aResponseCode);
	SetReasonPhraseL(aReasonPhrase);
	}

// -----------------------------------------------------------------------------
// CSIPResponse::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPResponse::ConstructL()
	{
	CSIPMessage::ConstructL ();
	}

// -----------------------------------------------------------------------------
// CSIPResponse::~CSIPResponse
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPResponse::~CSIPResponse()
	{
	iReasonPhrase.Close();
	}

// -----------------------------------------------------------------------------
// CSIPResponse::IsRequest
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPResponse::IsRequest() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPResponse::SetReasonPhraseL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPResponse::SetReasonPhraseL (RStringF aReasonPhrase)
	{
	SetReasonPhraseL(aReasonPhrase.DesC());
	}

// -----------------------------------------------------------------------------
// CSIPResponse::ReasonPhrase
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPResponse::ReasonPhrase() const
	{
	return iReasonPhrase;
	}

// -----------------------------------------------------------------------------
// CSIPResponse::SetResponseCodeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPResponse::SetResponseCodeL (TUint aResponseCode)
	{
	__ASSERT_ALWAYS (aResponseCode >= KMinimumResponseCode && 
	                 aResponseCode <= KMaximumResponseCode,
					 User::Leave(KErrSipCodecResponseCode));

	iResponseCode = aResponseCode;
	}

// -----------------------------------------------------------------------------
// CSIPResponse::ResponseCode
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPResponse::ResponseCode() const
	{
	return iResponseCode;
	}

// -----------------------------------------------------------------------------
// CSIPResponse::Type
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPResponse::TType CSIPResponse::Type () const
    {
    TType type;
    if (iResponseCode < K200ResponseCode)
        {
        type = E1XX;
        }
    else if (iResponseCode < K300ResponseCode)
        {
        type = E2XX;
        }
    else if (iResponseCode < K400ResponseCode)
        {
        type = E3XX;
        }
    else if (iResponseCode < K500ResponseCode)
        {
        type = E4XX;
        }
    else if (iResponseCode < K600ResponseCode)
        {
        type = E5XX;
        }
    else
        {
        type = E6XX;
        }
    return type;
    }

// -----------------------------------------------------------------------------
// CSIPResponse::IsErrorResponse
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CSIPResponse::IsErrorResponse () const
    {
    return (iResponseCode >= K300ResponseCode);
    }

// -----------------------------------------------------------------------------
// CSIPResponse::ToTextFirstLineLC
// From CSIPMessage:
// -----------------------------------------------------------------------------
//
HBufC8* CSIPResponse::ToTextFirstLineLC () const
	{
	TPtrC8 sipVersion(SIPVersion().DesC());
	TPtrC8 reasonPhrase(ReasonPhrase().DesC());
	const TInt KMaxResponseAsTextLength = 3;
	TBuf8<KMaxResponseAsTextLength> responseCodeAsText;
	responseCodeAsText.Format(KTUintFormat, iResponseCode);

	TUint statusLineLength = sipVersion.Length() + KSpace().Length() +
							 responseCodeAsText.Length() + KSpace().Length() +
							 reasonPhrase.Length();

	HBufC8* statusLine = HBufC8::NewLC(statusLineLength);
	TPtr8 statusLinePtr(statusLine->Des());
	statusLinePtr.Append(sipVersion);
	statusLinePtr.Append(KSpace);
	statusLinePtr.Append(responseCodeAsText);
	statusLinePtr.Append(KSpace);
	statusLinePtr.Append(reasonPhrase);

	return statusLine;
	}
	
// -----------------------------------------------------------------------------
// CSIPResponse::SetReasonPhraseL
// -----------------------------------------------------------------------------
//
void CSIPResponse::SetReasonPhraseL (const TDesC8& aReasonPhrase)
	{
	if (!SIPSyntaxCheck::ReasonPhrase(aReasonPhrase))
		{
		User::Leave(KErrSipCodecReasonPhrase);
		}
	RStringF tmpString = SIPStrings::Pool().OpenFStringL(aReasonPhrase);
	iReasonPhrase.Close();
	iReasonPhrase = tmpString;
	}	
