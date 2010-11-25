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

// This file defines the API for MSRPClient.dll

#ifndef CMSRP_H_
#define CMSRP_H_

// @platform

//  Include Files

#include <e32base.h>
#include <e32std.h>


// Forward Declarations
class CMSRPSession;
class CMSRPImplementation;
class MMSRPSessionObserver;


// Class declaration.
class CMSRP : public CBase
    {
public:
    
    /** 
     * Object Creation
     * @param aUid the UID of the application that is set by the client
     */
    IMPORT_C static CMSRP* NewL( const TUid& aUid );
    
    /**
     * Object Creation
     * @param aUid the UID of the application that is set by the client
     */
    IMPORT_C static CMSRP* NewLC( const TUid& aUid );
    
    IMPORT_C ~CMSRP();

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
    IMPORT_C CMSRPSession* CreateMSRPSessionL( MMSRPSessionObserver& aObserver, 
                                               const TUint aIapId,
                                               const TDesC8& aSessionId );
    
    /** 
     * Closes the msrp session that the client had created.
     * All the resources associated with the session are freed
     * @param aMsrpSession the session that needs to be closed
     */
    IMPORT_C void CloseMSRPSession( CMSRPSession* aMsrpSession );

private:
    
    void ConstructL( const TUid& aUid );
    
private:
        
    CMSRPImplementation* iMSRPImpl;
    
    };

#endif  // CMSRP_H_

