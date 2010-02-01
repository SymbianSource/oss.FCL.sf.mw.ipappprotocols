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
// RTP Pint Factory Implementation
// 
//

/**
 @file
 @internalComponent
*/



#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "rtppint.h"
#include "rtpflowfactory.h"	

using namespace ESock;



// =====================================================================================
// Rtp Protocol-Interface Factory

CRtpProtocolIntfFactory* CRtpProtocolIntfFactory::NewL(TUid aUid, ESock::CProtocolIntfFactoryContainer& aParentContainer)
/**
Constructs a Rtp ProtocolIntf Factory
@param aConstructionParameters construction data passed by ECOM
@return pointer to a constructed factory
*/
	{
	// Note: Architecture requires that Flow and Protocol-Interface factory be registered
	// under the same UID in the factory construction but retain seperate UIDs in the RSS file.
	// Hence the use of Flow Implementation UID here rather than Protocol-Interface
	// Implementation UID.
	CRtpProtocolIntfFactory* ptr = new (ELeave) CRtpProtocolIntfFactory(aUid, aParentContainer);
	CleanupStack::PushL(ptr);
	ptr->ConstructL();
	CleanupStack::Pop();
	return ptr;
	}

CRtpProtocolIntfFactory::CRtpProtocolIntfFactory(TUid aFactoryId, CProtocolIntfFactoryContainer& aParentContainer)
	: CProtocolIntfFactoryBase(aFactoryId, aParentContainer)
/**
Rtp ProtocolIntf Factory Constructor

@param aFactoryId ECOM Implementation Id
@param aParentContainer Object Owner
*/
	{
	}

CProtocolIntfBase* CRtpProtocolIntfFactory::DoCreateProtocolIntfL(TFactoryQueryBase& aQuery)
	{
	const TDefaultFlowFactoryQuery& query  = static_cast < const TDefaultFlowFactoryQuery& > (aQuery);
	return new (ELeave) CRtpProtocolIntf(*this,query.iCprId);
	}
