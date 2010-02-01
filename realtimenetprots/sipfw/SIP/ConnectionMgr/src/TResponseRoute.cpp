// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : TResponseRoute.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "TResponseRoute.h"

// -----------------------------------------------------------------------------
// TResponseRoute::TResponseRoute
// -----------------------------------------------------------------------------
//
TResponseRoute::TResponseRoute(TTransactionId& aTAId, const TInetAddr& aAddr) :
	iTAId(aTAId),
	iAddr(aAddr)
	{
	}

// -----------------------------------------------------------------------------
// TResponseRoute::Address
// -----------------------------------------------------------------------------
//
TInetAddr& TResponseRoute::Address()
	{
	return iAddr;
	}

// -----------------------------------------------------------------------------
// TResponseRoute::TAId
// -----------------------------------------------------------------------------
//
TTransactionId& TResponseRoute::TAId()
	{
	return iTAId;
	}
