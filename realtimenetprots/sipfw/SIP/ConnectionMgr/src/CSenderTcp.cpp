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
// Name          : CSenderTcp.cpp
// Part of       : ConnectionMgr
// Version       : SIP/5.1 
//



#include "SipAssert.h"
#include "CSenderTcp.h"
#include "MContext.h"
#include "SipLogs.h"
#include "sipmessage.h"
#include "CSocketContainer.h"
#include "CommonConsts.h"
#include "COwnerSettingsList.h"

// -----------------------------------------------------------------------------
// CSenderTcp::NewL
// -----------------------------------------------------------------------------
//
CSenderTcp* CSenderTcp::NewL(
    MContext& aContext, 
    COwnerSettingsList& aSettingsList)
	{
	CSenderTcp* self = NewLC(aContext, aSettingsList);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CSenderTcp::NewLC
// -----------------------------------------------------------------------------
//
CSenderTcp* CSenderTcp::NewLC(
    MContext& aContext,
    COwnerSettingsList& aSettingsList)
	{
	CSenderTcp* self = new (ELeave) CSenderTcp(aContext, aSettingsList);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSenderTcp::CSenderTcp
// -----------------------------------------------------------------------------
//
CSenderTcp::CSenderTcp(MContext& aContext, COwnerSettingsList& aSettingsList) : 
    CSender(aContext, aSettingsList)
	{	
	}
	
// -----------------------------------------------------------------------------
// CSenderTcp::~CSenderTcp
// -----------------------------------------------------------------------------
//
CSenderTcp::~CSenderTcp()
	{
	Cancel();
	}

// -----------------------------------------------------------------------------
// CSenderTcp::SendL
// -----------------------------------------------------------------------------
//
TBool CSenderTcp::SendL(const TSIPTransportParams& aTransportParams,
                        CSIPMessage& aMessage,
					    const TInetAddr& aAddr,
					    TUint aOrigTransport,
					    TRequestStatus& aStat,
					    TBool aStore,
					    TBool /*aForceUDP*/)
	{
	if(!aStore)
		{
		if(!IsActive())
			{
			if(aMessage.Content().Length() > 0)
				{
				iContinueSending = ETrue;
				}
			
			COutgoingData* data = 
			    COutgoingData::NewL( aTransportParams, aMessage, aAddr, 
			                         aOrigTransport, aStat, 
			                         *iContext.SigCompHandler() );		

	        iList.AddFirst( *data );		
		
            TCleanupItem cleanupItem( CleanCurrentDataOnLeave, this );
            CleanupStack::PushL( cleanupItem );
										
			EncodeAndSendL(aAddr);
			
			CleanupStack::Pop( 1 ); // cleanupItem
			
			return EFalse;
			}
		}

	return HandleFirstSendL( aTransportParams,
		                     aMessage, 
		                     aAddr, 
		                     aOrigTransport, 
		                     aStat,
		                     aStore );
	}

// -----------------------------------------------------------------------------
// CSenderTcp::SendL
// -----------------------------------------------------------------------------
//
void CSenderTcp::SendL(const TSIPTransportParams& aTransportParams,
                       CSIPMessage& aMessage, 
                       TRequestStatus& aStat)
	{
	if (!IsActive())
		{
		if(aMessage.Content().Length() > 0)
			{
			iContinueSending = ETrue;
			}
		
		COutgoingData* data = 	
            COutgoingData::NewL( aTransportParams, aMessage, aStat, 
                                 *iContext.SigCompHandler() );

        iList.AddFirst( *data );		
		
        TCleanupItem cleanupItem( CleanCurrentDataOnLeave, this );
        CleanupStack::PushL( cleanupItem );

		TInetAddr address;
		iContext.SocketContainer().RemoteName( address );
		
		EncodeAndSendL( address );
		
		CleanupStack::Pop( 1 ); // cleanupItem
		}
	else
		{
    	COutgoingData* data = COutgoingData::NewL( aTransportParams, 
    	                                           aMessage, aStat,
    	                                           *iContext.SigCompHandler() );
	    iList.AddLast( *data );
		}
	}

// -----------------------------------------------------------------------------
// CSenderTcp::SendNextL
// -----------------------------------------------------------------------------
//
void CSenderTcp::SendNextL()
	{
	COutgoingData* data = CurrentData();
	
	if ( !IsActive() )
		{
		if ( iContinueSending && data )
			{
			DoSend( data->Message().Content() );
			iContinueSending = EFalse;
			}
		else if ( data )
			{
			if ( data->Message().Content().Length() > 0 )
				{
				iContinueSending = ETrue;
				}
			TInetAddr address;
			iContext.SocketContainer().RemoteName( address );
            EncodeAndSendL( address );
			}
		else
			{
			iContext.Sending( EFalse );
			}
		}
	}

// -----------------------------------------------------------------------------
// CSenderTcp::EncodeAndSendL
// -----------------------------------------------------------------------------
//
void CSenderTcp::EncodeAndSendL( const TInetAddr& aAddr )
	{
	COutgoingData* data = CurrentData();
	
	__SIP_ASSERT_LEAVE( data, KErrNotFound );
	
	data->SetAddress( aAddr );
			
    TPtr8 ptrOutgoing = data->EncodeL( ETrue );
    
    if ( data->OrigTransport() == KProtocolInetUdp &&
         data->Compressed() &&
         ptrOutgoing.Length() < KMaxUdpMessageSize )
	    {
	    // If transport was originally UDP and compression decreased size
	    // of the message to be under MTU, UDP can be used.
	    data->Sent();
		iList.Remove( *data );
		iContext.Sending( EFalse );
		CleanupStack::PushL( data );
		iContext.ReRouteL( KProtocolInetUdp, data );
		CleanupStack::Pop( data );
		SendNextL();
		}
    else
        {
    	iOutgoingMessage.Set( ptrOutgoing );
    	if ( data->Compressed() )
    		{
    		iContinueSending = EFalse;
    		}
    	
    	__SIP_MESSAGE_LOG( "Connection Manager::SendToNetwork via TCP", 
    	                   iOutgoingMessage )
    	WriteToLog( iOutgoingMessage );
    	
    	DoSend( iOutgoingMessage );
        }
	}

// -----------------------------------------------------------------------------
// CSenderTcp::HandleFirstSendL
// -----------------------------------------------------------------------------
//	
TBool CSenderTcp::HandleFirstSendL( 
    const TSIPTransportParams& aTransportParams,
    CSIPMessage& aMessage, 
    const TInetAddr& aAddr, 
	TUint aOrigTransport, 
	TRequestStatus& aStat,
	TBool aStore )
    {
    COutgoingData* data = COutgoingData::NewLC( aTransportParams,
	                                            aMessage, 
	                                            aAddr, 
	                                            aOrigTransport,
		                                        aStat, 
											    *iContext.SigCompHandler() );
	
	TBool isFirstSend( iList.IsEmpty() && aStore );
	
	if ( aOrigTransport == KProtocolInetUdp && isFirstSend )
	    {
	    // Have to check immediately whether compression decreases size
	    // so that UDP could be used.
	    
	    TPtr8 encoded = data->EncodeL( ETrue );
	    
	    if ( encoded.Length() < KMaxUdpMessageSize && data->Compressed() )
	        {
	        data->Sent();
    		iContext.ReRouteL( KProtocolInetUdp, data );
    		CleanupStack::Pop( data );
    		return EFalse;
	        }
	    }

    CleanupStack::Pop( data );
	iList.AddLast( *data );
	
	return isFirstSend;
    }
	
// -----------------------------------------------------------------------------
// CSenderTcp::StoredData
// -----------------------------------------------------------------------------
//
COutgoingData* CSenderTcp::StoredData()
	{
	if (!iList.IsEmpty())
		{
		COutgoingData* listItem = 0;
		iListIter.SetToFirst();
		while ((listItem = iListIter++) != 0)
			{
			iList.Remove(*listItem);
            return listItem;
			}
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CSenderTcp::Data
// -----------------------------------------------------------------------------
//
COutgoingData* CSenderTcp::Data()
	{
	if (!iList.IsEmpty())
		{
		COutgoingData* listItem = 0;
		iListIter.SetToFirst();
		while ((listItem = iListIter++) != 0)
			{
			iList.Remove(*listItem);
			return listItem;
			}
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CSenderTcp::DoSend
// -----------------------------------------------------------------------------
//
void CSenderTcp::DoSend( const TDesC8& aData )
    {
    COutgoingData* data = CurrentData();
    
    __SIP_ASSERT_RETURN( !IsActive() && data, KErrInUse );
    
    // Set socket options before sending, options will be cleared after
    // send has completed.
    iSettingsList.SetOpts( data->TransportParams(),
    	                   iContext.SocketContainer().Socket() );
    	                       
	iContext.Sending( ETrue );
	iContext.SocketContainer().Send( aData, 0, iStatus );
			
	SetActive();
    }

