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

#ifndef CMSRPSENDRESULTLISTENER_H_
#define CMSRPSENDRESULTLISTENER_H_


// INCLUDES
#include "CMsrpListenerBase.h"


// FORWARD DECLARATIONS
class CMsrpListenerBase;
class CMSRPSessionImplementation;
class MMSRPServerInterface;


/**
*  The active object handler for listening to the result of a SendMessage
*/
class CMSRPSendResultListener : public CMSRPListenerBase
    {
public:
    
    static CMSRPSendResultListener* NewL(
                CMSRPSessionImplementation& aSessionImpl,
                MMSRPServerInterface& aServerInterface,
                const TDesC8& aSessionId );
    
    virtual ~CMSRPSendResultListener();
    
public:
    
    void ListenSendResultL( );
    
protected: //From base classes
    
    //From CActive
    void RunL();
    void DoCancel();
    
private:
    
    CMSRPSendResultListener( CMSRPSessionImplementation& aSessionImpl,
                             MMSRPServerInterface& aServerInterface,
                             const TDesC8& aSessionID );
    
private:
    
    TBuf8< KMaxLengthOfSessionId > iSessionID;

    };


#endif /* CMSRPSENDRESULTLISTENER_H_ */
