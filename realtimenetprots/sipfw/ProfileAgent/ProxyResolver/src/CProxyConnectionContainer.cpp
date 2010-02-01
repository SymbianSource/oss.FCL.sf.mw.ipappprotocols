// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : CProxyConnectionContainer.cpp
// Part of       : ProxyResolver
// Implementation
// Version       : SIP/4.0
//



#include "CProxyConnectionContainer.h"
#include "MConnObserver.h"


// -----------------------------------------------------------------------------
// CProxyConnectionContainer::NewL
// -----------------------------------------------------------------------------
//
CProxyConnectionContainer* 
CProxyConnectionContainer::NewL( MConnObserver& aObserver,
                                 TUint32 aIapId, 
                                 RSocketServ& aServer )
    {
    CProxyConnectionContainer* self = NewLC( aObserver, aIapId, aServer );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CProxyConnectionContainer::NewLC
// -----------------------------------------------------------------------------
//
CProxyConnectionContainer* 
CProxyConnectionContainer::NewLC( MConnObserver& aObserver,
                                  TUint32 aIapId, 
                                  RSocketServ& aServer )
    {
    CProxyConnectionContainer* self = 
        new( ELeave )CProxyConnectionContainer( aObserver, aIapId, aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CProxyConnectionContainer::CProxyConnectionContainer
// -----------------------------------------------------------------------------
//
CProxyConnectionContainer::CProxyConnectionContainer( MConnObserver& aObserver,
                                                      TUint32 aIapId, 
                                                      RSocketServ& aServer)
    : CActive( EPriorityStandard ),
      iObserver( aObserver ),
      iIapId(aIapId),
      iServer(aServer)
    {
    CActiveScheduler::Add( this );
    }    

// -----------------------------------------------------------------------------
// CProxyConnectionContainer::ConstructL
// -----------------------------------------------------------------------------
//
void CProxyConnectionContainer::ConstructL()
    {
    // open connection
    User::LeaveIfError( iConnection.Open( iServer ) );
        
    // Set connection preferences
    iPrefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
    iPrefs.SetDirection( ECommDbConnectionDirectionOutgoing );
    iPrefs.SetIapId( iIapId );

    // Start connecting
    iConnection.Start( iPrefs, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CProxyConnectionContainer::~CProxyConnectionContainer
// -----------------------------------------------------------------------------
//
CProxyConnectionContainer::~CProxyConnectionContainer()
    {
    Cancel();
    iConnection.Close();
    }

// -----------------------------------------------------------------------------
// CProxyConnectionContainer::Connection
// -----------------------------------------------------------------------------
//
RConnection& CProxyConnectionContainer::Connection() 
    {  
    return iConnection; 
    }

// -----------------------------------------------------------------------------
// CProxyConnectionContainer::RunL
// -----------------------------------------------------------------------------
//
void CProxyConnectionContainer::RunL()
    {
    TInt err = iStatus.Int();
    switch( err )
        {   
        case KErrNone: // if there is no error, Get the Dhcp options, 
                       // parse it and send it to proxy resolver owner
            {
            iObserver.ConnectionReady();
            }
            break;
        
        default: // other than KErrNone, KErrCancel, 
                 //send the err to proxy resolver owner
            {
            iObserver.ErrorOccured(err);
            }
            break;
        }   
    }

// -----------------------------------------------------------------------------
// CProxyConnectionContainer::DoCancel
// -----------------------------------------------------------------------------
//
void CProxyConnectionContainer::DoCancel()
    {
    iConnection.Close();   
    }
