// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : TStateUnavailable.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TStateUnavailable.h"
#include "msipconnectioncontext.h"
#include "SipLogs.h"

// ----------------------------------------------------------------------------
// TStateUnavailable::Enter
// ----------------------------------------------------------------------------
//
void TStateUnavailable::Enter( TInt /*aError*/ )
	{
	
	__SIP_LOG( "TStateUnavailable::Enter" )
	iContext.Destroy();
	}
