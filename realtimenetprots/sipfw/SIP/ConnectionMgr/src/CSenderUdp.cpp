// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSenderUdp.cpp
// Part of       : ConnectionMgr
// Version       : SIP/5.1
//



#include "CSenderUdp.h"
#include "CommonConsts.h"
#include "SipLogs.h"
#include "SipAssert.h"
#include "siperr.h"
#include "TSIPTransportParams.h"
#include "sipmessage.h"
#include "CSocketContainer.h"
#include "COwnerSettingsList.h"

// -----------------------------------------------------------------------------
// CSenderUdp::NewL
// -----------------------------------------------------------------------------
//
CSenderUdp* CSenderUdp::NewL( 
    MContext& aContext, 
    COwnerSettingsList& aSettingsList )
	{
	CSenderUdp* self = NewLC( aContext, aSettingsList );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSenderUdp::NewLC
// -----------------------------------------------------------------------------
//
CSenderUdp* CSenderUdp::NewLC( 
    MContext& aContext, 
    COwnerSettingsList& aSettingsList )
	{
	CSenderUdp* self = new ( ELeave ) CSenderUdp( aContext, aSettingsList );
	CleanupStack::PushL( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSenderUdp::CSenderUdp
// -----------------------------------------------------------------------------
//
CSenderUdp::CSenderUdp( MContext& aContext, COwnerSettingsList& aSettingsList ) : 
    CSender( aContext, aSettingsList ),
    iPermissionToSend( ETrue )
	{
	}

// -----------------------------------------------------------------------------
// CSenderUdp::~CSenderUdp
// -----------------------------------------------------------------------------
//
CSenderUdp::~CSenderUdp()
	{
	Cancel();
	}

// -----------------------------------------------------------------------------
// CSenderUdp::SendL
// -----------------------------------------------------------------------------
//
TBool CSenderUdp::SendL( const TSIPTransportParams& aTransportParams,
                         CSIPMessage& aMessage,
					     const TInetAddr& aAddr,
					     TUint aOrigTransport,
					     TRequestStatus& aStat,
					     TBool /*aStore*/,
					     TBool aForceUDP )
	{
    COutgoingData* data = 
        COutgoingData::NewL( aTransportParams, aMessage, aAddr, 
	                         aOrigTransport, aStat, *iContext.SigCompHandler(),
						     aForceUDP );	
	
	if ( !IsActive() && iPermissionToSend )
		{
	    iList.AddFirst( *data );		
		
        TCleanupItem cleanupItem( CleanCurrentDataOnLeave, this );
        CleanupStack::PushL( cleanupItem );		
		
        EncodeAndSendL( EFalse );
        
        CleanupStack::Pop( 1 ); // cleanupItem
		}
	else
	    {
	    iList.AddLast( *data );
	    }
		
    return EFalse;
	}

// -----------------------------------------------------------------------------
// CSenderUdp::SendL
// -----------------------------------------------------------------------------
//
void CSenderUdp::SendL( COutgoingData* aData )
	{
	iList.AddLast( *aData );
	SendNextL();
	}

// -----------------------------------------------------------------------------
// CSenderUdp::SendNextL
// -----------------------------------------------------------------------------
//
void CSenderUdp::SendNextL()
	{
	if ( !IsActive() )
	    {
    	if ( iPermissionToSend && !iList.IsEmpty() )
    		{
    		EncodeAndSendL( ETrue );
    		}
        else
            {
            iContext.Sending( EFalse );
            }
	    }
	}

// -----------------------------------------------------------------------------
// CSenderUdp::SetSendPermission
// -----------------------------------------------------------------------------
//	
void CSenderUdp::SetSendPermission( TBool aPermissionToSend )
    {
    iPermissionToSend = aPermissionToSend;
    }
    
// -----------------------------------------------------------------------------
// CSenderUdp::EncodeAndSendL
// -----------------------------------------------------------------------------
//
void CSenderUdp::EncodeAndSendL( TBool aRemoveFromList )
	{
	COutgoingData* data = CurrentData();
	
	__SIP_ASSERT_LEAVE( data, KErrNotFound );
	
	TBool sendData = ETrue;
	TPtr8 pointer = data->EncodeL( EFalse );	
	TInt encodedLen = pointer.Length();

	if ( encodedLen > KMaxMessageLength && data->ForceUDP() )
		{
		// Message is too large to be sent with UDP
		User::Leave( KErrTooBig );
		}

	if ( (encodedLen < KMaxUdpMessageSize ||
		 !data->Message().IsRequest() ||
		 data->ForceUDP() ||
		 data->OrigTransport() == KProtocolInetUdp) &&
		 !data->Compressed() )
		{
		iOutgoingMessage.Set( pointer );
		}
	else if ( data->Compressed() && encodedLen < KMaxUdpMessageSize )
		{
		iOutgoingMessage.Set( pointer );
		}
	else
		{
		data->Sent();
		if ( aRemoveFromList )
			{
			iList.Remove( *data );
			}
		CleanupStack::PushL( data );
		iContext.ReRouteL( KProtocolInetTcp, data );
		CleanupStack::Pop( data );
		sendData = EFalse;
		}
	
	if ( sendData )
	    {
        CSocketContainer* socketContainer( 
            iContext.SocketContainer( IpAddrType( data->Address() ) ) );
        __ASSERT_ALWAYS( socketContainer, User::Leave( KErrNotFound ) );
    	__SIP_MESSAGE_LOG( "Connection Manager::SendToNetwork via UDP", 
    	                   iOutgoingMessage )
    	WriteToLog( iOutgoingMessage );
    	
    	// Set socket options before sending, options will be cleared after
    	// send has completed.
    	iSettingsList.SetOpts( 
    	    data->TransportParams(), socketContainer->Socket() );
    	
    	iContext.Sending( ETrue );
    	socketContainer->Socket().SendTo( 
            iOutgoingMessage,
            data->Address(),
            0,
            iStatus );
    	SetActive();
	    }
	}
