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

#ifndef MMMSRPCONNECTOR_H
#define MMMSRPCONNECTOR_H

#include <e32base.h>
#include <in_sock.h>
#include "MMSRPConnectorObserver.h"

// CLASS DECLARATIONS
/**
* Interface for connector requests 
*/
class MMSRPConnector 
    {
    public: 
        inline virtual ~MMSRPConnector(){}
             
        virtual void Connect(TInetAddr& aRemoteAddr) = 0;
        
        virtual void ConnectComplete() = 0;
        
        //virtual void CancelConnect() = 0;               
        
        //close connection
       // virtual void Disconnect() = 0;
        
    };

#endif // MMMSRPCONNECTOR_H

// End of file
