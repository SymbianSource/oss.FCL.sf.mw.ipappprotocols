// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPHeaderLookupTable.cpp
// Part of       : SIP Codec
// Version       : SIP/6.0 
//



#include "CSIPHeaderLookupTable.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipextensionheader.h"
#include "sipfromheader.h"
#include "siptoheader.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "CSIPContentLengthHeader.h"
#include "sipviaheader.h"
#include "sipcontactheader.h"
#include "sipcontenttypeheader.h"
#include "sipexpiresheader.h"
#include "sipmaxforwardsheader.h"
#include "sipminexpiresheader.h"
#include "siprecordrouteheader.h"
#include "siprouteheader.h"
#include "sipservicerouteheader.h"
#include "siprequireheader.h"
#include "sipcontentencodingheader.h"
#include "sipproxyrequireheader.h"
#include "sipsupportedheader.h"
#include "sipunsupportedheader.h"
#include "sipallowheader.h"
#include "siprefertoheader.h"
#include "sipsecurityclientheader.h"
#include "sipsecurityserverheader.h"
#include "sipsecurityverifyheader.h"
#include "sipacceptheader.h"
#include "sipacceptlanguageheader.h"
#include "sipacceptencodingheader.h"
#include "sipauthenticationinfoheader.h"
#include "sipcontentdispositionheader.h"
#include "sipsubscriptionstateheader.h"
#include "sipeventheader.h"
#include "sipalloweventsheader.h"
#include "sipauthorizationheader.h"
#include "sipproxyauthenticateheader.h"
#include "sipproxyauthorizationheader.h"
#include "sipwwwauthenticateheader.h"
#include "sipreplytoheader.h"
#include "siprackheader.h"
#include "siprseqheader.h"
#include "siptimestampheader.h"
#include "sipretryafterheader.h"
#include "sipacceptcontactheader.h"
#include "sippaccessnetworkinfoheader.h"
#include "sippassociateduriheader.h"


// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::NewL
// -----------------------------------------------------------------------------
//
CSIPHeaderLookupTable* CSIPHeaderLookupTable::NewL ()
	{
	CSIPHeaderLookupTable* self = CSIPHeaderLookupTable::NewLC();
	CleanupStack::Pop(self);
	return self;
	} 

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::NewLC
// -----------------------------------------------------------------------------
//
CSIPHeaderLookupTable* CSIPHeaderLookupTable::NewLC()
	{
	CSIPHeaderLookupTable* self = new(ELeave)CSIPHeaderLookupTable;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::CSIPHeaderLookupTable
// -----------------------------------------------------------------------------
//
CSIPHeaderLookupTable::CSIPHeaderLookupTable()
 : iUsageCount(1), iConvertToSIPURI(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPHeaderLookupTable::ConstructL()
    {
    // CSIPViaHeader:
    AddL (SIPStrings::StringF(SipStrConsts::EViaHeader),
          SIPStrings::StringF(SipStrConsts::EViaHeaderCompact),
          &CSIPViaHeader::BaseDecodeL);

	// CSIPRouteHeader: 
    AddL (SIPStrings::StringF(SipStrConsts::ERouteHeader),
	      &CSIPRouteHeader::BaseDecodeL,
          &CSIPRouteHeader::InternalizeValueL);

	// CSIPRecordRouteHeader: 
    AddL (SIPStrings::StringF(SipStrConsts::ERecordRouteHeader),
	      &CSIPRecordRouteHeader::BaseDecodeL);

	// CSIPServiceRouteHeader: 
    AddL (SIPStrings::StringF(SipStrConsts::EServiceRouteHeader),
	      &CSIPServiceRouteHeader::BaseDecodeL);

	// CSIPFromHeader:
    AddL (SIPStrings::StringF(SipStrConsts::EFromHeader),
          SIPStrings::StringF(SipStrConsts::EFromHeaderCompact),
          &CSIPFromHeader::BaseDecodeL,
          &CSIPFromHeader::InternalizeValueL);

	// CSIPToHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EToHeader),
          SIPStrings::StringF(SipStrConsts::EToHeaderCompact),
          &CSIPToHeader::BaseDecodeL,
          &CSIPToHeader::InternalizeValueL);

	// CSIPCallIDHeader: 
    AddL (SIPStrings::StringF(SipStrConsts::ECallIDHeader),
          SIPStrings::StringF(SipStrConsts::ECallIDHeaderCompact),
          &CSIPCallIDHeader::BaseDecodeL);

	// CSIPCSeqHeader: 
	AddL (SIPStrings::StringF(SipStrConsts::ECSeqHeader),
	      &CSIPCSeqHeader::BaseDecodeL,
	      &CSIPCSeqHeader::InternalizeValueL);

	// CSIPContactHeader:
    AddL (SIPStrings::StringF(SipStrConsts::EContactHeader),
          SIPStrings::StringF(SipStrConsts::EContactHeaderCompact),	
          &CSIPContactHeader::BaseDecodeL,
          &CSIPContactHeader::InternalizeValueL);

	// CSIPExpiresHeader: 
    AddL (SIPStrings::StringF(SipStrConsts::EExpiresHeader),
          &CSIPExpiresHeader::BaseDecodeL,
          &CSIPExpiresHeader::InternalizeValueL);

	// CSIPMinExpiresHeader: 
	AddL (SIPStrings::StringF(SipStrConsts::EMinExpiresHeader),
	      &CSIPMinExpiresHeader::BaseDecodeL);

	// CSIPMaxForwardsHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EMaxForwardsHeader),
	      &CSIPMaxForwardsHeader::BaseDecodeL);

	// CSIPContentTypeHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EContentTypeHeader),
          SIPStrings::StringF(SipStrConsts::EContentTypeHeaderCompact),
          &CSIPContentTypeHeader::BaseDecodeL,
          &CSIPContentTypeHeader::InternalizeValueL);
          
	// CSIPContentLengthHeader: 
	AddL (SIPStrings::StringF(SipStrConsts::EContentLengthHeader),
          SIPStrings::StringF(SipStrConsts::EContentLengthHeaderCompact),
          &CSIPContentLengthHeader::BaseDecodeL);

	// CSIPContentEncodingHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EContentEncodingHeader),
	      SIPStrings::StringF(SipStrConsts::EContentEncodingHeaderCompact),
          &CSIPContentEncodingHeader::BaseDecodeL,
          &CSIPContentEncodingHeader::InternalizeValueL);

    // CSIPAllowHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EAllowHeader),
	      &CSIPAllowHeader::BaseDecodeL,
          &CSIPAllowHeader::InternalizeValueL);

	// CSIPSupportedHeader:
	AddL (SIPStrings::StringF(SipStrConsts::ESupportedHeader),
	      SIPStrings::StringF(SipStrConsts::ESupportedHeaderCompact),
          &CSIPSupportedHeader::BaseDecodeL,
          &CSIPSupportedHeader::InternalizeValueL);

    // CSIPUnsupportedHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EUnsupportedHeader),
	      &CSIPUnsupportedHeader::BaseDecodeL,
          &CSIPUnsupportedHeader::InternalizeValueL);

	// CSIPRequireHeader: 
	AddL (SIPStrings::StringF(SipStrConsts::ERequireHeader),
	      &CSIPRequireHeader::BaseDecodeL,
          &CSIPRequireHeader::InternalizeValueL);

	// CSIPProxyRequireHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EProxyRequireHeader),
	      &CSIPProxyRequireHeader::BaseDecodeL,
          &CSIPProxyRequireHeader::InternalizeValueL);
	
	// CSIPReferToHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EReferToHeader),
	      SIPStrings::StringF(SipStrConsts::EReferToHeaderCompact),
          &CSIPReferToHeader::BaseDecodeL,
          &CSIPReferToHeader::InternalizeValueL);        

    // CSIPSecurityClientHeader:
	AddL (SIPStrings::StringF(SipStrConsts::ESecurityClientHeader),
	      &CSIPSecurityClientHeader::BaseDecodeL,
          &CSIPSecurityClientHeader::InternalizeValueL);

    // CSIPSecurityServerHeader:
	AddL (SIPStrings::StringF(SipStrConsts::ESecurityServerHeader),
	      &CSIPSecurityServerHeader::BaseDecodeL);          

    // CSIPSecurityVerifyHeader:
	AddL (SIPStrings::StringF(SipStrConsts::ESecurityVerifyHeader),
	      &CSIPSecurityVerifyHeader::BaseDecodeL);

	// CSIPAuthenticationInfoHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EAuthenticationInfoHeader),
          &CSIPAuthenticationInfoHeader::BaseDecodeL);
          
	// CSIPAcceptHeader:
    AddL (SIPStrings::StringF(SipStrConsts::EAcceptHeader),
          &CSIPAcceptHeader::BaseDecodeL,
          &CSIPAcceptHeader::InternalizeValueL);

	// CSIPAcceptEncodingHeader:
    AddL (SIPStrings::StringF(SipStrConsts::EAcceptEncodingHeader),
          &CSIPAcceptEncodingHeader::BaseDecodeL,
          &CSIPAcceptEncodingHeader::InternalizeValueL);

	// CSIPAcceptLanguageHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EAcceptLanguageHeader),
	      &CSIPAcceptLanguageHeader::BaseDecodeL,
          &CSIPAcceptLanguageHeader::InternalizeValueL);          

	// CSIPContentDispositionHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EContentDispositionHeader),
          &CSIPContentDispositionHeader::BaseDecodeL,
          &CSIPContentDispositionHeader::InternalizeValueL);

	// CSIPSubscriptionStateHeader:
	AddL (SIPStrings::StringF(SipStrConsts::ESubscriptionStateHeader),
          &CSIPSubscriptionStateHeader::BaseDecodeL,
          &CSIPSubscriptionStateHeader::InternalizeValueL);

	// CSIPAllowEventsHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EAllowEventsHeader),
	      SIPStrings::StringF(SipStrConsts::EAllowEventsHeaderCompact),
          &CSIPAllowEventsHeader::BaseDecodeL,
          &CSIPAllowEventsHeader::InternalizeValueL);

	// CSIPEventHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EEventHeader),
	      SIPStrings::StringF(SipStrConsts::EEventHeaderCompact),
          &CSIPEventHeader::BaseDecodeL,
          &CSIPEventHeader::InternalizeValueL);
          
    // CSIPAuthorizationHeader:      
	AddL (SIPStrings::StringF(SipStrConsts::EAuthorizationHeader),
          &CSIPAuthorizationHeader::BaseDecodeL,
          &CSIPAuthorizationHeader::InternalizeValueL);
          
    // CSIPProxyAuthenticateHeader:      
	AddL (SIPStrings::StringF(SipStrConsts::EProxyAuthenticateHeader),
          &CSIPProxyAuthenticateHeader::BaseDecodeL,
          &CSIPProxyAuthenticateHeader::InternalizeValueL);
          
    // CSIPProxyAuthorizationHeader:      
	AddL (SIPStrings::StringF(SipStrConsts::EProxyAuthorizationHeader),
          &CSIPProxyAuthorizationHeader::BaseDecodeL,
          &CSIPProxyAuthorizationHeader::InternalizeValueL);
          
    // CSIPWWWAuthenticateHeader:      
	AddL (SIPStrings::StringF(SipStrConsts::EWWWAuthenticateHeader),
          &CSIPWWWAuthenticateHeader::BaseDecodeL,
          &CSIPWWWAuthenticateHeader::InternalizeValueL);
    
	// CSIPReplyToHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EReplyToHeader),
          &CSIPReplyToHeader::BaseDecodeL,
          &CSIPReplyToHeader::InternalizeValueL);        

    // CSIPRAckHeader:
	AddL (SIPStrings::StringF(SipStrConsts::ERAckHeader),
          &CSIPRAckHeader::BaseDecodeL,
          &CSIPRAckHeader::InternalizeValueL);        

    // CSIPRSeqHeader:
	AddL (SIPStrings::StringF(SipStrConsts::ERSeqHeader),
          &CSIPRSeqHeader::BaseDecodeL,
          &CSIPRSeqHeader::InternalizeValueL);        
    
    // CSIPTimestampHeader:
	AddL (SIPStrings::StringF(SipStrConsts::ETimestampHeader),
          &CSIPTimestampHeader::BaseDecodeL,
          &CSIPTimestampHeader::InternalizeValueL);        

    // CSIPRetryAfterHeader:
	AddL (SIPStrings::StringF(SipStrConsts::ERetryAfterHeader),
          &CSIPRetryAfterHeader::BaseDecodeL,
          &CSIPRetryAfterHeader::InternalizeValueL); 
    
	// CSIPAcceptContactHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EAcceptContactHeader),
          SIPStrings::StringF(SipStrConsts::EAcceptContactHeaderCompact),
          &CSIPAcceptContactHeader::BaseDecodeL,
          &CSIPAcceptContactHeader::InternalizeValueL);

    // CSIPPAccessNetworkInfoHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EPAccessNetworkInfoHeader),
          &CSIPPAccessNetworkInfoHeader::BaseDecodeL); 

    // CSIPPAssociatedURIHeader:
	AddL (SIPStrings::StringF(SipStrConsts::EPAssociatedURIHeader),
          &CSIPPAssociatedURIHeader::BaseDecodeL,
          &CSIPPAssociatedURIHeader::InternalizeValueL); 
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::~CSIPHeaderLookupTable
// -----------------------------------------------------------------------------
//
CSIPHeaderLookupTable::~CSIPHeaderLookupTable ()
	{
	iLookupTable.Close();
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::IsSupported
// -----------------------------------------------------------------------------
//
TBool CSIPHeaderLookupTable::IsSupported (RStringF aName) const
	{
	return (FindIndex(aName) >= 0);
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::InternalizeL
// -----------------------------------------------------------------------------
//
CSIPHeaderBase* CSIPHeaderLookupTable::InternalizeL (RStringF aName,
									                 RReadStream& aReadStream)
	{
	__ASSERT_ALWAYS(aName.DesC().Length() > 0, User::Leave(KErrArgument));

	TInt index = FindIndex(aName);
	if (index >= 0) // a supported header found
		{
		return iLookupTable[index].InternalizeL(aReadStream);
		}

	// a supported header not found, internalize extension header instead
	CSIPExtensionHeader* extensionHeader =
		CSIPExtensionHeader::InternalizeValueL(aReadStream);
	CleanupStack::PushL (extensionHeader);
	extensionHeader->SetNameL(aName.DesC());
	CleanupStack::Pop(extensionHeader);
	return extensionHeader;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::InternalizeSupported
// -----------------------------------------------------------------------------
//
TBool CSIPHeaderLookupTable::InternalizeSupported (RStringF aName) const
    {
    TBool isSupported=EFalse;
    TInt index = FindIndex (aName);
    if (index >= 0)
        {
        isSupported = iLookupTable[index].InternalizeSupported();
        }
    return isSupported;
    }

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::CreateHeaderL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPHeaderLookupTable::CreateHeaderL (RStringF aName,
									  const TDesC8& aValue)
	{
	__ASSERT_ALWAYS(aName.DesC().Length() > 0, User::Leave(KErrArgument));

	TInt index = FindIndex(aName);
	if (index >= 0) // a supported header found
		{
		return iLookupTable[index].DecodeL(aValue);
		}
	
	// a supported header not found, create extension header instead
	RPointerArray<CSIPHeaderBase> headerArray;
	CSIPHeaderBase* header = CSIPExtensionHeader::NewLC(aName.DesC(),aValue);
	headerArray.AppendL(header);
	CleanupStack::Pop(header);
	return headerArray;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::IncrementUsageCount
// -----------------------------------------------------------------------------
//
void CSIPHeaderLookupTable::IncrementUsageCount()
	{
	iUsageCount++;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::DecrementUsageCount
// -----------------------------------------------------------------------------
//
void CSIPHeaderLookupTable::DecrementUsageCount()
	{
	iUsageCount--;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::UsageCount
// -----------------------------------------------------------------------------
//
TUint CSIPHeaderLookupTable::UsageCount()
	{
	return iUsageCount;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::AddL
// -----------------------------------------------------------------------------
//
void CSIPHeaderLookupTable::AddL(RStringF aFullName,
                                 RPointerArray<CSIPHeaderBase>(*aDecode)(const TDesC8&),
                                 CSIPHeaderBase*(*aInternalize)(RReadStream&))
    {
	TSIPHeaderLookupEntry entry(aFullName,aDecode,aInternalize);
	User::LeaveIfError (iLookupTable.Append(entry));
    }

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::AddL
// -----------------------------------------------------------------------------
//
void CSIPHeaderLookupTable::AddL(RStringF aFullName,
                                 RStringF aCompactName,
                                 RPointerArray<CSIPHeaderBase>(*aDecode)(const TDesC8&),
                                 CSIPHeaderBase*(*aInternalize)(RReadStream&))
    {
	TSIPHeaderLookupEntry entry (aFullName,aCompactName,aDecode,aInternalize);
	User::LeaveIfError (iLookupTable.Append(entry));
    }

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::FindIndex
// -----------------------------------------------------------------------------
//
TInt CSIPHeaderLookupTable::FindIndex(RStringF aName) const
	{
	for (TInt index=0; index < iLookupTable.Count(); index++)
		{
		TSIPHeaderLookupEntry lookupEntry = iLookupTable[index];

		if (aName == lookupEntry.Name() || 
		    aName == lookupEntry.CompactName())
			{
			return index;
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::ConvertToSIPURI
// -----------------------------------------------------------------------------
//
TBool CSIPHeaderLookupTable::ConvertToSIPURI()
    {
    return iConvertToSIPURI;
    }

// -----------------------------------------------------------------------------
// CSIPHeaderLookupTable::SetConvertToSIPURI
// -----------------------------------------------------------------------------
//
void CSIPHeaderLookupTable::SetConvertToSIPURI(TBool aNewValue)
    {
    iConvertToSIPURI = aNewValue;
    }
