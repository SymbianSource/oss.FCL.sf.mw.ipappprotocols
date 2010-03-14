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
// SIP provision info class for both MCPR and CPR
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SIPPROVISIONINFO_H
#define SYMBIAN_SIPPROVISIONINFO_H

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/metadata.h>
#include <es_enum.h>
#include "TransitionEngineMgr.h"

class TSipMcprProvisionInfo : public Meta::SMetaData 
/** 
@internalTechnology
*/
	{
public:
    //use the SipMCpr Factory Id as the id
    enum {ETypeId = 0, EUid = 0x200041F9};
    
    TSipMcprProvisionInfo()
    : iProfileId(0)
    	{
    	
    	}
    // There is nothing to free memory for this guy.
    // iTransitionEngineMgr will be freed by MCPR.
    ~TSipMcprProvisionInfo()
    {    	
    }
	DATA_VTABLE
	
	TUint32	iProfileId;
	TUid	iAppUid;
	CTransitionEngineMgr*	iTransitionEngineMgr;
	};
	
	
class TSipCprProvisionInfo : public Meta::SMetaData
/** 
@internalTechnology
*/
	{
public:
    //use the SipCpr Factory Id as the id
    enum {ETypeId = 0, EUid = 0x10274C38};
    
    TSipCprProvisionInfo()
    : iTransitionEngine(NULL)
    	{
    	
    	}   
    // No need to free the memory because the membres will be freed by TransitionEngine.
    ~TSipCprProvisionInfo()
    {    	
    }
	DATA_VTABLE
	CSIPTransitionEngine*	iTransitionEngine;
	TDesC8*					iFromField;	
	};	

#endif