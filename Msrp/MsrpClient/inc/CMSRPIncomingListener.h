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

#ifndef CMSRPINCOMINGLISTENER_H_
#define CMSRPINCOMINGLISTENER_H_


//  INCLUDES

// FORWARD DECLARATIONS
class CMsrpListenerBase;
class CMSRPSessionImplementation;
class MMSRPServerInterface;



/**
*  The active object handler for incoming messages
*/
class CMSRPIncomingListener : public CMSRPListenerBase
    {
public:

    static CMSRPIncomingListener* NewL(
            CMSRPSessionImplementation& aSessionImpl,
            MMSRPServerInterface& aServerInterface );

    
    virtual ~CMSRPIncomingListener();
    
public:
    
    void ListenConnections( const TDesC8& aRemoteHost,
                            const TUint aRemotePort,
                            const TDesC8& aRemoteSessionID );
    
    void ListenMessages( );
    
protected: //From base classes
    
    // From CActive
    void RunL();
    void DoCancel();
    
private:
    
    CMSRPIncomingListener(
            CMSRPSessionImplementation& aSessionImpl,
            MMSRPServerInterface& aServerInterface );
        
private:
    
    // incoming connection or message
    TBool iIsMessage;

    };    


#endif /* CMSRPINCOMINGLISTENER_H_ */
