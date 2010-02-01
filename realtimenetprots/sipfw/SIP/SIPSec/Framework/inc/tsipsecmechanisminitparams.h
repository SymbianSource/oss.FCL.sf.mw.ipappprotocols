/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : tsipsecmechanisminitparams.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_MECHANISM_INIT_PARAMS_H__
#define __SIPSEC_MECHANISM_INIT_PARAMS_H__

// INCLUDES
#include "Lwtimer.h"
#include "MSIPSecEngineContext.h"

// FORWARD DECLARATIONS
class MSIPTransportMgr;

// CLASS DECLARATION

class TSIPSecMechanismInitParams
	{
public: // Constructors and destructor
        
    /**
    * Constructor
	* @param aTimer Timer
	* 
    */
	TSIPSecMechanismInitParams(MTimerManager& aTimer,
							   MSIPSecEngineContext& aEngineContext,
							   MSIPTransportMgr& aTransportMgr,
							   TUint aT1);
public: // Data
	MTimerManager& iTimer;
	MSIPSecEngineContext& iEngineContext;
	MSIPTransportMgr& iTransportMgr;
	TUint iT1;
	};

#include "tsipsecmechanisminitparams.inl"

#endif
