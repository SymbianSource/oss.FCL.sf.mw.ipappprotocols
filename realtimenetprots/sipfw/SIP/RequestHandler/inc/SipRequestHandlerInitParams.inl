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
* Name        : SipRequestHandlerInitParams.inl
* Part of     : SIP Request Handler
* Version     : SIP/4.0
* TSIPRequestHandlerInitParams::TSIPRequestHandlerInitParams
*
*/



inline TSIPRequestHandlerInitParams::TSIPRequestHandlerInitParams
    (TUid aUid, MSIPRequestHandlerObserver* aObserver)
    : iUid(aUid),
      iObserver(aObserver)
	{
	}

// -----------------------------------------------------------------------------
// TSIPRequestHandlerInitParams::TSIPRequestHandlerInitParams
// -----------------------------------------------------------------------------
//
inline TSIPRequestHandlerInitParams::TSIPRequestHandlerInitParams ()
 : iObserver(NULL)
	{
	iUid.iUid = 0;
	}
