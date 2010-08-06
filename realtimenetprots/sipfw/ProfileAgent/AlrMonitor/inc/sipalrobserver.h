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
* Name          : sipalrobserver.h
* Part of       : SIP / SIP Profile Agent / SIP ALR Monitor
* Version       : SIP/6.0 
*
*/



#ifndef MSIPALROBSERVER_H
#define MSIPALROBSERVER_H

// INCLUDES
#include <e32std.h>

// CONSTANTS
const TUint32 KNoIap = 0;

// CLASS DECLARATION
/**
* Callback to receive notifications about IAP availabilities.
*/
class MSipAlrObserver
	{
	public:

        /** ALR events */
        enum TEvent
            {
             /** 
            * A new IAP became available.
            * When receiving this event the client must call 
            * CSipAlrMonitor::AllowMigration or 
            * CSipAlrMonitor::DisallowMigration.
            */           
            EIapAvailable = 0,

            /** 
            * A new IAP that became available is now active 
            * and the client can start migrating to it.
            */
            EIapActive = 1,
            
            /**
            * No new IAP available in the used SNAP.
            */
            ENoNewIapAvailable = 2 ,
            
			/**
            * Refreshing IAPs failed
            */
            ERefreshError = 3,
            
			/**
            * The user has rejected offerred IAP
            */
            EOfferedIapRejected = 4
            };
                                   
        /**
        * ALR related event occurred.
        * @param aEvent the event that occurred
        * @param aSnapId the SNAP related to the event
        * @param aIapId the IAP related to the event
        */
        virtual void AlrEvent( TEvent aEvent,
                               TUint32 aSnapId,
                               TUint32 aIapId ) = 0;

	};

// CLASS DECLARATION
/**
* This is the class that encapsulates all snap specific data.
*/

class TSipSNAPConfigurationData  
    {
public:
    
    TSipSNAPConfigurationData(TUint32 aSnapId , TUint32 aBearerId)
        {
        iSnapId = aSnapId;
        iBearerId = aBearerId;
        };
    
   TUint32 iSnapId;
   TBool iBearerId;
    };
#endif // MSIPALROBSERVER_H
