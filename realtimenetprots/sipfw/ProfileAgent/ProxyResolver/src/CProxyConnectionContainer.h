/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CProxyConnectionContainer.h
* Part of       : ProxyResolver
* See class definition below.
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CPROXYCONNECTIONCONTAINER_H
#define CPROXYCONNECTIONCONTAINER_H

// INCLUDES
#include <in_sock.h>
#include <es_sock.h>
#include <commdbconnpref.h>

// FORWARD DECLARATIONS
class MConnObserver;

// CLASS DEFINITION
/**
 *
 */
class CProxyConnectionContainer
    : public CActive
    {
    public: // Constructors and destructor

        static CProxyConnectionContainer* NewL( MConnObserver& aObserver,
                                                TUint32 aIapId, 
                                                RSocketServ& aServer );
                                                
        static CProxyConnectionContainer* NewLC( MConnObserver& aObserver,
                                                 TUint32 aIapId, 
                                                 RSocketServ& aServer );

        /// Destructor
        ~CProxyConnectionContainer();
    
    public: // New functions

        // returns RConnection object
        RConnection& Connection();

    protected: // From CActive
    
        void DoCancel();
        void RunL();

    private: // Constructors
                                                
        void ConstructL();
        CProxyConnectionContainer( MConnObserver& aObserver,
                                   TUint32 aIapId, 
                                   RSocketServ& aServer );                                         
                                
    private: // Data

        MConnObserver& iObserver;         // Observer (CProxyQuery)
        RConnection iConnection;          // RConnection object
        TCommDbConnPref iPrefs;           // Connection preferences
        TUint32 iIapId;                   // current IAP id
        RSocketServ& iServer;             // socket server

    };

#endif  // CPROXYCONNECTIONCONTAINER_H
