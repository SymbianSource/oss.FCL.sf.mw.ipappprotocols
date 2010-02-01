// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipmessagebuilder.cpp
// Part of     : sip profile fsm
// implementation
// Version     : 1.0
//




// INCLUDE FILES
#include 	<uri8.h>
#include	"sipmessagebuilder.h"
#include	"sipcontactheader.h"
#include	"sipaddress.h"
#include	"siptoheader.h"
#include	"siprouteheader.h"
#include	"sipextensionheader.h"
#include	"sipgendefs.h"
#include    "sipstrings.h"
#include    "sipstrconsts.h"
#include    "sipcodecerr.h"

_LIT8(KUriParamTemplate, ";%S=%S");
_LIT8(KCompSigCompParam, "comp=sigcomp");

// -----------------------------------------------------------------------------
// SIPMessageBuilder::CreateToLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPToHeader* SIPMessageBuilder::CreateToLC(const TDesC8& aUri)
	{
	TUriParser8 parser;
	User::LeaveIfError( parser.Parse( aUri ) );
	
	CUri8* tmpUri = CUri8::NewLC( parser );
	
	CSIPAddress* tmpAddr = CSIPAddress::NewL( tmpUri );

	CleanupStack::Pop( tmpUri );
	CleanupStack::PushL( tmpAddr );
	
	CSIPToHeader* to = CSIPToHeader::NewL( tmpAddr );
	CleanupStack::Pop( tmpAddr );
	
	CleanupStack::PushL( to );
	return to;
	}

// -----------------------------------------------------------------------------
// SIPMessageBuilder::CreateContactLC
// adds contact header
// REGISTER request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPContactHeader* SIPMessageBuilder::CreateContactLC(
	const TDesC8& aUser,
	TInt aExpiresValue,
	const MDesC8Array& aParams,
	RStringF aTransport,
	TBool aSigComp)
	{
    RStringF comp = SIPStrings::StringF(SipStrConsts::EComp);
    RStringF sigcomp = SIPStrings::StringF(SipStrConsts::ESigComp);
    RStringF transport = SIPStrings::StringF(SipStrConsts::ETransport);

    TUriParser8 uriParser;
    User::LeaveIfError(uriParser.Parse(KSIPlocalhost));
    CUri8* uri = CUri8::NewLC(uriParser);

	if (aUser.Length() > 0)
	    {
	    uri->SetComponentL(aUser, EUriUserinfo);
	    }
	
	if (aSigComp)
		{
		AddUriParamL(*uri,comp.DesC(),sigcomp.DesC());
		}
	
	if (aTransport.DesC().Length() > 0)
		{
		AddUriParamL(*uri,transport.DesC(),aTransport.DesC());
		}
	
	CSIPAddress* sipAddress = CSIPAddress::NewL(uri);
	CleanupStack::Pop(uri);
	CleanupStack::PushL(sipAddress);
	CSIPContactHeader* contact = CSIPContactHeader::NewL(sipAddress);
	CleanupStack::Pop(sipAddress);
	CleanupStack::PushL(contact);
    contact->SetExpiresParameterL(aExpiresValue);
	
	for (TInt i=0; i<aParams.MdcaCount(); i++)
		{
		TChar equal = '=';
		TPtrC8 param(aParams.MdcaPoint(i));
		TInt index = param.Locate(equal);
		TInt locationAfterEquals(index+1);
		if (index != KErrNotFound && param.Length() > locationAfterEquals)
			{
			RStringF key = SIPStrings::Pool().OpenFStringL(param.Left(index));
			CleanupClosePushL(key);
			RStringF value = 
			    SIPStrings::Pool().OpenFStringL(param.Mid(locationAfterEquals));
			CleanupClosePushL(value);
			contact->SetParamL(key, value);
			CleanupStack::PopAndDestroy(2); //key, value
			}
		else
			{
			RStringF key = SIPStrings::Pool().OpenFStringL(param);
			CleanupClosePushL(key);
			contact->SetParamL(key);
			CleanupStack::PopAndDestroy(); //key	
			}
		}

	return contact;
	}

