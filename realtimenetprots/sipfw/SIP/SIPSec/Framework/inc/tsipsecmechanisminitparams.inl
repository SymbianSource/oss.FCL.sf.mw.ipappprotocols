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
* Name          : tsipsecmechanisminitparams.inl
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/



inline TSIPSecMechanismInitParams::TSIPSecMechanismInitParams(
	MTimerManager& aTimer,
	MSIPSecEngineContext& aEngineContext,
	MSIPTransportMgr& aTransportMgr,
	TUint aT1) :
	iTimer(aTimer),
 	iEngineContext(aEngineContext),
 	iTransportMgr(aTransportMgr),
 	iT1(aT1)
	{
	}