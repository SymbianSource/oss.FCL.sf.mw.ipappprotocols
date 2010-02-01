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
// SIP SubConnection Provider factory class
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SIPSCPR_FACTORY_H
#define SYMBIAN_SIPSCPR_FACTORY_H

#include <comms-infras/ss_subconnprov.h>

class CSipSubConnectionProviderFactory : public ESock::CSubConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x10274C17 };
	static CSipSubConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CSipSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif
//SYMBIAN_SIPSCPR_FACTORY_H
