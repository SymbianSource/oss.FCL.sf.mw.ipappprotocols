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
// Name          : CSender.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//



#include "CSender.h"
#include "SipLogs.h"
#include "CSocketContainer.h"
#include "siperr.h"
#include "COwnerSettingsList.h"

// -----------------------------------------------------------------------------
// CSender::CSender
// -----------------------------------------------------------------------------
//
CSender::CSender(MContext& aContext,COwnerSettingsList& aSettingsList) :
    CActive(CActive::EPriorityStandard),
    iContext(aContext), 
    iSettingsList(aSettingsList),
    iList(COutgoingData::iOffset),
    iListIter(iList),
    iContinueSending(EFalse)
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CSender::~CSender
// -----------------------------------------------------------------------------
//
CSender::~CSender() 
	{
	CancelAllRequests( KErrSIPTransportFailure );
	}

// -----------------------------------------------------------------------------
// CSender::SendL
// -----------------------------------------------------------------------------
//
void CSender::SendL (
    const TSIPTransportParams& /*aTransportParams*/, 
    CSIPMessage& /*aMessage*/, 
    TRequestStatus& /*aStat*/)
	{
	__ASSERT_DEBUG(EFalse,
	User::Panic(_L("CSender baseclass is called"),
	KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CSender::SendL
// -----------------------------------------------------------------------------
//
void CSender::SendL (COutgoingData* /*aData*/)
	{
	__ASSERT_DEBUG(EFalse,
	User::Panic(_L("CSender baseclass is called"),
	KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CSender::StoredData
// -----------------------------------------------------------------------------
//
COutgoingData* CSender::StoredData()
	{
	return 0;
	}

// -----------------------------------------------------------------------------
// CSender::Data
// -----------------------------------------------------------------------------
//
COutgoingData* CSender::Data()
	{
	return 0;
	}

// -----------------------------------------------------------------------------
// CSender::FindAndCancel
// -----------------------------------------------------------------------------
//
TBool CSender::FindAndCancel(TRequestStatus& aStat)
	{
	COutgoingData* currentData = CurrentData();
	
	if (currentData && &aStat == currentData->Status())
		{
		iContext.SocketContainer().CancelSend();
        iContext.Sending( EFalse );
        RemoveCurrentData( KErrSIPTransportFailure );
        return ETrue;
		}	
	
	if (!iList.IsEmpty())
		{
		iListIter.SetToFirst();
		for(COutgoingData* data = iListIter; data; data = iListIter++)
			{
			if(&aStat == data->Status())
				{
				CompleteRequest(data->Status(), KErrSIPTransportFailure );
				iList.Remove(*data);
				delete data;
				return ETrue;
				}
			}
		}

	return EFalse;		
	}

// -----------------------------------------------------------------------------
// CSender::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CSender::CancelAllRequests (TInt aReason)
	{
	Cancel();
	RemoveCurrentData(aReason);
	if (!iList.IsEmpty())
		{
		COutgoingData* data = 0;
		iListIter.SetToFirst();
		while ((data = iListIter++) != 0)
			{
			CompleteRequest(data->Status(),aReason);
			iList.Remove(*data);
			delete data;
			}
		}
	}

// -----------------------------------------------------------------------------
// CSender::CurrentData
// -----------------------------------------------------------------------------
//
COutgoingData* CSender::CurrentData()
    {
    COutgoingData* data( NULL );
    if ( !iList.IsEmpty() )
        {
        data = iList.First();
        }
    return data;
    }

// -----------------------------------------------------------------------------
// CSender::IpAddressType
// -----------------------------------------------------------------------------
//
TUint CSender::IpAddrType( const TInetAddr& aAddr )
    {
    // TInetAddr::Family returns sometimes KAfInet6 for an IPv4 address.
    // The way to make sure the address is IPv4 is to use TInetAddr::Address.
    // This is because a real IPv6 cannot be represented with a TUint.
    TUint type( KAFUnspec );
    if ( aAddr.Address() )
        {
        type = KAfInet;
        }
    else
        {
        type = KAfInet6;
        }
    return type;
    }

// -----------------------------------------------------------------------------
// CSender::CleanCurrentDataOnLeave
// -----------------------------------------------------------------------------
//
void CSender::CleanCurrentDataOnLeave( TAny* aSelf )
    {
    CSender* self = reinterpret_cast< CSender* >( aSelf );
    COutgoingData* data = self->CurrentData();
	if ( data )
		{
		self->iList.Remove( *data );
		self->iContinueSending = EFalse;
		delete data;
		}
	}

// -----------------------------------------------------------------------------
// CSender::WriteToLog
// -----------------------------------------------------------------------------
//
#if (defined(_DEBUG) && defined(USE_SIP_LOGS))
void CSender::WriteToLog( const TDesC8& aMessage )
	{
	if ( CurrentData() )
	    {
	    __SIP_ADDR_LOG( "Outgoing data to address", CurrentData()->Address() )
	    }
	__SIP_DES8_LOG( "Outgoing data", aMessage )
	}
#else
void CSender::WriteToLog( const TDesC8& /*aMessage*/ )
	{
	}
#endif


// -----------------------------------------------------------------------------
// CSender::RunL
// -----------------------------------------------------------------------------
//
void CSender::RunL()
	{
	TInt status = iStatus.Int();
	
	__SIP_INT_LOG1( "CSender::RunL status:", status )
	
	TBool terminated( EFalse );

    switch ( status )
        {
        case KErrNone:
            {
            COutgoingData* data = CurrentData();
           	if ( data )
    			{
    			data->Sent();
    			
    			CSocketContainer* socketContainer =
    			    iContext.SocketContainer( IpAddrType( data->Address() ) );
    			if ( socketContainer )
    			    {
        			// Clear socket options after sending has completed.
        			// Do only if it completed successfully as it might be 
        			// unsafe to attempt setting options after socket failure.
        			iSettingsList.ClearOpts( 
        			    data->TransportParams(),
        	            socketContainer->Socket() );
    			    }
    			}
    		if ( !iContinueSending )
    			{
    			RemoveCurrentData( KErrNone );
    			}
            break;
            }
        case KErrCancel:
            {
            break;
            }
        case KErrDisconnected:
            {
            terminated = iContext.DisconnectedL();
            break;
            }
        default: // Rest of the errors
            {
            terminated = iContext.StopL();
            if ( !terminated )
                {
                iContext.ResetSocketL();
			    iContext.ContinueL();
                }
            break;
            }
        }
        
    if ( !terminated )
        {
        SendNextL();
        }
	}

// -----------------------------------------------------------------------------
// CSender::RunError
// -----------------------------------------------------------------------------
//
TInt CSender::RunError(TInt aError)
	{
	if(aError != KErrNoMemory)
		{
		return KErrNone;
		}
	return aError;
	}

// -----------------------------------------------------------------------------
// CSender::DoCancel
// -----------------------------------------------------------------------------
//
void CSender::DoCancel()
	{
	iContext.SocketContainer().CancelSend();
	iContext.Sending( EFalse );
	RemoveCurrentData( KErrSIPTransportFailure );
	}

// -----------------------------------------------------------------------------
// CSender::RemoveCurrentData
// -----------------------------------------------------------------------------
//
void CSender::RemoveCurrentData(TInt aReason)
	{
	COutgoingData* data = CurrentData();
	if ( data )
		{
		iList.Remove( *data );
		iContinueSending = EFalse;
		CompleteRequest(data->Status(),aReason);
		delete data;
		}
	}

// -----------------------------------------------------------------------------
// CSender::CompleteRequest
// -----------------------------------------------------------------------------
//
void CSender::CompleteRequest (TRequestStatus* aStatus, TInt aReason)
	{
	if (aStatus != 0 && *aStatus == KRequestPending)
		{
		User::RequestComplete(aStatus,aReason);
		}
	}