// -----------------------------------------------------------------------------
// SIPMessageBuilder::CreateAuthorizationLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C CSIPExtensionHeader* SIPMessageBuilder::CreateAuthorizationLC(
	const TDesC8& aUri,
	const TDesC8& aRealm, 
	const TDesC8& aPrivateId)
	{
	HBufC8* authorizationValue = HBufC8::NewLC(aPrivateId.Length()
		+ aUri.Length() + aRealm.Length() + (5*KSIPequals().Length())
		+ (KSIPquotneeded) + (4*KSIPcomma().Length()) + KSIPUsername().Length() 
		+ KSIPauthForHeader().Length() + KSIPrealm().Length() + KSIPuri().Length() 
		+ KSIPnonce().Length() + KSIPresponse().Length());

	TPtr8 ptr = authorizationValue->Des();
	ptr.Append(KSIPauthForHeader());
	ptr.Append(KSIPUsername());
	ptr.Append(KSIPequals());
	ptr.Append(KSIPquot);
	ptr.Append(aPrivateId);
	ptr.Append(KSIPquot);
	ptr.Append(KSIPcomma());
	ptr.Append(KSIPrealm());
	ptr.Append(KSIPequals());
	ptr.Append(KSIPquot);
	ptr.Append(aRealm);
	ptr.Append(KSIPquot);
	ptr.Append(KSIPcomma());
	ptr.Append(KSIPnonce());
	ptr.Append(KSIPequals());
	ptr.Append(KSIPquot);
	ptr.Append(KSIPquot);
	ptr.Append(KSIPcomma());
	ptr.Append(KSIPuri());
	ptr.Append(KSIPequals());
	ptr.Append(KSIPquot);
	ptr.Append(aUri);
	ptr.Append(KSIPquot);
	ptr.Append(KSIPcomma());
	ptr.Append(KSIPresponse());
	ptr.Append(KSIPequals());
	ptr.Append(KSIPquot);
	ptr.Append(KSIPquot);

	CSIPExtensionHeader* authorizationHeader = CSIPExtensionHeader::NewL(
		KSIPauthheader(), *authorizationValue);
	CleanupStack::PopAndDestroy(authorizationValue);
	CleanupStack::PushL(authorizationHeader);
	return authorizationHeader;
	}

// -----------------------------------------------------------------------------
// SIPMessageBuilder::CreateRouteLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRouteHeader* SIPMessageBuilder::CreateRouteLC(
    const TDesC8& aRoute,
    TBool aSigComp)
	{
    RStringF comp = SIPStrings::StringF(SipStrConsts::EComp);
    RStringF sigcomp = SIPStrings::StringF(SipStrConsts::ESigComp);
    
    TUriParser8 uriParser;
    User::LeaveIfError(uriParser.Parse(aRoute));
    CUri8* uri = CUri8::NewLC(uriParser);
	if (aSigComp && uri->Uri().Extract(EUriPath).FindF(KCompSigCompParam) < 0)
		{    
        AddUriParamL(*uri, comp.DesC(), sigcomp.DesC());
		}
    CSIPAddress* sipAddress = CSIPAddress::NewL(uri);
    CleanupStack::Pop(uri);
    CleanupStack::PushL(sipAddress);
	CSIPRouteHeader* route = CSIPRouteHeader::NewL(sipAddress);
	CleanupStack::Pop(sipAddress);
	CleanupStack::PushL(route);
	return route;
	}
	
// -----------------------------------------------------------------------------
// SIPMessageBuilder::CreateHeaderLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPHeaderBase* SIPMessageBuilder::CreateHeaderLC(
    const TDesC8& aHeaderDes)
    {
	TInt colonPos = aHeaderDes.Locate(':');
	__ASSERT_ALWAYS(colonPos > 0, User::Leave(KErrSipCodecHeaderName));
	TPtrC8 name(aHeaderDes.Left(colonPos));
	TPtrC8 value(aHeaderDes.Mid(colonPos+1));
	TLex8 lex(value);
	lex.SkipSpace();
	TPtrC8 trimmedValue(lex.Remainder());
	CSIPExtensionHeader* header = CSIPExtensionHeader::NewLC(name,trimmedValue);
	return header;
    }
    
// -----------------------------------------------------------------------------
// SIPMessageBuilder::ParseParamL
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool SIPMessageBuilder::ParseParamL(
    const TDesC8& aParam,
    TPtrC8& aName,
    TPtrC8& aValue)
    {
	__ASSERT_ALWAYS (aParam.Length() > 0, User::Leave(KErrSipCodecAnyParam));
	TInt equalsPos = aParam.Locate('=');
	if (equalsPos == 0 || equalsPos == aParam.Length()-1) 
		{
		User::Leave (KErrSipCodecAnyParam);
		}
	TBool hasValue = EFalse;	
	if (equalsPos > 0)
		{
		aName.Set(aParam.Left(equalsPos));
		aValue.Set(aParam.Mid(equalsPos+1));
		hasValue = ETrue;
		}
	else
        {
	    aName.Set(aParam);
	    aValue.Set(KNullDesC8);   
        }
	return hasValue;    
    }	

// -----------------------------------------------------------------------------
// SIPMessageBuilder::AddUriParamL
// -----------------------------------------------------------------------------
//
void SIPMessageBuilder::AddUriParamL(
    CUri8& aUri, 
    const TDesC8& aName,
    const TDesC8& aValue)
    {
    TPtrC8 currentParamsPtr(aUri.Uri().Extract(EUriPath));
    TInt newParamsLength = currentParamsPtr.Length() + 
                           KUriParamTemplate().Length() + 
                           aName.Length() + 
                           aValue.Length();
    HBufC8* newParams = HBufC8::NewLC(newParamsLength);
    TPtr8 newParamsDes(newParams->Des());
    newParamsDes.Copy(currentParamsPtr);
    newParamsDes.AppendFormat(KUriParamTemplate,&aName,&aValue);
    aUri.SetComponentL(newParamsDes,EUriPath);
    CleanupStack::PopAndDestroy(newParams);
    }
