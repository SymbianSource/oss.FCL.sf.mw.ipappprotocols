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
// SIP Connection Provider factory class 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SIPCPR_FACTORY_H
#define SYMBIAN_SIPCPR_FACTORY_H

#include <comms-infras/ss_subconnprov.h>


class CSipConnectionProviderFactory : public ESock::CConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x10274C38 };
	static CSipConnectionProviderFactory* NewL(TAny* aParentContainer);
	~CSipConnectionProviderFactory();

protected:
	CSipConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	//virtual void DoEnumerateConnectionsL(RPointerArray<TConnectionInfoV2>& aConnectionInfoPtrArray);
	};

#endif
//SYMBIAN_SIPPCPR_FACTORY_H
