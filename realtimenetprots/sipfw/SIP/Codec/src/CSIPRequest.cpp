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
// Name          : CSIPRequest.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "siprequest.h"
#include "sipuri.h"
#include "siphostport.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "uricontainer.h"
#include "_sipcodecdefs.h"


_LIT8 (KSpace, " ");

// -----------------------------------------------------------------------------
// CSIPRequest::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRequest* CSIPRequest::NewL (RStringF aMethod)
	{
	CSIPRequest* self = CSIPRequest::NewLC(aMethod);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRequest::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRequest* CSIPRequest::NewLC (RStringF aMethod)
	{
	CSIPRequest* self = new(ELeave)CSIPRequest;
	CleanupStack::PushL(self);
	self->ConstructL(aMethod);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRequest::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRequest* CSIPRequest::NewL ()
	{
	CSIPRequest* self = CSIPRequest::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRequest::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRequest* CSIPRequest::NewLC()
	{
	CSIPRequest* self = new(ELeave) CSIPRequest;
	CleanupStack::PushL (self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRequest::CSIPRequest
// -----------------------------------------------------------------------------
//
CSIPRequest::CSIPRequest()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRequest::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRequest::ConstructL(RStringF aMethod)
	{
	CSIPMessage::ConstructL();
	SetMethodL(aMethod);
	}

// -----------------------------------------------------------------------------
// CSIPRequest::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRequest::ConstructL ()
	{
	CSIPMessage::ConstructL();
	}

// -----------------------------------------------------------------------------
// CSIPRequest::~CSIPRequest
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRequest::~CSIPRequest()
	{
	iMethod.Close();
	delete iRequestURI;
	}

// -----------------------------------------------------------------------------
// CSIPRequest::IsRequest
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPRequest::IsRequest() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPRequest::SetMethodL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPRequest::SetMethodL (const TDesC8& aMethod)
	{
    RStringF tmpString = 
        SIPCodecUtils::CheckAndCreateTokenL(aMethod, KErrSipCodecSIPMethod);
	iMethod.Close();
	iMethod = tmpString;
	}

// -----------------------------------------------------------------------------
// CSIPRequest::SetMethodL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPRequest::SetMethodL (RStringF aMethod)
	{
	SetMethodL(aMethod.DesC());
	}

// -----------------------------------------------------------------------------
// CSIPRequest::Method
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPRequest::Method() const
	{
	return iMethod;
	}

// -----------------------------------------------------------------------------
// CSIPRequest::SetRequestURIL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPRequest::SetRequestURIL (CURIContainer* aRequestURI)
	{
	__ASSERT_ALWAYS (aRequestURI != 0, User::Leave (KErrArgument));

	delete iRequestURI; iRequestURI = 0;
	iRequestURI = aRequestURI;
    if(iRequestURI->IsSIPURI())
        {
	    iRequestURI->SIPURI()->DeleteParam(SIPStrings::StringF(SipStrConsts::EMethod));
	    iRequestURI->SIPURI()->DeleteAllURIHeaders();
        }
	}

// -----------------------------------------------------------------------------
// CSIPRequest::RequestURI
// -----------------------------------------------------------------------------
//
EXPORT_C CURIContainer* CSIPRequest::RequestURI()
	{
	return iRequestURI;
	}

// -----------------------------------------------------------------------------
// CSIPRequest::ToTextFirstLineLC
// From CSIPMessage:
// -----------------------------------------------------------------------------
//
HBufC8* CSIPRequest::ToTextFirstLineLC () const
	{
	TPtrC8 sipVersion(SIPVersion().DesC());
	TPtrC8 method(Method().DesC());
	TUint requestLineLength = method.Length() + KSpace().Length() +
							  sipVersion.Length();
	HBufC8* requestURIAsText = 0;
	if (iRequestURI != 0)
		{
		requestURIAsText = iRequestURI->ToTextL();
		CleanupStack::PushL(requestURIAsText);
		requestLineLength += requestURIAsText->Length() + KSpace().Length();
		}
	
	HBufC8* requestLine = HBufC8::NewL(requestLineLength);
	if (requestURIAsText != 0) 
		{
		CleanupStack::Pop(requestURIAsText);
		}
	CleanupStack::PushL(requestLine);
	TPtr8 requestLinePtr(requestLine->Des());
	requestLinePtr.Append(method);
	requestLinePtr.Append(KSpace);
	if (requestURIAsText != 0)
		{
		requestLinePtr.Append(*requestURIAsText);
		requestLinePtr.Append(KSpace);
		}
	requestLinePtr.Append(sipVersion);

	delete requestURIAsText;

	return requestLine;
	}
