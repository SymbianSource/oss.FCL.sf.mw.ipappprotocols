// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : TTlsTransTlsConnecting.cpp
// Part of     : ConnectionMgr
// implementation
// Version     : SIP/5.0
//



#include "TTlsTransTlsConnecting.h"
#include "MTlsTransStateOwner.h"

// SIP layer waits 30 seconds for TLS Handshake to happen, if the timer
// fires and negotiation does not happen, it cancels the handshake assuming
// some problem while handshaking.
const TInt KConnectTLSTimeout = 30000; // 30 seconds

// -----------------------------------------------------------------------------
// TTlsTransTlsConnecting::EnterL
// -----------------------------------------------------------------------------
//
void TTlsTransTlsConnecting::EnterL()
    {
    //Check if CTransportTls has secure-socket. If yes, try to match
	//the certificate. Otherwise create secure-socket in CTransportTls Object
    if (iOwner.HasSecureSocket())
        {
        //Get Server Certificte Info and do the matching
        //If matching is successful, go into Connected State
        // and start receiving data, otherwise Disconnect
        iValidCertificate = iOwner.MatchCertificateL();  
        }
    else
        {                     
        iValidCertificate = EFalse;
        iOwner.ConnectTlsL();    	
        // Have to start guard timer since CSecureSocket doesn't have any
        // timeout mechanism.
        iOwner.StartTimerL( KConnectTLSTimeout );
        }
    }
    
// -----------------------------------------------------------------------------
// TTlsTransTlsConnecting::ConnectionOpenL
// -----------------------------------------------------------------------------
//
TBool TTlsTransTlsConnecting::ConnectionOpenL()
    {
    iOwner.ChangeStateL( TTlsTransStateBase::ETlsConnecting );
	//If the certificate is valid, move the state to connected
    if(iValidCertificate)
        {
        iOwner.ChangeStateL( TTlsTransStateBase::ETlsConnected );
        return ETrue;
        }
    else
        {
        //As the certificate does not match, destroy the TLS object and do
        //the necessary clean-up
        iOwner.Destroy();        
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// TTlsTransTlsConnecting::TTlsTransTlsConnecting
// -----------------------------------------------------------------------------
//
TTlsTransTlsConnecting::TTlsTransTlsConnecting( 
    MTlsTransStateOwner& aOwner ) : TTlsTransStateBase( aOwner )
    {
    }

// End of File
