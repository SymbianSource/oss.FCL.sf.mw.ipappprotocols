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
// Name          : CIpSecPolicyHandler.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include <utf.h>
#include <convutils.h>
#include <lib_pfkey.h>

#include "CIpSecPolicyHandler.h"
#include "MSipSecPolicyObserver.h"
#include "SipSecIpsecParams.h"

#ifndef CPPUNIT_TEST
// Comment this definition off, if ipsec is wanted to be used for real
//#define DISABLE_IPSEC_POLICY_USAGE
#endif

// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::NewL
// ----------------------------------------------------------------------------
//
CIpSecPolicyHandler* CIpSecPolicyHandler::NewL(
    MSipSecPolicyObserver& aObserver,
	RIpsecPolicyServ& aPolicyServ )
	{
	CIpSecPolicyHandler* 
		self = CIpSecPolicyHandler::NewLC( aObserver, aPolicyServ );
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::NewLC
// ----------------------------------------------------------------------------
//
CIpSecPolicyHandler* CIpSecPolicyHandler::NewLC(
    MSipSecPolicyObserver& aObserver,
	RIpsecPolicyServ& aPolicyServ )
	{
	CIpSecPolicyHandler* 
		self = new ( ELeave ) CIpSecPolicyHandler( aObserver, aPolicyServ );
	CleanupStack::PushL( self );
	return self;
	}

	
// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::~CIpSecPolicyHandler
// ----------------------------------------------------------------------------
//
CIpSecPolicyHandler::~CIpSecPolicyHandler()
	{
	Cancel();
	delete iPolicyData;
	}
	
// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::CreatePolicyL
// ----------------------------------------------------------------------------
//
void CIpSecPolicyHandler::CreatePolicyL( 
    const TInetAddr& aLocalAddress,
	const TInetAddr& aRemoteAddress,
	const TSipSecSaParams& aSaParams )
    {
    if ( IsActive() && iHandlerState != EIdle )
        {
        User::Leave( KErrInUse );
        }
        
    HBufC8* localAddr = GetAddrAsTextLC( aLocalAddress );
    TPtr8 plocalAddr( localAddr->Des() );
    HBufC8* remoteAddr = GetAddrAsTextLC( aRemoteAddress );
    TPtr8 premoteAddr( remoteAddr->Des() );
    
    HBufC8* mask = 0;
    if ( !(aLocalAddress.Address()))
        {
        _LIT8( KV6Mask, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ); 
        mask = KV6Mask().AllocLC();
        }
    else
        {
        _LIT8( KV4Mask, "255.255.255.255" );
        mask = KV4Mask().AllocLC();
        }
    TPtr8 pmask( mask->Des() );
        
    RemovePolicyData();
    
    const TInt KMAxTUint32AsDescLen = 10;

    _LIT8( KPolicyFormat, 
"SECURITY_FILE_VERSION: 3\r\n\
[INFO]\r\n\
SIP IPSec policies\r\n\
[POLICY]\r\n\
sa sip_sas = {\r\n\
esp\r\n\
encrypt_alg 11\r\n\
auth_alg %u\r\n\
src_specific\r\n\
local_port_specific\r\n\
remote_port_specific\r\n\
}\r\n\
local %S %S local_port %u remote %S %S remote_port %u = { sip_sas() }\r\n\
local %S %S local_port %u remote %S %S remote_port %u = { sip_sas() }\r\n\
local %S %S local_port %u = drop\r\n\
inbound = {}\r\n\
outbound = {}\r\n" );

    iPolicyData = HBufC8::NewL( KPolicyFormat().Length() +
                                6 * KMAxTUint32AsDescLen +
                                2 * premoteAddr.Length() +
                                5 * pmask.Length() + 
                                3 * plocalAddr.Length() );
                                
    TPtr8 pdata( iPolicyData->Des() );
    
    // Form policy data. Drop policy for the port_us cannot be created
    // since it will affect to other SAs using the same protected server port.
    // Instead, dropping of packets coming to port_us from incorrect remote
    // addresses is handled in ConnectionMgr level.
    pdata.AppendFormat( KPolicyFormat, 
                        aSaParams.iAuthAlg,
                        &plocalAddr,
                        &pmask,
                        aSaParams.iPort_us,
                        &premoteAddr,
                        &pmask,
                        aSaParams.iPort_pc,
                        &plocalAddr,
                        &pmask,
                        aSaParams.iPort_uc,
                        &premoteAddr,
                        &pmask,
                        aSaParams.iPort_ps,
                        &plocalAddr,
                        &pmask,
                        aSaParams.iPort_uc );
                        
    CleanupStack::PopAndDestroy( mask );
    CleanupStack::PopAndDestroy( remoteAddr );                  
    CleanupStack::PopAndDestroy( localAddr );
    
#ifdef DISABLE_IPSEC_POLICY_USAGE
    // Dummy completion of the loading request
	iStatus = KRequestPending;
	SetActive();
	iHandlerState = ELoading;
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
#else
    // Start loading the policy                    
    iPolicyServ.LoadPolicy( *iPolicyData, iPolicyHandle, iStatus );
    iHandlerState = ELoading;
    SetActive();
#endif                        
    }

// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::RemovePolicy
// ----------------------------------------------------------------------------
//
void CIpSecPolicyHandler::RemovePolicy()
    {
    // Cancel first outstanding operations
    Cancel();
   
#ifdef DISABLE_IPSEC_POLICY_USAGE
    // Dummy completion of the unloading request
	iStatus = KRequestPending;
	SetActive();
	iHandlerState = EUnloading;
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
#else 
    // Start unloading
    iPolicyServ.UnloadPolicy( iPolicyHandle(), iStatus );
    iHandlerState = EUnloading;
    SetActive();
#endif
    }

// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::RunL
// ----------------------------------------------------------------------------
//
void CIpSecPolicyHandler::RunL()
	{
	TInt err( iStatus.Int() );
	
	if (  err != KErrNone )
	    {
	    RemovePolicyData();
	    NotifyError( err );
	    return;
	    }
	    
	switch ( iHandlerState )
	    {
	    case ELoading:
	        {
	        // Policy data is not needed anymore
	        RemovePolicyData();
	        
#ifdef DISABLE_IPSEC_POLICY_USAGE
            // Dummy completion of the activating request
			iStatus = KRequestPending;
			SetActive();
			iHandlerState = EActivating;
			TRequestStatus* status = &iStatus;
			User::RequestComplete( status, KErrNone );
#else
	        // Start activating the policy
	        iPolicyServ.ActivatePolicy( iPolicyHandle(), iStatus );
	        iHandlerState = EActivating;
	        SetActive();
#endif
	        break;
	        }
	    case EActivating:
	        {
	        // Inform observer, all done
	        iHandlerState = EActive;
	        NotifyActiveL();
	        break;
	        }
	    case EUnloading:
	        {
	        // Inform observer
	        iHandlerState = EIdle;
	        NotifyUnload();
	        break;
	        }
	    default:
	        {
	        break;
	        }
	    }
	}
	
// -----------------------------------------------------------------------------
// CIpSecPolicyHandler::RunError
// -----------------------------------------------------------------------------
//
TInt CIpSecPolicyHandler::RunError( TInt aError )
	{
	if ( aError != KErrNoMemory )
		{
		return KErrNone;
		}
	return aError;
	}

// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::DoCancel
// ----------------------------------------------------------------------------
//
void CIpSecPolicyHandler::DoCancel()
	{
	switch ( iHandlerState )
	    {
	    case ELoading:
	        {
            iPolicyServ.CancelLoad();
            break;
	        }
	    case EActivating:
	        {
	        iPolicyServ.CancelActivate();
	        break;
	        }
	    case EUnloading:
	        {
	        iPolicyServ.CancelUnload();
	        break;
	        }
	    default:
	        {
	        break;
	        }
	    }
	}
	
// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::CIpSecPolicyHandler
// ----------------------------------------------------------------------------
//
CIpSecPolicyHandler::CIpSecPolicyHandler( 
    MSipSecPolicyObserver& aObserver,
    RIpsecPolicyServ& aPolicyServ ) :
	CActive( EPriorityStandard ),
	iObserver( aObserver ),
	iPolicyServ( aPolicyServ ),
	iHandlerState( EIdle )
	{
	CActiveScheduler::Add( this );
	}

// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::GetAddrAsTextLC
// ----------------------------------------------------------------------------
//	
HBufC8* CIpSecPolicyHandler::GetAddrAsTextLC( const TInetAddr& aAddr )
    {
    const TInt KMaxInetAddrLen = 256;
    TBuf<KMaxInetAddrLen> tempAddr;
    aAddr.Output( tempAddr );
    
    HBufC8* address = HBufC8::NewLC( tempAddr.Length() );
    TPtr8 paddress( address->Des() );  
    TInt ret = 
        CnvUtfConverter::ConvertFromUnicodeToUtf8( paddress, tempAddr );
    
    if ( ret > 0 || ret == CCnvCharacterSetConverter::EErrorIllFormedInput )
        {
        User::Leave( KErrGeneral );
        }
    
    return address;
    }

// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::RemovePolicyData
// ----------------------------------------------------------------------------
//		
void CIpSecPolicyHandler::RemovePolicyData()
    {
    delete iPolicyData;
    iPolicyData = 0;
    }

// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::NotifyActiveL
// ----------------------------------------------------------------------------
//    
void CIpSecPolicyHandler::NotifyActiveL()
    {
	iObserver.PolicyActivatedL();
    }

// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::NotifyUnload
// ----------------------------------------------------------------------------
//    
void CIpSecPolicyHandler::NotifyUnload()
    {
	iObserver.PolicyUnloaded();
    }

// ----------------------------------------------------------------------------
// CIpSecPolicyHandler::NotifyError
// ----------------------------------------------------------------------------
//
void CIpSecPolicyHandler::NotifyError( TInt aError )
    {
    TPolicyHandlerState oldState = iHandlerState;
	iHandlerState = EIdle;
	iObserver.PolicyError( oldState, aError );
    }
    
// End of File
