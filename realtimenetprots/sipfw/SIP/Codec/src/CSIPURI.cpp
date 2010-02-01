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
// Name          : CSIPURI.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipuri.h"
#include "CSIPURIParams.h"
#include "CSIPURIHeaders.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include <uricommon.h>
#include "_sipcodecdefs.h"

_LIT8(KSIP, "sip");
_LIT8(KSIPS, "sips");
_LIT8(KColon, ":");
_LIT8(KSemiColon, ";");
_LIT8(KAt, "@");
_LIT8(KQuestionMark, "?");

// -----------------------------------------------------------------------------
// CSIPURI::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPURI* CSIPURI::DecodeL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, User::Leave(KErrSipCodecURI));

	CSIPURI* sipUri = CSIPURI::NewLC();

	sipUri->ParseL(sipUri->ParseSipSchemeL(aValue));

	CleanupStack::Pop(sipUri);
	return sipUri;
	}

// -----------------------------------------------------------------------------
// CSIPURI::NewL
// -----------------------------------------------------------------------------
//
CSIPURI* CSIPURI::NewL (const CSIPURI& aSIPURI)
	{
	CSIPURI* self = CSIPURI::NewLC(aSIPURI);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPURI::NewLC
// -----------------------------------------------------------------------------
//
CSIPURI* CSIPURI::NewLC (const CSIPURI& aSIPURI)
	{
	CSIPURI* self = new(ELeave)CSIPURI(aSIPURI.IsSIPSURI());
	CleanupStack::PushL(self);
	self->ConstructL(aSIPURI);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPURI::NewL
// -----------------------------------------------------------------------------
//
CSIPURI* CSIPURI::NewL ()
	{
	CSIPURI* self = CSIPURI::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPURI::NewLC
// -----------------------------------------------------------------------------
//
CSIPURI* CSIPURI::NewLC ()
	{
	CSIPURI* self = new(ELeave)CSIPURI;
	CleanupStack::PushL(self);
	self->ConstructL ();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPURI::CSIPURI
// -----------------------------------------------------------------------------
//
CSIPURI::CSIPURI(TBool aIsSIPSUri)
: iIsSIPSURI(aIsSIPSUri)
	{
	}

// -----------------------------------------------------------------------------
// CSIPURI::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPURI::ConstructL ()
	{
	iURIParams = new(ELeave)CSIPURIParams(iIsSIPSURI);
	iURIHeaders = new(ELeave)CSIPURIHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPURI::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPURI::ConstructL (const CSIPURI& aSIPURI)
	{
	ConstructL();
	iHostPort = CSIPHostPort::NewL(*aSIPURI.iHostPort);
	if (aSIPURI.User().Compare(KNullDesC8) != 0) 
		{
		SetUserL(aSIPURI.User());
		}
	if (aSIPURI.Password().Compare(KNullDesC8) != 0) 
		{
		SetPasswordL(aSIPURI.Password());
		}
	SetURIParams(CSIPURIParams::NewL(*aSIPURI.iURIParams,aSIPURI.IsSIPSURI()));
	SetURIHeaders(CSIPURIHeaders::NewL(*aSIPURI.iURIHeaders));
	}

// -----------------------------------------------------------------------------
// CSIPURI::~CSIPURI
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPURI::~CSIPURI ()
	{
	delete iHostPort;
	delete iURIHeaders;
	delete iURIParams;
	delete iPassword;
	delete iUser;
	}

// -----------------------------------------------------------------------------
// CSIPURI::IsSIPSURI
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPURI::IsSIPSURI () const
	{
	return iIsSIPSURI;
	}

// -----------------------------------------------------------------------------
// CSIPURI::SetSIPS
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPURI::SetSIPS(TBool aSIPS)
    {
    iIsSIPSURI = aSIPS;
    }

// -----------------------------------------------------------------------------
// CSIPURI::SetHostPortL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPURI::SetHostPortL (CSIPHostPort* aHostPort)
 	{
	__ASSERT_ALWAYS (aHostPort != 0, User::Leave(KErrSipCodecHost));

	delete iHostPort;
	iHostPort = aHostPort;
	}

// -----------------------------------------------------------------------------
// CSIPURI::HostPort
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPHostPort& CSIPURI::HostPort () const
	{
	return *iHostPort;
	}

// -----------------------------------------------------------------------------
// CSIPURI::HostPort
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHostPort& CSIPURI::HostPort ()
	{
	return *iHostPort;
	}

// -----------------------------------------------------------------------------
// CSIPURI::User
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPURI::User () const
	{
	if (iUser) 
        {
        return *iUser;
        }
	return KNullDesC8;
	}

// -----------------------------------------------------------------------------
// CSIPURI::SetUserL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPURI::SetUserL (const TDesC8& aUser)
	{
	__ASSERT_ALWAYS (SIPSyntaxCheck::User(aUser),
	                 User::Leave(KErrSipCodecURI));

	HBufC8* tmp = aUser.AllocL();
	delete iUser;
	iUser = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPURI::DeleteUserInfo
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPURI::DeleteUserInfo ()
	{
	delete iUser; iUser = 0;
	DeletePassword();
	}

// -----------------------------------------------------------------------------
// CSIPURI::Password
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPURI::Password () const
	{
	if (iPassword) 
        {
        return *iPassword;
        }
	return KNullDesC8;
	}

// -----------------------------------------------------------------------------
// CSIPURI::SetPasswordL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPURI::SetPasswordL (const TDesC8& aPassword)
	{
	__ASSERT_ALWAYS (SIPSyntaxCheck::Password(aPassword),
	                 User::Leave(KErrSipCodecURI));
	__ASSERT_ALWAYS (iUser, User::Leave(KErrArgument));

	HBufC8* tmp = aPassword.AllocL();
	delete iPassword;
	iPassword = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPURI::DeletePassword
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPURI::DeletePassword ()
	{ 
	delete iPassword; iPassword = 0;
	}

// -----------------------------------------------------------------------------
// CSIPURI::HasParams
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPURI::HasParams () const
	{
	return (iURIParams->ParamCount() > 0);
	}

// -----------------------------------------------------------------------------
// CSIPURI::TtlParam
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPURI::TtlParam () const
	{
	RStringF ttl = SIPStrings::StringF(SipStrConsts::ETtl);	
	return iURIParams->IntParamValue(ttl);
	}

// -----------------------------------------------------------------------------
// CSIPURI::SetTtlParamL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPURI::SetTtlParamL (TInt aValue)
	{
	RStringF ttl = SIPStrings::StringF(SipStrConsts::ETtl);
	iURIParams->SetParamL(ttl,aValue);
	}

// -----------------------------------------------------------------------------
// CSIPURI::HasParam
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPURI::HasParam(RStringF aName) const
	{
	return iURIParams->HasParam(aName);
	}

// -----------------------------------------------------------------------------
// CSIPURI::ParamValue
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPURI::ParamValue(RStringF aName) const
	{
	return iURIParams->ParamValue(aName);
	}

// -----------------------------------------------------------------------------
// CSIPURI::SetParamL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPURI::SetParamL(RStringF aName)
	{
	iURIParams->SetParamL(aName);
	}

// -----------------------------------------------------------------------------
// CSIPURI::SetParamL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPURI::SetParamL(RStringF aName, RStringF aValue)
	{
	iURIParams->SetParamL(aName,aValue);
	}

// -----------------------------------------------------------------------------
// CSIPURI::DeleteParam
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPURI::DeleteParam(RStringF aName)
	{
	iURIParams->DeleteParam (aName);
	}

// -----------------------------------------------------------------------------
// CSIPURI::HasURIHeaders
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPURI::HasURIHeaders (const TDesC8& aName) const
	{
	return iURIHeaders->HasHeaders(aName);
	}

// -----------------------------------------------------------------------------
// CSIPURI::URIHeaderValuesL
// -----------------------------------------------------------------------------
//
EXPORT_C CPtrC8Array* CSIPURI::URIHeaderValuesL (const TDesC8& aName) const
	{
	return iURIHeaders->HeaderValuesL(aName);
	}

// -----------------------------------------------------------------------------
// CSIPURI::AddURIHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPURI::AddURIHeaderL (const TDesC8& aName,
									  const TDesC8& aValue)
	{
	iURIHeaders->AddHeaderL(aName,aValue);
	}

// -----------------------------------------------------------------------------
// CSIPURI::DeleteAllURIHeaders
// -----------------------------------------------------------------------------
//
void CSIPURI::DeleteAllURIHeaders ()
	{
	iURIHeaders->DeleteAll();
	}

// -----------------------------------------------------------------------------
// CSIPURI::operator==
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPURI::operator==(const CSIPURI& aURI) const
    {
	if (aURI.iIsSIPSURI != iIsSIPSURI)
		{
		return EFalse;
		}
    TBool hasuserparam((aURI.User().Compare(KNullDesC8) != 0));
    TBool hasuser((User().Compare(KNullDesC8) != 0));
	if (hasuserparam != hasuser)
		{
		return EFalse;
		}
	if (hasuserparam && aURI.User() != User())
		{
		return EFalse;
		}
    TBool haspasswordparam((aURI.Password().Compare(KNullDesC8) != 0));
    TBool haspassword((Password().Compare(KNullDesC8) != 0));
	if (haspasswordparam != haspassword)
		{
		return EFalse;
		}
	if (haspasswordparam && 
        aURI.Password() != Password())
		{
		return EFalse;
		}
	if (!(*iHostPort == *(aURI.iHostPort))) 
		{
		return EFalse;
		}
	if (!(*aURI.iURIParams == *iURIParams))
		{
		return EFalse;
		}
	return (*aURI.iURIHeaders == *iURIHeaders);
	}

// -----------------------------------------------------------------------------
// CSIPURI::ToTextL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPURI::ToTextL () const
	{
	TPtrC8 scheme(KSIP);
	if (iIsSIPSURI)
		{
		scheme.Set(KSIPS);
		}
	TUint encodedLength = scheme.Length() + KColon().Length();
	
	if (iUser)
		{
		encodedLength += iUser->Length();
		if (iPassword) encodedLength+=KColon().Length()+iPassword->Length();
		encodedLength += KAt().Length();
		}

	HBufC8* hostPort = iHostPort->ToTextLC();
	encodedLength += hostPort->Length();

	HBufC8* uriParams = iURIParams->ToTextLC();
	if (uriParams->Length() > 0) encodedLength += (1+uriParams->Length());

	HBufC8* uriHeaders = iURIHeaders->ToTextLC();
	if (uriHeaders->Length() > 0) encodedLength += (1+uriHeaders->Length());

	// Create and fill the encoded SIP URI
	HBufC8* encodedSIPURI = HBufC8::NewL (encodedLength);
	TPtr8 encodedSIPURIPtr = encodedSIPURI->Des();

	encodedSIPURIPtr.Append(scheme);
	encodedSIPURIPtr.Append(KColon);
	
	if (iUser)
		{
		encodedSIPURIPtr.Append(*iUser);
		if (iPassword)
			{
			encodedSIPURIPtr.Append(KColon); 
			encodedSIPURIPtr.Append(*iPassword);
			}
		encodedSIPURIPtr.Append(KAt);
		}

	encodedSIPURIPtr.Append(*hostPort);

	if (uriParams->Length() > 0)
		{
		encodedSIPURIPtr.Append(KSemiColon);
		encodedSIPURIPtr.Append (*uriParams);
		}
	
	if (uriHeaders->Length() > 0)
		{
		encodedSIPURIPtr.Append(KQuestionMark);
		encodedSIPURIPtr.Append (*uriHeaders);
		}

	CleanupStack::PopAndDestroy(uriHeaders); 
	CleanupStack::PopAndDestroy(uriParams); 
	CleanupStack::PopAndDestroy(hostPort);  
	return encodedSIPURI;
	}

// -----------------------------------------------------------------------------
// CSIPURI::ParseSipSchemeL
// -----------------------------------------------------------------------------
//
TPtrC8 CSIPURI::ParseSipSchemeL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, User::Leave(KErrSipCodecURIScheme));

	_LIT8(KSIPAndColon, "sip:");
	_LIT8(KSIPSAndColon, "sips:");

	TPtrC8 schemeAndColon;
	TInt sipSchemePos = aValue.FindF(KSIPAndColon);
	if (sipSchemePos < 0)
		{
		TInt sipsSchemePos = aValue.FindF(KSIPSAndColon);
		if (sipsSchemePos != 0) 
            {
            User::Leave (KErrSipCodecURIScheme);
            }
		schemeAndColon.Set(KSIPSAndColon);
		iIsSIPSURI = ETrue;
		}
	else
		{
		if (sipSchemePos != 0) 
            {
            User::Leave (KErrSipCodecURIScheme);
            }
		schemeAndColon.Set(KSIPAndColon);
		iIsSIPSURI = EFalse;
		}
	TPtrC8 remainder(aValue.Mid(schemeAndColon.Length()));
	return remainder;
	}

// -----------------------------------------------------------------------------
// CSIPURI::ParseL
// -----------------------------------------------------------------------------
//
void CSIPURI::ParseL (const TDesC8& aValue)
	{
	TPtrC8 remainder(aValue);
	TInt atPos = 0;
	LocateSeparatorL(aValue, '@', atPos);
	if (atPos > 0) // has userinfo
		{
		ParseUserInfoL(aValue.Left(atPos));
		remainder.Set(aValue.Mid(atPos+1));
		}

	TInt semiColonPos = 0;
	LocateSeparatorL(remainder, ';', semiColonPos);
	if (semiColonPos > 0) // has uri-parameters
		{
		iHostPort = CSIPHostPort::DecodeL (remainder.Left(semiColonPos));
		remainder.Set(remainder.Mid(semiColonPos+1));
		TInt questionMarkPos = 0;
		LocateSeparatorL(remainder, '?', questionMarkPos);
		TPtrC8 uriParams;
		if (questionMarkPos > 0) // has uri-headers
			{
			uriParams.Set(remainder.Left(questionMarkPos));
			TPtrC8 uriHeaders(remainder.Mid(questionMarkPos+1));
			SetURIHeaders (CSIPURIHeaders::DecodeL(uriHeaders));
			}
		else // does not have uri-headers
			{
			uriParams.Set(remainder);
			}
		SetURIParams(CSIPURIParams::DecodeL(uriParams,iIsSIPSURI));
		}
	else // does not have uri-parameters
		{
		TInt questionMarkPos = 0;
		LocateSeparatorL(remainder, '?', questionMarkPos);
		if (questionMarkPos > 0) // has uri-headers
			{
			iHostPort = CSIPHostPort::DecodeL (remainder.Left(questionMarkPos));
			TPtrC8 uriHeaders(remainder.Mid(questionMarkPos+1));
			SetURIHeaders (CSIPURIHeaders::DecodeL(uriHeaders));
			}
		else
			{
			iHostPort = CSIPHostPort::DecodeL (remainder);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPURI::ParseUserInfoL
// -----------------------------------------------------------------------------
//
void CSIPURI::ParseUserInfoL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, User::Leave(KErrSipCodecURI));

	TInt colonPos = 0;
	LocateSeparatorL(aValue, ':', colonPos);
	if (colonPos > 0) // has both user and password part
		{
		SetUserL (aValue.Left(colonPos));
		SetPasswordL (aValue.Mid(colonPos+1));
		}
	else // has only user part
		{	 
		SetUserL (aValue);
		}
	}

// -----------------------------------------------------------------------------
// CSIPURI::LocateSeparatorL
// -----------------------------------------------------------------------------
//
void CSIPURI::LocateSeparatorL(
    const TDesC8& aValue,
    TChar aSeparator,
    TInt& aPosition) const
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, User::Leave (KErrSipCodecURI));

	TInt position = aValue.Locate(aSeparator);
	if (position == 0 || position == aValue.Length()-1)
		{
		User::Leave (KErrSipCodecURI);
		}
    aPosition = position;
	}

// -----------------------------------------------------------------------------
// CSIPURI::SetURIParams
// -----------------------------------------------------------------------------
//
void CSIPURI::SetURIParams (CSIPURIParams* aURIParams)
	{
	delete iURIParams;
	iURIParams = aURIParams;
	}

// -----------------------------------------------------------------------------
// CSIPURI::SetURIHeaders
// -----------------------------------------------------------------------------
//
void CSIPURI::SetURIHeaders (CSIPURIHeaders* aURIHeaders)
	{
	delete iURIHeaders;
	iURIHeaders = aURIHeaders;
	}	
	
	
