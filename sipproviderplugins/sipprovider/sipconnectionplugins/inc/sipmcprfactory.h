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
// SIP Meta Connection Provider factory class 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SIPMCPR_FACTORY_H
#define SYMBIAN_SIPMCPR_FACTORY_H

#include <comms-infras/ss_metaconnprov.h>
#include <in_sock.h>

class CSipMetaConnectionProviderFactory : public ESock::CMetaConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x200041F9 };
	static CSipMetaConnectionProviderFactory* NewL(TAny* aParentContainer);
	
		
protected:
	CSipMetaConnectionProviderFactory(TUid aFactoryUid, ESock::CMetaConnectionFactoryContainer& aParentContainer);
	ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif
//SYMBIAN_SIPMCPR_FACTORY_H
