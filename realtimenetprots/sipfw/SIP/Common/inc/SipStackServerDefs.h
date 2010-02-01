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
* Name          : SipStackServerDefs.h
* Part of       : SIPCommon
* Version       : SIP/3.0 
*
*/




/**
 @internalComponent
*/


#ifndef __SIP_STACK_SERVER_DEFS__
#define __SIP_STACK_SERVER_DEFS__

#include <e32std.h>
#include "sipclientserver.h"

// Contains all the "global" definitions visible on the C/S-server side.

typedef TUint32 TTransactionId;
const TTransactionId KEmptyTransactionId=0;
const TTransactionId KMinTransactionId=1;
const TTransactionId KMaxTransactionId=KMaxTUint32;

typedef TUint32 TRegistrationId;
const TRegistrationId KEmptyRegistrationId=0;
const TRegistrationId KMinRegistrationId=1;
const TRegistrationId KMaxRegistrationId=KMaxTUint32;

typedef TUint32 TRefreshId;
const TRefreshId KEmptyRefreshId=0;
const TRefreshId KMinRefreshId=1;
const TRefreshId KMaxRefreshId=KMaxTUint32;

typedef TUint32 TDialogId;
const TDialogId KEmptyDialogId=0;
const TDialogId KMinDialogId=1;
const TDialogId KMaxDialogId=KMaxTUint32;

#endif // end of __SIP_STACK_SERVER_DEFS__

// End of File
