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

#ifndef CMSRPCONNLISTENER_H_
#define CMSRPCONNLISTENER_H_


#include "MMSRPListener.h"
#include "CMSRPConnectionManager.h"
#include "MMSRPTimeOutObserver.h"
#include "CMSRPTimeOutTimer.h"
#include "MMSRPConnection.h"

const TInt KListenQueueSize = 4;

class CMSRPConnListener:   public CActive, public MMSRPListener, public MMSRPTimeOutObserver
    {
    
public:
    
    //constructors and destructors
    static MMSRPListener* NewL(CMSRPConnectionManager& aConnectionMngr);
        
    virtual ~CMSRPConnListener();
    
    //MMsrpListener fns    
    
     virtual void ListenL();
     
     virtual void ListenCancel();
     
     virtual void ListenCompletedL(TBool aValid);
                                       
     //MMsrpTimeoutObserver fn
     virtual void TimerExpiredL();
     
protected: // from CActive
  
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);
             

private:
    CMSRPConnListener(CMSRPConnectionManager& aConnectionMngr);
    void ConstructL();
    void NotifyL(TListenerState aNewState);
    virtual void ActivateL();//start listening


private:
    TUint           iListenCount;
    RSocket*        iDataSocket;
    RSocket         iListenSocket;
    TListenerState  iState;
    CMSRPTimeOutTimer*  iTimer;
    CMSRPConnectionManager& iConnMngr;
    };



#endif /* CMSRPCONNLISTENER_H_ */
