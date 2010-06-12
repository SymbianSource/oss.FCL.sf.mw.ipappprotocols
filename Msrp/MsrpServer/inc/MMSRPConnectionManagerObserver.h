/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef __MMMSRPCONNECTIONMANAGEROBSERVER_H
#define __MMMSRPCONNECTIONMANAGEROBSERVER_H

#include <in_sock.h>
#include <e32base.h>
#include "MMSRPConnection.h"
#include "MMSRPConnectionObserver.h"

class MMSRPConnection;
// CLASS DECLARATIONS
class MMSRPConnectionManagerObserver
	{
	
	public:
	    
	    enum TMSRPConnManagerErrors
	         {
	         ESocketServerStartFailed = -110,
	         EInterfaceStartFailed,
	         ELocalAddressResolveFailed,
	         EAddressResolveError, 
	         EListenerStartError,
	         EListenPortFailed
	         };
	    
        
	    /**
        * Get local ip address to form msrp path
        * @out param aLocalAddress 
        */	    
	    //socket server, connection, listen port failure
	    virtual void HandleError(TInt aError, TInt aStatus, MMSRPConnection& aConnection) = 0;
	    
	    //if listener fails, change listen port and restart listener
	    
	};

#endif // __MMMSRPCONNECTIONMANAGER_H

// End of file
