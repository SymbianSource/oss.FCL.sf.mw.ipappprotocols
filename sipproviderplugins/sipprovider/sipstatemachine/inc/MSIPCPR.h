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
// MSIPCPR definition file.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SIPSCPR_SIPCPR_H
#define SIPSCPR_SIPCPR_H

#include <ss_subconnprov.h>
#include "transitionengine.h"


/**This interface cannot be defined neither in SIPCPR nor in SIPSCPR
because both SIPSCPR and SIPCPR must be able to link to it 
(link to IfSIPCPRTid - see below).
*/
const TUint KUidSIPInterfaces = 0x10274C38; //same as SIP CPR Factory ID
#define IfSIPCPRTid MSIPCPR::GetSTypeId()

class MSIPCPR
/** The interface SIPSCPR expects from its connection provider.


@internalComponent
@released since v9.2
*/
	{
public:	    
    static const Meta::STypeId GetSTypeId()
        {
        const Meta::STypeId id = Meta::STypeId::CreateSTypeId(KUidSIPInterfaces, 0);
        return id;
        }
        
  	/**
	Called by the SIPSCPR to obtain the TransitionEngine it requires to function.
	
	@param a pointer to the transition engine or NULL if error.
	*/	
    virtual CSIPTransitionEngine* GetTransitionEngine() = 0;
    
	/**
	Called by the SIPSCPR to obtain the MSubConnectionControlClient that is
	awaiting to pick up an incoming connection. 'this' should reset
	the held pointer so that the next time this method is called,
	it returns NULL (unless someone subscribe for the incoming request
	again).
	
	@param a pointer to the MSubConnectionControlClient or NULL if nobody's awaiting.
	*/	
    virtual MSubConnectionControlClient* ResetIncomingSubConClient() = 0;
    
	/**
	Called by the SIPSCPR to obtain the 'from' field associated with 'this'.
	
	@return a pointer to the buffer holding the 'from' field. The buffer
	must be valid throughout the lifetime of 'this'.
	@exception failure to retrieve the from 'field'.
	*/	
    virtual const TPtrC8 GetFromFieldL() = 0;
	};
	
	

#endif	// SIPSCPR_SIPCPR_H
