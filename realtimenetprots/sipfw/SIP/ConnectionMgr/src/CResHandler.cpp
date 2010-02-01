// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CResHandler.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "CResHandler.h"
#include "SipLogs.h"
#include "Lwtimer.h"
#include "CTransportUdp.h"
#include "multipleaddrressender.h"
#include "sipresponse.h"
#include "sipmessage.h"

const TInt CResponseHandler::iOffset = _FOFF(CResponseHandler, iLink);

// -----------------------------------------------------------------------------
// CResponseHandler::NewL
// -----------------------------------------------------------------------------
//
CResponseHandler* CResponseHandler::NewL(CTransportUdp& aOwner,
										 MTimerManager& aTimer,
										 CSIPServerResolver& aServerResolver,
										 TTransactionId aId,
										 TUint aICMPErrorWaitTime)
	{
	CResponseHandler* self = NewLC(aOwner, aTimer, aServerResolver,
								   aId, aICMPErrorWaitTime);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CResponseHandler::NewLC
// -----------------------------------------------------------------------------
//
CResponseHandler* CResponseHandler::NewLC(CTransportUdp& aOwner,
										  MTimerManager& aTimer,
										  CSIPServerResolver& aServerResolver,
										  TTransactionId aId,
										  TUint aICMPErrorWaitTime)
	{
	CResponseHandler* self =
		new (ELeave) CResponseHandler(aOwner, aTimer, aServerResolver,
									  aId, aICMPErrorWaitTime);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CResponseHandler::Handle
// -----------------------------------------------------------------------------
//
TBool CResponseHandler::Handle(TTransactionId aId) const
	{
	return (aId == iId);
	}

// -----------------------------------------------------------------------------
// CResponseHandler::SendL
// -----------------------------------------------------------------------------
//
void CResponseHandler::SendL(const TSIPTransportParams& aParams,
                             CSIPResponse& aResponse,
							 const TInetAddr& aAddr,
							 TRequestStatus& aStat)
	{
	CMultipleAddrResSender* multiSender =
		new (ELeave) CMultipleAddrResSender(aParams,
		                                    *this,
											*iTimer,
											*iServerResolver,
											iICMPErrorWaitTime
											);
	iList.AddLast(*multiSender);
	multiSender->SendL(aResponse, aAddr, aStat);
	}

// -----------------------------------------------------------------------------
// CResponseHandler::CancelAll
// -----------------------------------------------------------------------------
//
TBool CResponseHandler::CancelAll(
    TTransactionId aId, 
    TBool aCancelAlso2xxResponses)
	{
	if ( aId == iId )
		{
		CMultipleAddrResSender* listItem;
		iListIter.SetToFirst();
		while ( ( listItem = iListIter++ ) != 0 )
			{
			if ( aCancelAlso2xxResponses || !listItem->Has2xxResponse() )
			    {
			    iList.Remove( *listItem );
			    delete listItem;
			    }
			}
	    if ( iList.IsEmpty() )
	        {
	    	iTransport->RemoveResHandler( this );
	        }
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CResponseHandler::SendToNetL
// -----------------------------------------------------------------------------
//
void CResponseHandler::SendToNetL(const TSIPTransportParams& aParams,
                                  CSIPResponse& aResponse,
								  const TInetAddr& aAddr,
								  TRequestStatus& aStat)
	{
	iTransport->SendToNetL(aParams, aAddr, aResponse, aStat);
	}

// -----------------------------------------------------------------------------
// CResponseHandler::CancelRequest
// -----------------------------------------------------------------------------
//
TBool CResponseHandler::CancelRequest(TRequestStatus& aStat)
	{
	return iTransport->CancelSend(aStat);
	}

// -----------------------------------------------------------------------------
// CResponseHandler::ICMPErrorL
// -----------------------------------------------------------------------------
//
void CResponseHandler::ICMPErrorL(CSipConnectionMgr::TICMPError aICMPError,
								  const TInetAddr& aSourceAddr)
	{
	CMultipleAddrResSender* listItem;
	iListIter.SetToFirst();
	while ((listItem = iListIter++) !=0)
		{
		listItem->ICMPErrorL(aICMPError, aSourceAddr);
		}
	}

// -----------------------------------------------------------------------------
// CResponseHandler::Remove
// -----------------------------------------------------------------------------
//
void CResponseHandler::Remove(CMultipleAddrResSender* aSender)
	{
	if( !iList.IsEmpty() )
	    {
	    iList.Remove(*aSender);
	    delete aSender;
	    __SIP_LOG( "CMultipleAddrResSender removed" )
	    }
	}

// -----------------------------------------------------------------------------
// CResponseHandler::CResponseHandler
// -----------------------------------------------------------------------------
//
CResponseHandler::CResponseHandler(CTransportUdp& aOwner,
								   MTimerManager& aTimer,
								   CSIPServerResolver& aServerResolver,
								   TTransactionId aId,
								   TUint aICMPErrorWaitTime) : 
	iTransport(&aOwner), iTimer(&aTimer), iServerResolver(&aServerResolver),  
	iList(CMultipleAddrResSender::iMultisenderOffset), iListIter(iList),
	iICMPErrorWaitTime(aICMPErrorWaitTime)
	{
	iId = aId;
	}

// -----------------------------------------------------------------------------
// CResponseHandler::~CResponseHandler
// -----------------------------------------------------------------------------
//
CResponseHandler::~CResponseHandler() 
	{
	CMultipleAddrResSender* listItem = 0;
	iListIter.SetToFirst();
	while ((listItem = iListIter++) != 0)
		{
		iList.Remove(*listItem);
		delete listItem;
		}
	}
