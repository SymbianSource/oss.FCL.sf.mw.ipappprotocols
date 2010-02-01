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
* Name          : timerlog.h
* Part of       : LightWeightTimer
* Private header file for the Lightweight timer subsystem.
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef TIMERLOG_H
#define TIMERLOG_H

// INCLUDES
#include "SipLogs.h"


// MACROS
#if defined(USE_SIP_LOGS)

//Define this macro to turn on LightweightTimer's logs. They are written to
//the same file as SIP stack's other logs.
//#define WRITE_TIMER_LOG

#endif // USE_SIP_LOGS


#endif // TIMERLOG_H

// End of File
