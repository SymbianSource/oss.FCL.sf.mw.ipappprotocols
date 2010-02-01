/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipresolvedclient2.inl
* Part of       : SIP Client Resolver
* Version       : 2.0
*
*/



#ifndef SIPRESOLVEDCLIENT2_INL
#define SIPRESOLVEDCLIENT2_INL

#include <ecom/ecom.h>

// ---------------------------------------------------------------------------
// CSIPResolvedClient2::CSIPResolvedClient2 
// ---------------------------------------------------------------------------
//
inline CSIPResolvedClient2::CSIPResolvedClient2()
	{
	}

// ---------------------------------------------------------------------------
// CSIPResolvedClient2::~CSIPResolvedClient2 
// ---------------------------------------------------------------------------
//
inline CSIPResolvedClient2::~CSIPResolvedClient2()
	{
	REComSession::DestroyedImplementation( iInstanceKey );
	}	

#endif // SIPRESOLVEDCLIENT2_INL

// End of File
