// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SipProfileAgentConfigRetriever.cpp
// Part of       : SIP / SIP Profile Server
//



#include <centralrepository.h>
#include "SipProfileAgentConfigRetriever.h"
#include <sipprofileservercrkeys.h>

#define KIETFDefaultExpiryValue 3600
#define KIMSDefaultExpiryValue 600000
// -----------------------------------------------------------------------------
// CSipProfileAgentConfigRetriever::NewL
// -----------------------------------------------------------------------------
//
CSipProfileAgentConfigRetriever* CSipProfileAgentConfigRetriever::NewL()
	{
	CSipProfileAgentConfigRetriever* self = new (ELeave) CSipProfileAgentConfigRetriever();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigRetriever::CSipProfileAgentConfigRetriever
// -----------------------------------------------------------------------------
//
CSipProfileAgentConfigRetriever::CSipProfileAgentConfigRetriever() :
	CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigRetriever::ConstructL
// -----------------------------------------------------------------------------
//
void CSipProfileAgentConfigRetriever::ConstructL()
	{
	iRepository = CRepository::NewL(KCRUidProfileSrv);		
	for(TUint32 i = 0; i < TSIPProfileTypeInfo::EMaxSIPProfileClass; i++)
		{
		for(TUint32 j = 0; j < CSipProfileAgentConfigExtension::EMaxProfileProperty; j++)
			{
			iProfilePropertyKeys[i][j] = 0;
			}
		iDefaultRegistrationValue[i] = 0;
		iDefaultSubscriptionValue[i] = 0;
		}
	ReadValuesL();
	ListenForChangesL();
	}

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigRetriever::~CSipProfileAgentConfigRetriever
// -----------------------------------------------------------------------------
//
CSipProfileAgentConfigRetriever::~CSipProfileAgentConfigRetriever()
	{
	Cancel();
	delete iRepository;
	}

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigRetriever::DoCancel
// -----------------------------------------------------------------------------
//
void CSipProfileAgentConfigRetriever::DoCancel()
	{
	if (iRepository)
		{
		iRepository->NotifyCancelAll();
		}
	}

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigRetriever::RunL
// -----------------------------------------------------------------------------
//
void CSipProfileAgentConfigRetriever::RunL()
	{

	// In S60, repository completes the notification request with value 1, even
	// though there is no error.
	if (iStatus.Int() >= KErrNone)
		{
		ReadValuesL();
		}

	ListenForChangesL();
	}

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigRetriever::RunError
// -----------------------------------------------------------------------------
//
TInt CSipProfileAgentConfigRetriever::RunError(TInt aError)
	{
	return (aError == KErrNoMemory) ? aError : KErrNone;
	}

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigRetriever::ExpiryValueL
// -----------------------------------------------------------------------------
//
TUint CSipProfileAgentConfigRetriever::ExpiryValueL(
		TSIPProfileTypeInfo::TSIPProfileClass aClass,
		CSipProfileAgentConfigExtension::TProfileProperty aProperty)
	{

	if(!(aClass < TSIPProfileTypeInfo::EMaxSIPProfileClass) || 
	    !(aProperty < CSipProfileAgentConfigExtension::EMaxProfileProperty))
		{ 
		return KIETFDefaultExpiryValue;
		}
	
	TUint32 Val = 0;
	
	switch(aProperty)
	    {
	    case CSipProfileAgentConfigExtension::EProfileRegistrationValue:
	        Val = iDefaultRegistrationValue[aClass];break;
		case CSipProfileAgentConfigExtension::EProfileSubscriptionValue:
		    Val = iDefaultSubscriptionValue[aClass];break;
	    }
	
	if(0 == Val)
	    {
	    if(TSIPProfileTypeInfo::EIms == aClass)
	        Val = KIMSDefaultExpiryValue;
	    else
	        Val = KIETFDefaultExpiryValue;
	    }
	return Val;
	}

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigRetriever::ReadValuesL
// If the values read from repository are invalid, use the previous values.
// -----------------------------------------------------------------------------
//
void CSipProfileAgentConfigRetriever::ReadValuesL()
	{
	RArray<TUint32> ProfilePropertiesTableKeys;
	TUint ProfileType(0);
	TUint Property(0);
	TInt PptVal(0);
	
	iRepository->FindL(KSIPProfilePropertiesTable,KSIPProfileTableMask,
			ProfilePropertiesTableKeys);
	TInt count = ProfilePropertiesTableKeys.Count();
	for(TUint16 i=0;i<count;i++)
		{
		ProfileType = (ProfilePropertiesTableKeys[i] & KSIPProfileTypeMask) >> 
				KSIPProfileTypeShift;
		Property = (ProfilePropertiesTableKeys[i] & KSIPProfilePropertyMask) >> 
				KSIPProfilePropertyShift;
		if((ProfileType-1) < TSIPProfileTypeInfo::EMaxSIPProfileClass &&
				(Property-1) < CSipProfileAgentConfigExtension::EMaxProfileProperty)
			{
			User::LeaveIfError(iRepository->Get(
					ProfilePropertiesTableKeys[i], PptVal));
			iProfilePropertyKeys[ProfileType-1][Property-1] = 
					ProfilePropertiesTableKeys[i];
			switch(Property-1)
			    {
			    case CSipProfileAgentConfigExtension::EProfileRegistrationValue:
			        iDefaultRegistrationValue[ProfileType-1] =  PptVal; break;
			    case CSipProfileAgentConfigExtension::EProfileSubscriptionValue:
			        iDefaultSubscriptionValue[ProfileType-1] =  PptVal; break;
			    }
			}
		}
	ProfilePropertiesTableKeys.Reset();
    }

// -----------------------------------------------------------------------------
// CSipProfileAgentConfigRetriever::ListenForChangesL
// -----------------------------------------------------------------------------
//
void CSipProfileAgentConfigRetriever::ListenForChangesL()
	{

	const TUint32 KDummyPartialKey = 0x0;
	//This mask means listening for all keys 
	const TUint32 KMaskIncludingAllKeys = 0x0;

	User::LeaveIfError(iRepository->NotifyRequest(KDummyPartialKey,
												  KMaskIncludingAllKeys,
												  iStatus));
	SetActive();
	}
