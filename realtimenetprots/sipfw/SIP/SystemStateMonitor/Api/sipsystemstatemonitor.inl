/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipsystemstatemonitor.inl
* Part of       : SIP / SIP Profile Agent
* Version       : SIP/6.0 
*
*/



#ifndef CSIPSYSTEMSTATEMONITOR_INL
#define CSIPSYSTEMSTATEMONITOR_INL

// -----------------------------------------------------------------------------
// CSipSystemStateMonitor::~CSipSystemStateMonitor
// -----------------------------------------------------------------------------
//
inline CSipSystemStateMonitor::~CSipSystemStateMonitor()
	{
	REComSession::DestroyedImplementation( iInstanceKey );
	}

// -----------------------------------------------------------------------------
// CSipSystemStateMonitor::CSipSystemStateMonitor
// -----------------------------------------------------------------------------
//
inline CSipSystemStateMonitor::CSipSystemStateMonitor()
	{
	}

// -----------------------------------------------------------------------------
// CSipSystemStateMonitor::StartMonitoringL
// -----------------------------------------------------------------------------
//	
inline void CSipSystemStateMonitor::StartMonitoringL( 
    TSystemVariable /*aVariable*/,
    TInt /*aObjectId*/,
    MSipSystemStateObserver& /*aObserver*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitor::StopMonitoring
// -----------------------------------------------------------------------------
//
inline void CSipSystemStateMonitor::StopMonitoring( 
    TSystemVariable /*aVariable*/, 
    TInt /*aObjectId*/,
    MSipSystemStateObserver& /*aObserver*/ )
    {
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitor::CurrentValue
// -----------------------------------------------------------------------------
//
inline TInt CSipSystemStateMonitor::CurrentValue( 
    TSystemVariable /*aVariable*/,
    TInt /*aObjectId*/ ) const
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CSipSystemStateMonitor::EventProcessingCompleted
// -----------------------------------------------------------------------------
//
inline void CSipSystemStateMonitor::EventProcessingCompleted( 
        TSystemVariable /*aVariable*/,
        TInt /*aObjectId*/,
        MSipSystemStateObserver& /*aObserver*/)
    {
    
    }
#endif // CSIPSYSTEMSTATEMONITOR_INL