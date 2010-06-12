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

#ifndef CMSRPCONNECTIONLISTENER_H_
#define CMSRPCONNECTIONLISTENER_H_


//  INCLUDES
#include "CMSRPListenerBase.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CMsrpListenerBase;
class CMSRPSessionImplementation;
class MMSRPServerInterface;


// CLASS DECLARATION

/*
 * The active object for all connection related events
 */

class CMSRPConnectionListener : public CMSRPListenerBase
    {
public:
    
    static CMSRPConnectionListener* NewL(CMSRPSessionImplementation& aSessionImpl,
                                         MMSRPServerInterface& aServerInterface);
    
    virtual ~CMSRPConnectionListener();
    
public:
    
    void ConnectL( const TDesC8& aRemoteHost,
                   const TUint aRemotePort,
                   const TDesC8& aRemoteSessionID );
  
protected: //from base class
    
    // CActive
    void RunL();
    void DoCancel();
    
private:
    CMSRPConnectionListener( CMSRPSessionImplementation& aSessionImpl, 
                             MMSRPServerInterface& aServerInterface );
    
    };

#endif /* CMSRPCONNECTIONLISTENER_H_ */
