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
// Name          : CSIPRegistrar.cpp
// Part of       : Registration
// Version       : SIP/4.0 
//



#include "CSIPRegistrar.h"
#include "CSIPRegistrarStore.h"
#include "TSIPRegistrationUtility.h"
#include "uricontainer.h"
#include "siprequest.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSIPRegistrar::NewLC
// -----------------------------------------------------------------------------
//
CSIPRegistrar* CSIPRegistrar::NewLC(const CURIContainer&  aRegistrar,
									const TDesC8&         aCallId,
									TUint                 aCSeqNumber, 
									const TRegistrationId aRegistrationId,
									CSIPRegistrarStore&   aRegistrarStore)
	{
    CSIPRegistrar* self = 
		new (ELeave) CSIPRegistrar(aCSeqNumber, aRegistrarStore);
	CleanupStack::PushL(self);
	self->ConstructL(aRegistrar, aCallId, aRegistrationId);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrar::CSIPRegistrar
// -----------------------------------------------------------------------------
//
CSIPRegistrar::CSIPRegistrar(TUint               aCSeqNumber, 
							 CSIPRegistrarStore& aRegistrarStore) 
 : iCSeqNumber     (aCSeqNumber),
   iRegistrarStore (aRegistrarStore)
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistrar::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRegistrar::ConstructL(const CURIContainer&  aRegistrar,
		                       const TDesC8&         aCallId,
							   const TRegistrationId aRegistrationId)
	{
	iRegistrar = CURIContainer::NewL(aRegistrar);
	iCallId    = aCallId.AllocL();
	User::LeaveIfError(iRegistrationIds.Append(aRegistrationId));
	}

// -----------------------------------------------------------------------------
// CSIPRegistrar::~CSIPRegistrar
// -----------------------------------------------------------------------------
//
CSIPRegistrar::~CSIPRegistrar()
	{
	delete iCallId;
	delete iRegistrar;
	iRegistrationIds.Reset();
	}

// -----------------------------------------------------------------------------
// CSIPRegistrar::Registrar
// -----------------------------------------------------------------------------
//
const CURIContainer& CSIPRegistrar::Registrar() const
	{
	return *iRegistrar;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrar::IncreaseCSeqNumber
// -----------------------------------------------------------------------------
//
void CSIPRegistrar::IncreaseCSeqNumber()
	{
	iCSeqNumber++;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrar::FillCallIdAndCSeqL
// -----------------------------------------------------------------------------
//
void CSIPRegistrar::FillCallIdAndCSeqL(CSIPRequest& aSIPRequest)
	{
	CSIPCallIDHeader* callId = CSIPCallIDHeader::DecodeL(*iCallId);
	CleanupStack::PushL(callId);
	// fill in callid header, and add it to request
	CSIPCallIDHeader* oldCallId = aSIPRequest.CallID();
	if (oldCallId)
		{
		aSIPRequest.ReplaceHeaderL(oldCallId, callId);
		}
	else // there are callId header in request
		{
		aSIPRequest.AddHeaderL(callId);
		}
	CleanupStack::Pop(callId);

	// increase the CSeqNumber by one
	IncreaseCSeqNumber();

    CSIPCSeqHeader* oldCSeqHeader = aSIPRequest.CSeq();
	if (oldCSeqHeader)
		{
		oldCSeqHeader->SetSeq(iCSeqNumber);
		}
	else
		{	
		// create a CSIPCSeqHeader header, and add it to request
		CSIPCSeqHeader* cseqHeader = CSIPCSeqHeader::NewLC(
			iCSeqNumber, SIPStrings::StringF(SipStrConsts::ERegister));
		aSIPRequest.AddHeaderL(cseqHeader);
		CleanupStack::Pop(cseqHeader);
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrar::AddRegistrationIdL
// -----------------------------------------------------------------------------
//
void CSIPRegistrar::AddRegistrationIdL(const TRegistrationId aRegistrationId)
	{
	User::LeaveIfError(iRegistrationIds.Append(aRegistrationId));
	}

// -----------------------------------------------------------------------------
// CSIPRegistrar::BindingRemoved
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrar::BindingRemoved(const TRegistrationId aRegistrationId)  
	{
	TInt idFound = FindRegistrationId(aRegistrationId);

	// there is no registration binding relate to this registrar,
	// remove the registrar
	if (idFound != KErrNotFound)
		{
		if (iRegistrationIds.Count() == 1)
			{
			// if registration is added to delete mgr succeed,
			// remove registration id
			TInt res = iRegistrarStore.RemoveRegistrar(this);
			if (res == KErrNone)
				{
				iRegistrationIds.Remove(idFound);
				return ETrue; 
				}
			else
				{
				return EFalse;
				}
			}
		else
			{
			iRegistrationIds.Remove(idFound);
			return ETrue;
			}
		}

	return EFalse;
	}  

// -----------------------------------------------------------------------------
// CSIPRegistrar::FindRegistrationId
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrar::FindRegistrationId (const TRegistrationId aRegistrationId)
	{
	TInt idFound = KErrNotFound;

	for (TInt i=iRegistrationIds.Count()-1; (i>=0 && idFound==KErrNotFound); i--)
		{
		if (iRegistrationIds[i] == aRegistrationId)
			{
			return i;
			}
		}
	return idFound;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrar::CSeqNumber
// -----------------------------------------------------------------------------
//
TUint CSIPRegistrar::CSeqNumber() const
	{
	return iCSeqNumber;
	}


