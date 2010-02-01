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
// Name          : SIPCodecUtils.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipcodecutils.h"
#include "siprouteheader.h"
#include "siprecordrouteheader.h"
#include "sipaddress.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "_sipcodecdefs.h"

// -----------------------------------------------------------------------------
// SIPCodecUtils::SIPCodecUtils
// -----------------------------------------------------------------------------
//
SIPCodecUtils::SIPCodecUtils()
	{	
	}

// -----------------------------------------------------------------------------
// SIPCodecUtils::CreateRouteHeaderLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRouteHeader* 
SIPCodecUtils::CreateRouteHeaderLC(const CSIPRouteHeaderBase& aHeader)
	{
	CSIPRouteHeader* header = CSIPRouteHeader::NewL(aHeader);
	CleanupStack::PushL(header);
	return header;
	}
			
// -----------------------------------------------------------------------------
// SIPCodecUtils::CopyRecordRouteHeaderLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRecordRouteHeader* 
SIPCodecUtils::CreateRecordRouteHeaderLC(const CSIPRouteHeaderBase& aHeader)
	{
	CSIPRecordRouteHeader* header = CSIPRecordRouteHeader::NewL(aHeader);
	CleanupStack::PushL(header);
	return header;
	}

// -----------------------------------------------------------------------------
// SIPCodecUtils::CreateSIPAddressLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress* SIPCodecUtils::CreateSIPAddressLC(CURIContainer* aURI)
    {
    return CSIPAddress::NewLC(aURI);
    }

// -----------------------------------------------------------------------------
// SIPCodecUtils::CheckAndCreateTokenL
// -----------------------------------------------------------------------------
//
RStringF 
SIPCodecUtils::CheckAndCreateTokenL(const TDesC8& aToken, TInt aLeaveCode)
    {
	HBufC8* tmp = aToken.AllocLC();
	tmp->Des().Trim();
	if (!SIPSyntaxCheck::Token(*tmp))
		{
		User::Leave(aLeaveCode);
		}
	RStringF tmpString = SIPStrings::Pool().OpenFStringL(*tmp);
	CleanupStack::PopAndDestroy(tmp);		
    return tmpString;
    }

// -----------------------------------------------------------------------------
// SIPCodecUtils::ValueWithoutQuotes
// -----------------------------------------------------------------------------
//
TInt SIPCodecUtils::ValueWithoutQuotes(
    const TDesC8& aValue,
    TPtrC8& aWithoutQuotes)
    {
    if (aValue.Length() > 0)
        {
        TLex8 lex(aValue);
        lex.SkipSpace();
        if (lex.Get() == '"')
            {
            // Remove first quote
            TPtrC8 remainder(aValue.Mid(1));
            // Ensure that ending quote exists.
            if(remainder.LocateReverse('"') != (remainder.Length()-1))
                {
                return KErrArgument;
                }
	        aWithoutQuotes.Set(remainder.Left(remainder.Length()-1));
            }
        else
            {
            return KErrArgument;
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// SIPCodecUtils::CheckAndSetValueL
// -----------------------------------------------------------------------------
//
void SIPCodecUtils::CheckAndSetValueL(HBufC8*& aBuffer, 
                                      const TDesC8& aValue, 
                                      TInt aErr)
    {
	HBufC8* tmp = aValue.AllocLC();
	tmp->Des().Trim();
	if (!SIPSyntaxCheck::Token(*tmp))
		{
		User::Leave(aErr);
		}
	CleanupStack::Pop(tmp);
	delete aBuffer;
	aBuffer = tmp;
    }

// -----------------------------------------------------------------------------
// SIPCodecUtils::ReadDescFromStreamL
// -----------------------------------------------------------------------------
//
HBufC8* SIPCodecUtils::ReadDescFromStreamL(RReadStream& aReadStream)
    {
	TUint32 valueLength = aReadStream.ReadUint32L();
	HBufC8* buf = HBufC8::NewLC(valueLength);
    if(valueLength > 0)
        {
	    TPtr8 value(buf->Des());
	    aReadStream.ReadL(value,valueLength);
        }
    CleanupStack::Pop(buf);
    return buf;
    }
