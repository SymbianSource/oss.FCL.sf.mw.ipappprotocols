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
* Name          : sipalrpluginloader.h
* Part of       : SIP / SIP Profile Server
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/


#ifndef SIPALRPLUGINGLOADER_H
#define SIPALRPLUGINGLOADER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSipAlrMonitor;

// CLASS DECLARATION
/**
* Factory class for loading SIP ALR Monitor.
*
* @lib sipsrofilesrv.exe
*/
class SipAlrPluginLoader
    {
    public: // New functions

        /**
        * Loads SIP ALR Monitor plug-in.
        * @param aDeltaTimer a handle to CDeltaTimer
        * @return new CSipAlrMonitor instance, the ownership is transferred.
        */    
        static CSipAlrMonitor* LoadMonitorL(CDeltaTimer& aDeltaTimer);
        
    private:
    
        static void ResetAndDestroy(TAny* anArray);
    };

#endif // SIPALRPLUGINGLOADER_H

// End of File
