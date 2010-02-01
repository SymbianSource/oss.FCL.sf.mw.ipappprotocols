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
* Name          : sipnattraversalcontroller.inl
* Part of       : SIP NAT Traversal Controller
* Version       : SIP/5.0 
*
*/



#ifndef CSIPNATTRAVERSALCONTROLLER_INL
#define CSIPNATTRAVERSALCONTROLLER_INL

// -----------------------------------------------------------------------------
// CSIPNATTraversalController::~CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
inline CSIPNATTraversalController::~CSIPNATTraversalController()
	{
	REComSession::DestroyedImplementation(iInstanceKey);
	}

// -----------------------------------------------------------------------------
// CSIPNATTraversalController::CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
inline CSIPNATTraversalController::CSIPNATTraversalController()
	{
	}
	
#endif // CSIPNATTRAVERSALCONTROLLER_INL