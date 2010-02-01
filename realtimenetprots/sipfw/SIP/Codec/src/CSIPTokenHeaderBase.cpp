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
// Name          : CSIPTokenHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "siptokenheaderbase.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::~CSIPTokenHeaderBase
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPTokenHeaderBase::~CSIPTokenHeaderBase()
	{
	iValue.Close();
	}

// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::SetValueL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPTokenHeaderBase::SetValueL(RStringF aValue)
	{
	SetValueL(aValue.DesC());
	}

// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::Value
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPTokenHeaderBase::Value() const
	{
	return iValue;
	}

// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::ToTextValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPTokenHeaderBase::ToTextValueL() const
	{
	return iValue.DesC().AllocL();
	}

// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::MoreThanOneAllowed
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPTokenHeaderBase::MoreThanOneAllowed() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::CSIPTokenHeaderBase
// -----------------------------------------------------------------------------
//
CSIPTokenHeaderBase::CSIPTokenHeaderBase()
	{
	}

// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::CSIPTokenHeaderBase
// -----------------------------------------------------------------------------
//
CSIPTokenHeaderBase::CSIPTokenHeaderBase(const CSIPTokenHeaderBase& aHeader)
 : CSIPHeaderBase()
	{
	iValue = aHeader.Value().Copy();
	}

// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPTokenHeaderBase::ConstructL(const TDesC8& aValue,
                                     TBool aEmptyValueAllowed)
	{
	TLex8 lex(aValue);
	lex.SkipSpace();
	if (lex.Remainder().Length() > 0)
		{
		SetValueL(aValue);
		}
	else
		{
		if (!aEmptyValueAllowed)
			{
			User::Leave(KErrSipCodecTokenBaseHeader);
			}
		iValue = SIPStrings::StringF(SipStrConsts::EEmpty);
		}	
	}

// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPTokenHeaderBase::ConstructL(RStringF aValue,
                                     TBool aEmptyValueAllowed)
	{
	ConstructL(aValue.DesC(),aEmptyValueAllowed);
	}

// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPTokenHeaderBase::ExternalizeValueL (RWriteStream& aWriteStream) const
	{
	TPtrC8 value(iValue.DesC());
	aWriteStream.WriteUint32L(value.Length());
	if(value.Length()) 
		{
		aWriteStream.WriteL(value);
		}
	}

// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CSIPTokenHeaderBase::DoInternalizeValueL (RReadStream& aReadStream)
	{
    HBufC8* value = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
    CleanupStack::PushL(value);
    iValue = SIPStrings::Pool().OpenFStringL(*value);
    CleanupStack::PopAndDestroy(value);
	}
	
// -----------------------------------------------------------------------------
// CSIPTokenHeaderBase::SetValueL
// -----------------------------------------------------------------------------
//
void CSIPTokenHeaderBase::SetValueL(const TDesC8& aValue)
	{
    RStringF tmp = SIPCodecUtils::CheckAndCreateTokenL(aValue, 
                                            KErrSipCodecTokenBaseHeader);
    iValue.Close();
    iValue = tmp;
	}





