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
* Name          : SipLogs.h
* Part of       : Logging
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef SIPLOGS_H
#define SIPLOGS_H

#include <e32std.h>
#include <in_sock.h>

#ifdef _DEBUG
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TSIPLoggerTraces.h"
// Comment the following line to turn off all kind of logging:
#define USE_SIP_LOGS
// Comment the following line to switch off SIP message logging
#define USE_SIP_MESSAGE_LOG
#endif
#endif


#ifdef USE_SIP_LOGS

    #include "TSIPLogger.h"
    
    #define __SIP_LOG(aStr)\
        TSIPLogger::Print(_L(aStr));

    #define __SIP_TXT8_LOG(aStr)\
        TSIPLogger::Print(aStr);
   
    #define __SIP_DES8_LOG(aStr,aMsg)\
        TSIPLogger::Print(_L(aStr), aMsg);    

    #define __SIP_DES16_LOG(aStr,aMsg)\
        TSIPLogger::Print(_L(aStr), aMsg); 

    #define __SIP_INT_LOG1(aStr,aValue)\
        TSIPLogger::Print(_L(aStr), aValue); 
 
    #define __SIP_INT_LOG2(aStr,aValue1,aValue2)\
        TSIPLogger::Print(_L(aStr), aValue1, aValue2);
        
    #define __SIP_ADDR_LOG(aStr,aAddr)\
        TSIPLogger::Print(_L(aStr), aAddr);
        
    #ifdef USE_SIP_MESSAGE_LOG
        #include "TSIPMessageLogger.h"
        #define __SIP_MESSAGE_LOG(aSubsystemName, aMessage)\
            TSIPMessageLogger::Write(_L8(aSubsystemName),aMessage);
    #else
        #define __SIP_MESSAGE_LOG(aSubsystemName,aSIPMessage)
    #endif // USE_SIP_MESSAGE_LOG

#else // _USE_SIP_LOGS

    #define __SIP_LOG(aStr)
    #define __SIP_TXT8_LOG(aStr)
    #define __SIP_DES8_LOG(aStr,aMsg)
    #define __SIP_DES16_LOG(aStr,aMsg)
    #define __SIP_INT_LOG1(aStr,aValue)
    #define __SIP_INT_LOG2(aStr,aValue1,aValue2)
    #define __SIP_ADDR_LOG(aStr,aAddr)
    #define __SIP_MESSAGE_LOG(aSubsystemName,aSIPMessage)

#endif // USE_SIP_LOGS


#endif // SIPLOGS_H

// End of File
