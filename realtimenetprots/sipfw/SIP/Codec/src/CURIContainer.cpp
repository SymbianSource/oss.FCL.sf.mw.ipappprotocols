// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CURIContainer.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "uricontainer.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipuri.h"
#include "sipcodecerr.h"
#include "SIPHeaderLookup.h"
#include "_sipcodecdefs.h"

// ----------------------------------------------------------------------------
// CURIContainer::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CURIContainer* CURIContainer::DecodeL(const TDesC8& aValue)
	{
    HBufC8* trimmedValue = aValue.AllocLC();
    trimmedValue->Des().Trim();

    __ASSERT_ALWAYS(trimmedValue->Length() > 0, User::Leave(KErrSipCodecURI));

    CURIContainer* container = new(ELeave) CURIContainer;
    CleanupStack::PushL(container);
    container->ConstructL();
    if(SIPHeaderLookup::ConvertToSIPURI())
        {
        if(FindSIP(*trimmedValue))
            {
            CSIPURI* sipuri = CSIPURI::DecodeL(*trimmedValue);
            CleanupStack::PushL(sipuri);
            container->iSIPURI = sipuri;
            CleanupStack::Pop(sipuri);
            }
        else
            {
            CUri8* uri8 = CreateUri8L(*trimmedValue);
            CleanupStack::PushL(uri8);
            container->iUri8 = uri8;
            CleanupStack::Pop(uri8);
            }
        }
    else
        {
        CUri8* uri8 = CreateUri8L(*trimmedValue);
        CleanupStack::PushL(uri8);
        container->iUri8 = uri8;
        CleanupStack::Pop(uri8);
        }
    CleanupStack::Pop(container);
    
    CleanupStack::PopAndDestroy(trimmedValue);
    
    return container;
	}

