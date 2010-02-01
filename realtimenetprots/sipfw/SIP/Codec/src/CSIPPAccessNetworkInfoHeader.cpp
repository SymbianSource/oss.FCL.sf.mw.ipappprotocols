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
// Name        : CSIPPAccessNetworkInfoHeader.cpp
// Part of     : SIP Codec
// Interface   : SDK API, SIP Codec API
// Version     : SIP/4.0 
//



#include "sippaccessnetworkinfoheader.h"
#include "CSIPPAccessNetworkInfoHeaderParams.h"
#include "SIPSyntaxCheck.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipcodecutils.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPPAccessNetworkInfoHeader* 
	CSIPPAccessNetworkInfoHeader::DecodeL(const TDesC8& aValue)
    {
    __ASSERT_ALWAYS(aValue.Length() > 0,
		User::Leave (KErrSipCodecPAccessNetworkInfoHeader));

    CSIPPAccessNetworkInfoHeader* header = 
        new(ELeave) CSIPPAccessNetworkInfoHeader;
    CleanupStack::PushL(header);
    header->ConstructL();
    header->ParseL(aValue);
    CleanupStack::Pop(header);
    return header;
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPPAccessNetworkInfoHeader* 
    CSIPPAccessNetworkInfoHeader::NewL(RStringF aAccessType)
    {
    CSIPPAccessNetworkInfoHeader* self = 
        CSIPPAccessNetworkInfoHeader::NewLC(aAccessType);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPPAccessNetworkInfoHeader* 
    CSIPPAccessNetworkInfoHeader::NewLC(RStringF aAccessType)
    {
    CSIPPAccessNetworkInfoHeader* self = 
        new (ELeave)CSIPPAccessNetworkInfoHeader;
    CleanupStack::PushL(self);
    self->ConstructL(aAccessType);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::~CSIPPAccessNetworkInfoHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPPAccessNetworkInfoHeader::~CSIPPAccessNetworkInfoHeader()
    {
    iAccessType.Close();
    delete iParams;
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::CSIPPAccessNetworkInfoHeader
// ----------------------------------------------------------------------------
//
CSIPPAccessNetworkInfoHeader::CSIPPAccessNetworkInfoHeader()
: CSIPParameterHeaderBase(';')
    {
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPPAccessNetworkInfoHeader::ConstructL()		
    {
    iParams = new (ELeave) CSIPPAccessNetworkInfoHeaderParams;
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPPAccessNetworkInfoHeader::ConstructL(RStringF aAccessType)
    {
    ConstructL();
    SetAccessTypeL(aAccessType);
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPPAccessNetworkInfoHeader::ConstructL(
                                const CSIPPAccessNetworkInfoHeader& aHeader)
    {
    SetAccessTypeL(aHeader.AccessType());
    iParams = CSIPPAccessNetworkInfoHeaderParams::NewL(*(aHeader.iParams));
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::SetAccessTypeL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPPAccessNetworkInfoHeader::SetAccessTypeL(RStringF 
                                                           aAccessType)
    {
    RStringF tmp = 
        SIPCodecUtils::CheckAndCreateTokenL(
            aAccessType.DesC(), KErrSipCodecPAccessNetworkInfoHeader);
	iAccessType.Close();
    iAccessType = tmp;    
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::AccessType
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPPAccessNetworkInfoHeader::AccessType() const
    {
    return iAccessType;
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::CloneL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPPAccessNetworkInfoHeader::CloneL() const
    {
    CSIPPAccessNetworkInfoHeader* header = 
        new(ELeave) CSIPPAccessNetworkInfoHeader;
    CleanupStack::PushL(header);
    header->ConstructL(*this);
    CleanupStack::Pop(header);
    return header;
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::Name
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPPAccessNetworkInfoHeader::Name() const
    {
    return SIPStrings::StringF(SipStrConsts::EPAccessNetworkInfoHeader);
    }
    
// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::ExternalizeSupported
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPPAccessNetworkInfoHeader::ExternalizeSupported () const
	{
	return EFalse;
	}    

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::PreferredPlaceInMessage
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace 
CSIPPAccessNetworkInfoHeader::PreferredPlaceInMessage() const
    {
    return CSIPHeaderBase::EMiddleBottom;
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPPAccessNetworkInfoHeader::BaseDecodeL(const TDesC8& aValue)
    {
	CSIPPAccessNetworkInfoHeader* header = DecodeL(aValue);
	CleanupStack::PushL(header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError (headers.Append(header));
	CleanupStack::Pop(header);
	return headers;	
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::ToTextMandatoryPartLC
// ----------------------------------------------------------------------------
//
HBufC8* CSIPPAccessNetworkInfoHeader::ToTextMandatoryPartLC() const
    {
    return iAccessType.DesC().AllocLC();
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::ParseMandatoryPartL
// ----------------------------------------------------------------------------
//
void CSIPPAccessNetworkInfoHeader::ParseMandatoryPartL(
                                                  const TDesC8& aMandatoryPart)
    {
    __ASSERT_ALWAYS(aMandatoryPart.Length() > 0,
		User::Leave (KErrSipCodecPAccessNetworkInfoHeader));

	TLex8 lex(aMandatoryPart);
	lex.SkipSpace();
    RStringF accessType = SIPStrings::Pool().OpenFStringL(lex.Remainder());
    CleanupClosePushL(accessType);
	SetAccessTypeL(accessType);
    CleanupStack::PopAndDestroy(&accessType);
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::Params
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPPAccessNetworkInfoHeader::Params() const
    {
    return *iParams;
    }

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeader::Params
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPPAccessNetworkInfoHeader::Params()
    {
    return *iParams;
    }
