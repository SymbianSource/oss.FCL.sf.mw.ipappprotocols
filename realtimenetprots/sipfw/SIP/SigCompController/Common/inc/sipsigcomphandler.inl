/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipsigcomphandler.inl
* Part of     : SIP SigComp Controller
* inline functions
* Version     : SIP 4.0
* CSigCompHandler::~CSigCompHandler
*
*/



inline CSigCompHandler::~CSigCompHandler()
	{
	REComSession::DestroyedImplementation(iInstanceKey);
	}

// -----------------------------------------------------------------------------
// CSigCompHandler::CSigCompHandler
// -----------------------------------------------------------------------------
//
inline CSigCompHandler::CSigCompHandler()
	{
	}