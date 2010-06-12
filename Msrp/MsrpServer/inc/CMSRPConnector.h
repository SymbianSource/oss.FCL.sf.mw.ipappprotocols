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

#ifndef CMSRPCONNECTOR_H
#define CMSRPCONNECTOR_H

// INTERNAL INCLUDES
#include "MMSRPConnector.h"
#include "MMSRPTimeOutObserver.h"
#include "CMSRPTimeOutTimer.h"
#include "MMSRPConnectionManager.h"
#include "MMSRPConnection.h"


// CLASS DECLARATION

/**
*  MSRP Connection Manager
*/
class CMSRPConnector : public CActive, public MMSRPConnector, public MMSRPTimeOutObserver
    {
 public:

     // Constructors and destructor
     static MMSRPConnector* NewL(MMSRPConnectionManager& aConnMngr, MMSRPConnectorObserver& aConnection);
     virtual ~CMSRPConnector();
     
     //from MMSRPConnector        
     void Connect(TInetAddr& aRemoteAddr);          
     void ConnectComplete();
     
     //from MMSRPTimeOutObserver
     void TimerExpiredL();
         
     
 protected: // from CActive
     void DoCancel();
     void RunL();
     TInt RunError(TInt aError);
    
 private:
     CMSRPConnector(MMSRPConnectionManager& aConnMngr, MMSRPConnectorObserver& aConnection);
     void ConstructL();

 private: // data
     CMSRPTimeOutTimer*  iTimer;
     MMSRPConnectionManager& iConnMngr; 
     MMSRPConnectorObserver& iConnection;  
     RSocket* iSocket;
     
     
    };

#endif      // CMSRPCONNECTOR_H

// End of File
