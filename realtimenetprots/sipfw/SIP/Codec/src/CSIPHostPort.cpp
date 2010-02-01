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
// Name          : CSIPHostPort.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "siphostport.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "_sipcodecdefs.h"

_LIT8(KPortFormat, "%u");


// -----------------------------------------------------------------------------
// CSIPHostPort::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHostPort* CSIPHostPort::DecodeL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, User::Leave(KErrSipCodecHostPort));

	CSIPHostPort* hostPort = new(ELeave)CSIPHostPort;
    CleanupStack::PushL (hostPort);

	if (aValue.Locate('[') == 0) // seems to be an IPv6 address
		{
		TInt hostEndPos = aValue.Locate(']');
		if (hostEndPos > 1)
			{
			hostPort->SetHostL (aValue.Mid(1,hostEndPos-1));
			if (hostEndPos < aValue.Length()-1) // seems to have a port
				{
				TLex8 lex(aValue.Mid(hostEndPos+1));
				lex.SkipSpace();
				TPtrC8 portAsText(lex.Remainder());
				if (portAsText.Length() > 0)
					{
					TInt colonPos = portAsText.Locate (':');
					if (colonPos != 0) User::Leave(KErrSipCodecPort);
					hostPort->SetPort (ParsePortL(portAsText.Mid(1)));
					}
				}
			}
		else
			{
			User::Leave(KErrSipCodecHostPort);
			}
	    __ASSERT_ALWAYS (hostPort->HostType() == ESIPIpv6, 
	                     User::Leave(KErrSipCodecHostPort));
		}
	else // seems to be an IPv4 address or a host name 
		{
		TInt colonPos = aValue.Locate(':');
		if (colonPos == 0 || colonPos == aValue.Length()-1)
			{
			User::Leave(KErrSipCodecPort);
			}
		if (colonPos > 0) // has port
			{
			hostPort->SetPort (ParsePortL (aValue.Mid(colonPos+1)));
			hostPort->SetHostL (aValue.Left(colonPos));
			}
		else // has only host part
			{
			hostPort->SetHostL (aValue);
			}
		}
	CleanupStack::Pop(hostPort);
    
	return hostPort;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHostPort* CSIPHostPort::NewL (const CSIPHostPort& aHostPort)
	{
	CSIPHostPort* self = CSIPHostPort::NewLC (aHostPort);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHostPort* CSIPHostPort::NewLC (const CSIPHostPort& aHostPort)
	{
	CSIPHostPort* self = new(ELeave)CSIPHostPort;
	CleanupStack::PushL(self);
	self->ConstructL (aHostPort);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::CSIPHostPort
// -----------------------------------------------------------------------------
//
CSIPHostPort::CSIPHostPort()
 : iHostType (ESIPNoHost),
   iHasPort (EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPHostPort::ConstructL ()
	{
	iHost = HBufC8::NewL(0);
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPHostPort::ConstructL (const CSIPHostPort& aHostPort)
	{
	iHost = aHostPort.iHost->AllocL();
	iHostType = aHostPort.iHostType;
	iHasPort = aHostPort.iHasPort;
	iPort = aHostPort.iPort;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::~CSIPHostPort
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHostPort::~CSIPHostPort()
	{
	delete iHost;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::SetHostL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPHostPort::SetHostL (const TDesC8& aHost)
	{
	HBufC8* tmp = aHost.AllocLC();
	tmp->Des().Trim();
	if (tmp->Length() == 0) 
        {
        User::Leave(KErrSipCodecHost);
        }

	TType hostType;
	TInt err = SIPSyntaxCheck::HostType(*tmp,hostType);
	if (err != KErrNone) 
        {
        User::Leave (err);
        }

	CleanupStack::Pop(tmp);

	delete iHost;
	iHost = tmp;
	iHostType = hostType;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::HostType
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHostPort::TType CSIPHostPort::HostType () const
	{
	return iHostType;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::Host
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPHostPort::Host () const
	{
	return *iHost;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::HasPort
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPHostPort::HasPort() const
	{
	return iHasPort;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::Port
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPHostPort::Port() const
	{
	if (iHasPort) 
        {
        return iPort;
        }
	return 0;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::SetPort
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPHostPort::SetPort (TUint aPort)
	{
	iPort = aPort;
	iHasPort = ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::DeletePort
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPHostPort::DeletePort ()
	{
	if (!iHasPort) 
        {
        return KErrNotFound;
        }
	iPort = 0;
	iHasPort = EFalse;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::operator==
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPHostPort::operator==(const CSIPHostPort& aHostPort) const
	{
	if (iHostType != aHostPort.iHostType) 
        {
        return EFalse;
        }
	if (iHost->CompareF(*(aHostPort.iHost)) != 0) 
        {
        return EFalse;
        }
	if (iHasPort != aHostPort.iHasPort) 
        {
        return EFalse;
        }
	if (iHasPort)
		{
		if (iPort != aHostPort.iPort) 
            {
            return EFalse;
            }
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::ToTextLC
// -----------------------------------------------------------------------------
//
HBufC8* CSIPHostPort::ToTextLC () const
	{
	TUint encodedLength = iHost->Length();
	if (iHostType == ESIPIpv6)
	    {
	    const TInt KBracketsLength = 2; // []
	    encodedLength += KBracketsLength;
	    }
	
	const TInt KMaxPortAsText = 40;
	TBuf8<KMaxPortAsText> portAsText;
	if (iHasPort)
		{
		portAsText.Format(KPortFormat, iPort);
		encodedLength += 1 + portAsText.Length(); // COLON + port
		}
	
	HBufC8* encodedHostPort = HBufC8::NewLC(encodedLength);
	TPtr8 encodedHostPortPtr = encodedHostPort->Des();
	if (iHostType == ESIPIpv6)
		{
		encodedHostPortPtr.Append('[');
		encodedHostPortPtr.Append(*iHost);
		encodedHostPortPtr.Append(']');
		}
	else
		{
		encodedHostPortPtr.Append(*iHost);
		}

	if (iHasPort)
		{
		encodedHostPortPtr.Append(':');
		encodedHostPortPtr.Append(portAsText);
		}
	return encodedHostPort;
	}

// -----------------------------------------------------------------------------
// CSIPHostPort::ParsePortL
// -----------------------------------------------------------------------------
//
TUint CSIPHostPort::ParsePortL (const TDesC8& aValue)
	{
	TLex8 lex(aValue);
	lex.SkipSpace();
	TPtrC8 remainder(lex.Remainder());
    if (remainder.Length() == 0) 
        {
        User::Leave(KErrSipCodecPort);
        }
	TUint parsedValue=0;
	if (lex.Val(parsedValue) != KErrNone) 
        {
        User::Leave (KErrSipCodecPort);
        }
    lex.SkipSpace();
	if (lex.Remainder().Length() != 0) 
        {
        User::Leave (KErrSipCodecPort);
        }
	return parsedValue;
	}
