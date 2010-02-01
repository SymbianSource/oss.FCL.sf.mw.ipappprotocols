// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSdpRtpmapStrategy.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSdpRtpmapStrategy.h"
#include "sdpdocument.h"
#include "sdpmediafield.h"
#include "sdpcodecstringconstants.h"
#include "sdpcodecstringpool.h"

const TUint KMinDynPayloadType = 96;

// ----------------------------------------------------------------------------
// CSdpRtpmapStrategy::NewLC
// ----------------------------------------------------------------------------
//
CSdpRtpmapStrategy* CSdpRtpmapStrategy::NewLC(
    const MSipClients& aSipClients)
	{
	CSdpRtpmapStrategy* self = 
		new(ELeave)CSdpRtpmapStrategy(aSipClients);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpRtpmapStrategy::CSdpRtpmapStrategy
// ----------------------------------------------------------------------------
//
CSdpRtpmapStrategy::CSdpRtpmapStrategy(const MSipClients& aSipClients) 
 : CSdpMLineStrategyBase(aSipClients)
	{
	}

// ----------------------------------------------------------------------------
// CSdpRtpmapStrategy::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpRtpmapStrategy::ConstructL()
	{
	iApplicationStringF = 
	    SdpCodecStringPool::StringPoolL().StringF(
	        SdpCodecStringConstants::EMediaApplication,
	        SdpCodecStringPool::StringTableL());	
	
	iRtpAvpStringF = 
	    SdpCodecStringPool::StringPoolL().StringF(
	        SdpCodecStringConstants::EProtocolRtpAvp,
	        SdpCodecStringPool::StringTableL());
	        
	iRtpmapStringF = 
	    SdpCodecStringPool::StringPoolL().StringF(
	        SdpCodecStringConstants::EAttributeRtpmap,
	        SdpCodecStringPool::StringTableL());
	}

// ----------------------------------------------------------------------------
// CSdpRtpmapStrategy::~CSdpRtpmapStrategy
// ----------------------------------------------------------------------------
//
CSdpRtpmapStrategy::~CSdpRtpmapStrategy ()
	{
	iRtpmapStringF.Close();
	iRtpAvpStringF.Close();
	iApplicationStringF.Close();
	}

// ----------------------------------------------------------------------------
// CSdpRtpmapStrategy::IsApplicable
// ----------------------------------------------------------------------------
//	
TBool CSdpRtpmapStrategy::IsApplicable(CSdpMediaField& aMediaField)
    {
	if (aMediaField.Media() != iApplicationStringF &&
	    aMediaField.Protocol() == iRtpAvpStringF &&
	    aMediaField.FormatAttributeFields().Count() > 0)
	    {
	    return ETrue;
	    }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CSdpRtpmapStrategy::FieldsMatchL
// ----------------------------------------------------------------------------
//
TBool CSdpRtpmapStrategy::FieldsMatchL(
    const CSdpMediaField& aReceivedMedia,
    const CSdpMediaField& aClientMedia)
    {
	if (aReceivedMedia.Media() == aClientMedia.Media() &&
	    aReceivedMedia.Protocol() == iRtpAvpStringF &&
	    aClientMedia.Protocol() == iRtpAvpStringF)
	    {
        const RPointerArray<CSdpFmtAttributeField>& clientAttributes = 
            const_cast<CSdpMediaField&>(aClientMedia).FormatAttributeFields();
	    if (clientAttributes.Count() == 0)
	        {
	        // A client that does not announce any RTPMAPs 
	        // is assumed to accept all possible mappings.
	        return ETrue;
	        }
	        
	    if (HasMatchingFormatsL(aReceivedMedia,aClientMedia))
	        {
	        return ETrue;
	        }
	        
        const RPointerArray<CSdpFmtAttributeField>& attributes = 
            const_cast<CSdpMediaField&>(aReceivedMedia).FormatAttributeFields();
	    for (TInt i=0; i < attributes.Count(); i++)
	        {
	        if (HasMatchingRtpmapAttritbute(aClientMedia,*attributes[i]))
	            {
	            return ETrue;
	            }
	        }
	    }
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSdpRtpmapStrategy::HasMatchingRtpmapAttritbute
// ----------------------------------------------------------------------------
//	
TBool CSdpRtpmapStrategy::HasMatchingRtpmapAttritbute(
    const CSdpMediaField& aMediaField,
    const CSdpFmtAttributeField& aAttribute)
    {
    if (aAttribute.Attribute() != iRtpmapStringF)
        {
        return EFalse;
        }
        
    const RPointerArray<CSdpFmtAttributeField>& attributes = 
        const_cast<CSdpMediaField&>(aMediaField).FormatAttributeFields();
        
    for (TInt i=0; i < attributes.Count(); i++)
        {
        CSdpFmtAttributeField& attribute = *attributes[i];
        if (attribute.Attribute() == iRtpmapStringF &&
            aAttribute.Value().CompareF(attribute.Value()) == 0)
            {
            return ETrue;
            }
        }
    
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CSdpRtpmapStrategy::HasMatchingFormatsL
// ----------------------------------------------------------------------------
//
TBool CSdpRtpmapStrategy::HasMatchingFormatsL(
    const CSdpMediaField& aReceivedMedia,
    const CSdpMediaField& aClientMedia) const
    {
    if (aReceivedMedia.Protocol() != iRtpAvpStringF ||
	    aClientMedia.Protocol() != iRtpAvpStringF)
	    {
	    return EFalse;
	    }
    
    RArray<TUint> formats(ExtractFormatsL(aReceivedMedia.FormatList()));
    CleanupClosePushL(formats);
    
    const RPointerArray<CSdpFmtAttributeField>& clientAttributes = 
        const_cast<CSdpMediaField&>(aClientMedia).FormatAttributeFields();
    
    for (TInt i=0; i < clientAttributes.Count(); i++)
        {
        const CSdpFmtAttributeField& clientAttribute = *clientAttributes[i];
        if (clientAttribute.Attribute() == iRtpmapStringF)
            {
            TLex8 formatLex(clientAttribute.Format());
            TUint value = 0;
            if (formatLex.Val(value) == KErrNone && 
                value < KMinDynPayloadType)
                {
                TInt index = 0;
                TInt err = formats.FindInOrder(value,index);
                if (err == KErrNone && index >= 0)
                    {
                    CleanupStack::PopAndDestroy(1); // formats 
                    return ETrue;
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(1); // formats      
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CSdpRtpmapStrategy::ExtractFormatsL
// ----------------------------------------------------------------------------
//
RArray<TUint> CSdpRtpmapStrategy::ExtractFormatsL(
    const TDesC8& aFormats) const
    {
    RArray<TUint> formats;
    CleanupClosePushL(formats);
    TLex8 formatLex(aFormats);
    formatLex.SkipSpace();
    TUint value = 0;
    while (!formatLex.Eos())
        {
        if (formatLex.Val(value) == KErrNone)
            {
            formats.InsertInOrderAllowRepeatsL(value);
            }
        else
            {
            // Skip the invalid token
            formatLex.NextToken();
            }
        formatLex.SkipSpace();
        }
    CleanupStack::Pop(&formats);
    return formats;
    }
