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
// Name          : CSIPViaHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipviaheader.h"
#include "CSIPViaHeaderParams.h"
#include "CSIPTokenizer.h"
#include "siphostport.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPViaHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPViaHeader> 
CSIPViaHeader::DecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&baseheaders);
	RPointerArray<CSIPViaHeader> viaheaders;
	CleanupClosePushL(viaheaders);
	TInt count = baseheaders.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPViaHeader* via = static_cast<CSIPViaHeader*>(baseheaders[i]);
		viaheaders.AppendL(via);
		}
	CleanupStack::Pop(2); // viaheaders, baseheaders
	baseheaders.Close();
	return viaheaders;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPViaHeader* CSIPViaHeader::NewL (RStringF aTransport,
											 CSIPHostPort* aSentByHostPort)
	{
	CSIPViaHeader* self = CSIPViaHeader::NewLC (aTransport,aSentByHostPort);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPViaHeader* CSIPViaHeader::NewLC (RStringF aTransport,
											  CSIPHostPort* aSentByHostPort)
	{
	CSIPViaHeader* self = new(ELeave)CSIPViaHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aTransport,aSentByHostPort);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::CSIPViaHeader
// -----------------------------------------------------------------------------
//
CSIPViaHeader::CSIPViaHeader()
 : CSIPParameterHeaderBase(';')
	{
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPViaHeader::ConstructL()
	{
	iSentProtocolName = SIPStrings::StringF(SipStrConsts::ESIP);
	iSentProtocolVersion = 
		SIPStrings::StringF(SipStrConsts::EDefaultVersionNumber);
	iParams = new(ELeave)CSIPViaHeaderParams;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPViaHeader::ConstructL(RStringF aTransport,
							   CSIPHostPort* aSentByHostPort)
	{
	__ASSERT_ALWAYS(aSentByHostPort, User::Leave(KErrArgument));

	ConstructL();
	SetTransportL(aTransport);
	iSentByHostPort = aSentByHostPort;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPViaHeader::ConstructL (const CSIPViaHeader& aViaHeader)
	{
	iSentProtocolName = aViaHeader.SentProtocolName().Copy();
	iSentProtocolVersion = aViaHeader.SentProtocolVersion().Copy();
	iTransport = aViaHeader.Transport().Copy();	
	iParams = CSIPViaHeaderParams::NewL(*(aViaHeader.iParams));
	iSentByHostPort = CSIPHostPort::NewL(*(aViaHeader.iSentByHostPort));
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::~CSIPViaHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPViaHeader::~CSIPViaHeader()
	{
	delete iSentByHostPort;
	delete iParams;
	iTransport.Close();	
	iSentProtocolVersion.Close();
	iSentProtocolName.Close();
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPViaHeader::CloneL () const
	{
	CSIPViaHeader* clone = new(ELeave)CSIPViaHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);
	return clone;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::operator==
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPViaHeader::operator==(const CSIPViaHeader& aViaHeader)
	{
	if (iTransport != aViaHeader.Transport())
		{
		return EFalse;
		}
	if (iSentProtocolName != aViaHeader.iSentProtocolName)
		{
		return EFalse;
		}
	if (iSentProtocolVersion != aViaHeader.iSentProtocolVersion)
		{
		return EFalse;
		}
	if (!(*iSentByHostPort == *aViaHeader.iSentByHostPort))
		{
		return EFalse;
		}
	return (*iParams == *(aViaHeader.iParams));
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::Transport
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPViaHeader::Transport () const
	{
	return iTransport;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::SetTransportL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPViaHeader::SetTransportL(RStringF aTransport)
	{
	SetTransportL(aTransport.DesC());	
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::SentByHostPort
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHostPort& CSIPViaHeader::SentByHostPort()
	{
	return *iSentByHostPort;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::SentProtocolName
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPViaHeader::SentProtocolName () const
	{
	return iSentProtocolName;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::SentProtocolVersion
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPViaHeader::SentProtocolVersion () const
	{
	return iSentProtocolVersion;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::SetSentProtocolVersionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPViaHeader::SetSentProtocolVersionL(RStringF aVersion)
	{
	SetSentProtocolVersionL(aVersion.DesC());
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::ViaTtl
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPViaHeader::TtlParam() const
	{
	return iParams->IntParamValue(SIPStrings::StringF(SipStrConsts::ETtl));
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::SetViaTtlL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPViaHeader::SetTtlParamL (TInt aValue)
	{
	iParams->SetParamL(SIPStrings::StringF(SipStrConsts::ETtl), aValue);
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::MoreThanOneAllowed
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPViaHeader::MoreThanOneAllowed () const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPViaHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EViaHeader);
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::HasCompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPViaHeader::HasCompactName () const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::CompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
RStringF CSIPViaHeader::CompactName () const
	{
	return SIPStrings::StringF(SipStrConsts::EViaHeaderCompact);
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::ExternalizeSupported
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPViaHeader::ExternalizeSupported () const
	{
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// CSIPViaHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPViaHeader::BaseDecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers;
	CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPViaHeader* header = new(ELeave)CSIPViaHeader;
        CleanupStack::PushL(header);
        header->ConstructL();
		header->ParseL(tokenizer->Tokens()[i]);
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace CSIPViaHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::ETop;
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeader::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
HBufC8* CSIPViaHeader::ToTextMandatoryPartLC () const
    {
    TPtrC8 sentProtocolName(iSentProtocolName.DesC());
	TUint encodedLength = sentProtocolName.Length() + 1; // SLASH
	
	TPtrC8 sentProtocolVersion(iSentProtocolVersion.DesC());
	encodedLength += sentProtocolVersion.Length() + 1; // SLASH
	
	TPtrC8 transport(iTransport.DesC());
	encodedLength += transport.Length() + 1; // SP

	HBufC8* encodedSentByHostPort = iSentByHostPort->ToTextLC();
	encodedLength += encodedSentByHostPort->Length(); 

	HBufC8* encodingResult = HBufC8::NewL (encodedLength);
	TPtr8 encodingResultPtr = encodingResult->Des();
	
	encodingResultPtr.Append(sentProtocolName);
	encodingResultPtr.Append('/'); // SLASH
	encodingResultPtr.Append(sentProtocolVersion);
	encodingResultPtr.Append('/'); // SLASH
	encodingResultPtr.Append(transport);
	encodingResultPtr.Append(' '); // SP
	encodingResultPtr.Append(*encodedSentByHostPort);

	CleanupStack::PopAndDestroy(encodedSentByHostPort);
    CleanupStack::PushL (encodingResult);
	return encodingResult;
    }

// -----------------------------------------------------------------------------
// CSIPViaHeader::ParseMandatoryPartL
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPViaHeader::ParseMandatoryPartL (const TDesC8& aMandatoryPart)
    {
	__ASSERT_ALWAYS (aMandatoryPart.Length()>0,User::Leave(KErrSipCodecViaHeader));

	// sent-protocol
	TInt endPos = 0;
	ParseSentProtocolL(aMandatoryPart,endPos);
	if (endPos >= aMandatoryPart.Length()-1)
        {
        User::Leave (KErrSipCodecViaHeader);
        }
	TLex8 lex(aMandatoryPart.Mid(endPos+1));
	lex.SkipSpace();
	// sent-by
    iSentByHostPort = CSIPHostPort::DecodeL (lex.Remainder());
    }
    
// -----------------------------------------------------------------------------
// CSIPViaHeader::Params
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPViaHeader::Params () const
    {
    return *iParams; 
    }

// -----------------------------------------------------------------------------
// CSIPViaHeader::Params
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPViaHeader::Params () 
    {
    return *iParams; 
    }

// -----------------------------------------------------------------------------
// CSIPViaHeader::ParseSentProtocolL
// -----------------------------------------------------------------------------
//
void CSIPViaHeader::ParseSentProtocolL (
    const TDesC8& aValue,
    TInt& aLength)
	{
	__ASSERT_ALWAYS (aValue.Length()>0, User::Leave(KErrSipCodecViaHeader));

	TInt length = 0;
	TInt slashPos = aValue.Locate ('/');
	if (slashPos <= 0 || slashPos == aValue.Length()-1)
		{
		User::Leave(KErrSipCodecViaHeader);
		}
	SetSentProtocolNameL(aValue.Left(slashPos));
	length += (slashPos+1);

	TPtrC8 remainder (aValue.Mid(slashPos+1));
	slashPos = remainder.Locate ('/');
	if (slashPos <= 0 || slashPos == remainder.Length()-1)
		{
		User::Leave (KErrSipCodecViaHeader);
		}
	SetSentProtocolVersionL(remainder.Left(slashPos));
	length += (slashPos+1);

	TLex8 lex(remainder.Mid(slashPos+1));
	lex.Mark();
	SetTransportL (lex.NextToken());
	length += lex.Offset();

	aLength = length;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::SetTransportL
// -----------------------------------------------------------------------------
//
void CSIPViaHeader::SetTransportL(const TDesC8& aTransport)
	{
	HBufC8* tmp = aTransport.AllocLC();
	tmp->Des().Trim();
	tmp->Des().UpperCase();
	if (!SIPSyntaxCheck::Token(*tmp))
		{
		User::Leave(KErrSipCodecViaHeader);
		}
	RStringF tmpString = SIPStrings::Pool().OpenFStringL(*tmp);
	CleanupStack::PopAndDestroy(tmp);		
	iTransport.Close();
	iTransport = tmpString;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::SetSentProtocolVersionL
// -----------------------------------------------------------------------------
//
void CSIPViaHeader::SetSentProtocolVersionL(const TDesC8& aVersion)
	{	
    RStringF tmp = 
        SIPCodecUtils::CheckAndCreateTokenL(aVersion, KErrSipCodecViaHeader);
	iSentProtocolVersion.Close();
    iSentProtocolVersion = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeader::SetSentProtocolNameL
// -----------------------------------------------------------------------------
//
void CSIPViaHeader::SetSentProtocolNameL (const TDesC8& aName)
	{
    RStringF tmp = 
        SIPCodecUtils::CheckAndCreateTokenL(aName, KErrSipCodecViaHeader);
	iSentProtocolName.Close();
	iSentProtocolName = tmp;        
	}
