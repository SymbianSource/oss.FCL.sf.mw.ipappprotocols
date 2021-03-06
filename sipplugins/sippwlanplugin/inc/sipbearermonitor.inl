/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef CSIPBEARERMONITOR_INL
#define CSIPBEARERMONITOR_INL


// ---------------------------------------------------------------------------
// TSIPBearerParams::TSIPBearerParams
// ---------------------------------------------------------------------------
//
inline TSIPBearerParams::TSIPBearerParams( 
    MSIPBearerOwner& aOwner,
    TInt aIapId,
    TInt aError,
    MSIPNetworkInfoObserver& aNetworkInfoObserver ) :
    iOwner( aOwner ),
    iIapId( aIapId ),
    iError( aError ),
    iNetworkInfoObserver( aNetworkInfoObserver )
    {
    }

// ---------------------------------------------------------------------------
// CSIPBearerMonitor::~CSIPBearerMonitor
// ---------------------------------------------------------------------------
//
inline CSIPBearerMonitor::~CSIPBearerMonitor()
    {
	REComSession::DestroyedImplementation( iInstanceKey );
	}

// ---------------------------------------------------------------------------
// CSIPBearerMonitor::State
// ---------------------------------------------------------------------------
//
inline MSIPNetworkObserver::TNetworkState CSIPBearerMonitor::State() const
    {
    return iState;
    }

// ---------------------------------------------------------------------------
// CSIPBearerMonitor::RefreshL
// ---------------------------------------------------------------------------
//			
inline void CSIPBearerMonitor::RefreshL( TInt /*aError*/ )
    {
	}

// ---------------------------------------------------------------------------
// CSIPBearerMonitor::ContinueMonitoring
// By default, all other error codes than KErrNone lead to negative result
// ---------------------------------------------------------------------------
//	
inline TBool CSIPBearerMonitor::ContinueMonitoring( TInt aError )
    {    
    return aError == KErrNone;
    }

// ---------------------------------------------------------------------------
// CSIPBearerMonitor::CSIPBearerMonitor
// ---------------------------------------------------------------------------
//
inline CSIPBearerMonitor::CSIPBearerMonitor( TAny* aParams ) :
    iParent( reinterpret_cast< TSIPBearerParams* >( aParams )->iOwner ),
    iIapId( reinterpret_cast< TSIPBearerParams* >( aParams )->iIapId ),
	iError( reinterpret_cast< TSIPBearerParams* >( aParams )->iError ),
	iNetworkInfoObserver( reinterpret_cast< TSIPBearerParams* >
		( aParams )->iNetworkInfoObserver )
	{
	}

#endif // CSIPBEARERMONITOR_INL

// End of File

