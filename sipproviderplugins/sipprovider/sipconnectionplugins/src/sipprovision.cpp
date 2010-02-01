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
// SIP ProvisionInfo. Encompasses the provision info that will be sent to SIP CPR
// 
//

/**
 @file
 @internalComponent
*/

#include "sipprovisioninfo.h"
#include <comms-infras/metatype.h>

using namespace Meta;

START_ATTRIBUTE_TABLE(TSipMcprProvisionInfo, TSipMcprProvisionInfo::EUid, TSipMcprProvisionInfo::ETypeId)
	REGISTER_ATTRIBUTE(TSipMcprProvisionInfo, iProfileId, TMetaNumber)
	REGISTER_ATTRIBUTE(TSipMcprProvisionInfo, iAppUid, TMeta<TUid>)
	REGISTER_ATTRIBUTE(TSipMcprProvisionInfo, iTransitionEngineMgr, TMeta<CTransitionEngineMgr*>)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE(TSipCprProvisionInfo, TSipCprProvisionInfo::EUid, TSipCprProvisionInfo::ETypeId)
	REGISTER_ATTRIBUTE(TSipCprProvisionInfo, iTransitionEngine, TMeta<CSIPTransitionEngine*>)	
	REGISTER_ATTRIBUTE(TSipCprProvisionInfo, iFromField, TMeta<TDesC8*>)
END_ATTRIBUTE_TABLE()
