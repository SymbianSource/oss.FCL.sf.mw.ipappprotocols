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

#ifndef CMSRPIMPLEMENTATION_H_
#define CMSRPIMPLEMENTATION_H_

// @internal

// System Includes
#include <e32base.h>

// Forward Declarations
class CMSRPSession;
class CMSRP;
class MMSRPSessionObserver;
class RMSRP;


class CMSRPImplementation : public CBase
    {
public:
    
    static CMSRPImplementation* NewL( const TUid& aUid );
    
    static CMSRPImplementation* NewLC( const TUid& aUid );
    
    ~CMSRPImplementation();
    
    /**
     * Creates an msrp session for the client
     * @param aObserver a reference to the session observer created by the client.
     *        This client-side observer receives callbacks from the msrp stack, in order to
     *        notify it of varous events
     * @param aIapId the IAP on which the connection has to start for this session
     * @param aSessionId session id of this MSRP session. This comes from the
     *        path SDP attribute where session id is used
     * @return the created session object which can be used by the client
     */
    CMSRPSession* CreateMSRPSessionL( MMSRPSessionObserver& aObserver, 
                                      const TUint aIapId,
                                      const TDesC8& aSessionId );
    
    void CloseMSRPSession( CMSRPSession* aMsrpSession );
        
private:
    
    CMSRPImplementation( const TUid& aUid );
    
    void ConstructL();
    
private:
    
    TUid iUid;
    
    RMSRP* iMSRP;
    
    //Array of MSRP sessions that it is serving
    RPointerArray<CMSRPSession> iMSRPSessionsArray;
        
    };
#endif /* CMSRPIMPLEMENTATION_H_ */