// ----------------------------------------------------------------------------
// CURIContainer::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CURIContainer* CURIContainer::NewL(CUri8* aUri8)
    {
    CURIContainer* self = CURIContainer::NewLC(aUri8);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CURIContainer::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CURIContainer* CURIContainer::NewLC(CUri8* aUri8)
    {
    CURIContainer* self = new(ELeave) CURIContainer;
    CleanupStack::PushL(self);
    self->ConstructL(aUri8);
    return self;
    }

// ----------------------------------------------------------------------------
// CURIContainer::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CURIContainer* CURIContainer::NewL(const CURIContainer& aURI)
    {
	CURIContainer* self = CURIContainer::NewLC(aURI);
	CleanupStack::Pop(self);
	return self;
    }

// ----------------------------------------------------------------------------
// CURIContainer::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CURIContainer* CURIContainer::NewLC(const CURIContainer& aURI)
    {
	CURIContainer* self = new (ELeave) CURIContainer;
	CleanupStack::PushL(self);
	self->ConstructL(aURI);
	return self;
    }

// ----------------------------------------------------------------------------
// CURIContainer::~CURIContainer
// ----------------------------------------------------------------------------
//
EXPORT_C CURIContainer::~CURIContainer()
    {
    delete iSIPURI;
    delete iUri8;
    SIPHeaderLookup::Close();
    }

// ----------------------------------------------------------------------------
// CURIContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CURIContainer::ConstructL()
    {
    SIPHeaderLookup::OpenL();
    }

// ----------------------------------------------------------------------------
// CURIContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CURIContainer::ConstructL(CUri8* aUri8)
    {
    SIPHeaderLookup::OpenL();
    if(SIPHeaderLookup::ConvertToSIPURI())
        {
        if(FindSIP(aUri8->Uri().UriDes()))
            {
            iSIPURI = CSIPURI::DecodeL(aUri8->Uri().UriDes());
            delete aUri8;
            }
        else
            {
            iUri8 = aUri8;
            }
        }
    else
        {
        iUri8 = aUri8;
        }
    }

// ----------------------------------------------------------------------------
// CURIContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CURIContainer::ConstructL(const CURIContainer& aURI)
    {
    SIPHeaderLookup::OpenL();
    if(aURI.IsSIPURI())
        {
        HBufC8* sipuriastext = aURI.iSIPURI->ToTextL();
        CleanupStack::PushL(sipuriastext);
        iSIPURI = CSIPURI::DecodeL(*sipuriastext);
        CleanupStack::PopAndDestroy(sipuriastext);
        }
    else
        {
        iUri8 = CUri8::NewL(aURI.Uri8()->Uri());
        }
    }

// ----------------------------------------------------------------------------
// CURIContainer::IsSIPURI
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CURIContainer::IsSIPURI() const
    {
    if(iSIPURI)
        {
        return ETrue;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CURIContainer::SIPURI
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPURI* CURIContainer::SIPURI()
    {
    return iSIPURI;
    }

// ----------------------------------------------------------------------------
// CURIContainer::SIPURI
// ----------------------------------------------------------------------------
//
EXPORT_C const CSIPURI* CURIContainer::SIPURI() const
    {
    return iSIPURI;
    }

// ----------------------------------------------------------------------------
// CURIContainer::Uri8
// ----------------------------------------------------------------------------
//
EXPORT_C const CUri8* CURIContainer::Uri8() const
    {
    return iUri8;
    }

// ----------------------------------------------------------------------------
// CURIContainer::operator==
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CURIContainer::operator==(const CURIContainer& aURI) const
    {
    if(aURI.IsSIPURI() && IsSIPURI())
        {
        return (*iSIPURI == *aURI.iSIPURI);
        }
    if(!aURI.IsSIPURI() && !IsSIPURI())
        {
        if(aURI.Uri8()->Uri().UriDes().Compare(Uri8()->Uri().UriDes()) == 0)
            {
            return ETrue;
            }
        return EFalse;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CURIContainer::ToTextL
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CURIContainer::ToTextL() const
    {
    __ASSERT_ALWAYS((iUri8 || iSIPURI), User::Leave(KErrSipCodecURIContainer));

	if(iSIPURI)
	    {
        return iSIPURI->ToTextL();
	    }
    return iUri8->Uri().UriDes().AllocL();
    }

// ----------------------------------------------------------------------------
// CURIContainer::InternalizeL
// ----------------------------------------------------------------------------
//
EXPORT_C CURIContainer* CURIContainer::InternalizeL(RReadStream& aReadStream)
    {
    CURIContainer* container = new(ELeave) CURIContainer;
    CleanupStack::PushL(container);
    container->ConstructL();
    container->DoInternalizeL(aReadStream);
    CleanupStack::Pop(container);
    return container;
    }

// ----------------------------------------------------------------------------
// CURIContainer::ExternalizeL
// ----------------------------------------------------------------------------
//
EXPORT_C void CURIContainer::ExternalizeL(RWriteStream& aWriteStream) const
    {
    if(IsSIPURI())
        {
        HBufC8* tmp = iSIPURI->ToTextL();
        CleanupStack::PushL(tmp);
        aWriteStream.WriteUint32L(tmp->Des().Length());
        aWriteStream.WriteL(tmp->Des());
        CleanupStack::PopAndDestroy(tmp);
        }
    else
        {
        TPtrC8 uri8(iUri8->Uri().UriDes());
        aWriteStream.WriteUint32L(uri8.Length());
        aWriteStream.WriteL(uri8);
        }
    }

// ----------------------------------------------------------------------------
// CURIContainer::CloneAsUri8L
// ----------------------------------------------------------------------------
//
EXPORT_C CUri8* CURIContainer::CloneAsUri8L() const
    {
    HBufC8* uriAsText = ToTextL();
    CleanupStack::PushL(uriAsText);
    CUri8* clone = CreateUri8L(*uriAsText);
    CleanupStack::PopAndDestroy(uriAsText);
    return clone;
    }

// ----------------------------------------------------------------------------
// CURIContainer::DoInternalizeL
// ----------------------------------------------------------------------------
//
void CURIContainer::DoInternalizeL(RReadStream& aReadStream)
    {
    TInt length(aReadStream.ReadUint32L());
    HBufC8* buffer = HBufC8::NewLC(length);
    TPtr8 bufptr(buffer->Des());
    aReadStream.ReadL(bufptr, length);
    if(SIPHeaderLookup::ConvertToSIPURI())
        {
        if(FindSIP(bufptr))
            {
            iSIPURI = CSIPURI::DecodeL(bufptr);
            }
        else
            {
            iUri8 = CreateUri8L(bufptr);
            }
        }
    else
        {
        iUri8 = CreateUri8L(bufptr);
        }
    CleanupStack::PopAndDestroy(buffer);
    }

// ----------------------------------------------------------------------------
// CURIContainer::CreateUri8L
// ----------------------------------------------------------------------------
//
CUri8* CURIContainer::CreateUri8L(const TDesC8& aValue)
    {
    TUriParser8 parser;
    User::LeaveIfError(parser.Parse(aValue));
    TPtrC8 scheme(parser.Extract(EUriScheme));
    if (scheme.CompareF(SIPStrings::StringF(SipStrConsts::ESip).DesC()) == 0)
        {
        User::LeaveIfError(parser.Validate());
        }
    return CUri8::NewL(parser);
    }

// ----------------------------------------------------------------------------
// CURIContainer::FindSIP
// ----------------------------------------------------------------------------
//
TBool CURIContainer::FindSIP(const TDesC8& aValue)
    {
    _LIT8(KSIP, "sip:");
    _LIT8(KSIPS, "sips:");
    TPtrC8 value(aValue);
    if(value.FindF(KSIP) == 0 ||
       value.FindF(KSIPS) == 0)
        {
        return ETrue;
        }
    return EFalse;
    }

