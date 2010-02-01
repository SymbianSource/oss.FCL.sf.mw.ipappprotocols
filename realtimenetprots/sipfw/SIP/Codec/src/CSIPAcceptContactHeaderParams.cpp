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
// Name          : CSIPAcceptContactHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPAcceptContactHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// -----------------------------------------------------------------------------
// CSIPAcceptContactHeaderParams::NewL
// -----------------------------------------------------------------------------
//
CSIPAcceptContactHeaderParams* CSIPAcceptContactHeaderParams::NewL (
    const CSIPAcceptContactHeaderParams& aParams)
	{
	CSIPAcceptContactHeaderParams* self = 
        CSIPAcceptContactHeaderParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPAcceptContactHeaderParams::NewLC
// -----------------------------------------------------------------------------
//
CSIPAcceptContactHeaderParams* CSIPAcceptContactHeaderParams::NewLC (
    const CSIPAcceptContactHeaderParams& aParams)
	{
	CSIPAcceptContactHeaderParams* self = 
        new(ELeave)CSIPAcceptContactHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL (aParams);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPAcceptContactHeaderParams::CSIPAcceptContactHeaderParams
// -----------------------------------------------------------------------------
//
CSIPAcceptContactHeaderParams::CSIPAcceptContactHeaderParams ()
 : CSIPParamContainerBase(';')
	{
	}

// -----------------------------------------------------------------------------
// CSIPAcceptContactHeaderParams::~CSIPAcceptContactHeaderParams
// -----------------------------------------------------------------------------
//
CSIPAcceptContactHeaderParams::~CSIPAcceptContactHeaderParams ()
	{
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeaderParams::InternalizeL
// ----------------------------------------------------------------------------
//
CSIPAcceptContactHeaderParams* CSIPAcceptContactHeaderParams::InternalizeL (
    RReadStream& aReadStream)
	{
	CSIPAcceptContactHeaderParams* self = 
	    new(ELeave)CSIPAcceptContactHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeaderParams::operator==
// ----------------------------------------------------------------------------
//
TBool CSIPAcceptContactHeaderParams::operator==(
    const CSIPAcceptContactHeaderParams& aParams) const
    {
    // There should be exactly same number of params.
    TInt paramCount(ParamCount());
    if(paramCount != aParams.ParamCount())
        {
        return EFalse;
        }

    // All params should exist in both.
    for(TInt i=0; i<paramCount; i++)
        {
        TInt index = aParams.FindParamIndex(iParams[i]->Name());
		if (index >= 0)
			{
			if (!(*(aParams.iParams[index]) == *(iParams[i]))) 
                {
                return EFalse; // Params don't match
                }
			}
        else
            {
            return EFalse; // Not found
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPAcceptContactHeaderParams::CheckParamL
// -----------------------------------------------------------------------------
//
void CSIPAcceptContactHeaderParams::CheckParamL (CSIPParam& aParam) const
	{
    RStringF name = aParam.Name();
	TPtrC8 value = aParam.Value().DesC();
	// feature-param base-tags/other-tags
	if(name == SIPStrings::StringF(SipStrConsts::EAudio) ||
	    name == SIPStrings::StringF(SipStrConsts::EAutomata) ||
		name == SIPStrings::StringF(SipStrConsts::EClass) ||
		name == SIPStrings::StringF(SipStrConsts::EDuplex) ||
		name == SIPStrings::StringF(SipStrConsts::EData) ||
		name == SIPStrings::StringF(SipStrConsts::EControl) ||
		name == SIPStrings::StringF(SipStrConsts::EMobility) ||
		name == SIPStrings::StringF(SipStrConsts::EDescription) ||
		name == SIPStrings::StringF(SipStrConsts::EEvents) ||
		name == SIPStrings::StringF(SipStrConsts::EPriority) ||
		name == SIPStrings::StringF(SipStrConsts::EMethods) ||
		name == SIPStrings::StringF(SipStrConsts::ESchemes) ||
		name == SIPStrings::StringF(SipStrConsts::EApplication) ||
		name == SIPStrings::StringF(SipStrConsts::EVideo) ||
		name == SIPStrings::StringF(SipStrConsts::ELanguage) ||
		name == SIPStrings::StringF(SipStrConsts::EType) ||
		name == SIPStrings::StringF(SipStrConsts::EIsfocus) ||
		name == SIPStrings::StringF(SipStrConsts::EActor) ||
		name == SIPStrings::StringF(SipStrConsts::EText) ||
		name == SIPStrings::StringF(SipStrConsts::EExtensions))
		{
        if(!aParam.HasValue())
            {
            return; // No value present. Value is optional.
            }
        // tag-value-list / string-value
        if(!SIPSyntaxCheck::FeatureValue(value))
			{
			User::Leave(KErrSipCodecAcceptContactHeader);
			}
		return;
		}

    // req-param
    if(name == SIPStrings::StringF(SipStrConsts::ERequire) ||
       name == SIPStrings::StringF(SipStrConsts::EExplicit))
        {
        return;
        }

    // generic-param
    CheckGenericParamL(aParam, KErrSipCodecAcceptContactHeader);
	}
