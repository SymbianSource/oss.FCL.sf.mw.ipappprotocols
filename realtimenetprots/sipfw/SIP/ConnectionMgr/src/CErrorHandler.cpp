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
// Name          : CErrorHandler.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0
//




#include "CErrorHandler.h"
#include "CTransport.h"
#include "sipuri.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipviaheader.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TSIPTransportParams.h"
#include "CommonConsts.h"

// -----------------------------------------------------------------------------
// CTransportErrorHandler::NewL
// -----------------------------------------------------------------------------
//
CTransportErrorHandler* CTransportErrorHandler::NewL(CTransportBase& aTransport)
	{
	CTransportErrorHandler* self = NewLC(aTransport);
	CleanupStack::Pop(self);
	return self;	
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::NewLC
// -----------------------------------------------------------------------------
//
CTransportErrorHandler*
CTransportErrorHandler::NewLC(CTransportBase& aTransport)
	{
	CTransportErrorHandler* self =
		new (ELeave) CTransportErrorHandler(aTransport);
	CleanupStack::PushL(self);
	return self;	
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::SendErrorResponseL
// -----------------------------------------------------------------------------
//
void CTransportErrorHandler::SendErrorResponseL(
    const TSIPTransportParams& aParams,
    TUint aResponseCode,	
    RStringF aReasonPhrase, 
	CSIPMessage& aMessage,
	const TInetAddr& aAddr)
	{	
	if (!aMessage.HasHeader(SIPStrings::StringF(SipStrConsts::EViaHeader)))
	    {
	    User::Leave(KErrArgument);
	    }
	    
	if ( !IsActive() )
		{
		if (iTransport->ProtocolType() == KProtocolInetUdp)
			{
			iTransport->CancelResponseSend(iId, ETrue);
			delete iResponse; 
			iResponse = 0;
			CreateResponseL(aResponseCode, aReasonPhrase, aMessage);
			iAddr = aAddr;
			iTransport->Send(aParams, *iResponse, iId, iStatus);
			}
		else
			{
			CreateResponseL(aResponseCode, aReasonPhrase, aMessage);
			iAddr = aAddr;
			iTransport->Send(aParams, *iResponse, iAddr, iStatus);
			}
		SetActive();
		}
		
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::CreateResponseL
// -----------------------------------------------------------------------------
//
void CTransportErrorHandler::CreateResponseL(TUint aResponseCode,	
							RStringF aReasonPhrase, 
							CSIPMessage& aMessage)
	{
	iResponse = CSIPResponse::NewL(aResponseCode, aReasonPhrase);
	SetViaToResL(aMessage);
	if (aMessage.HasHeader(SIPStrings::StringF(SipStrConsts::EToHeader)))
	    {
		SetToToResL(aMessage);
	    }
	if (aMessage.HasHeader(SIPStrings::StringF(SipStrConsts::EFromHeader)))
	    {
		SetFromToResL(aMessage);
	    }
	if (aMessage.HasHeader(SIPStrings::StringF(SipStrConsts::ECallIDHeader)))
	    {
		SetCallIdToResL(aMessage);
	    }
	if (aMessage.HasHeader(SIPStrings::StringF(SipStrConsts::ECSeqHeader)))
	    {
		SetCSeqToResL(aMessage);
	    }

	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::RunL
// -----------------------------------------------------------------------------
//
void CTransportErrorHandler::RunL()
	{
	TUint protocol( iTransport->ProtocolType() );
	if( protocol == KProtocolInetTcp || protocol == KProtocolTls )
		{
		delete iResponse;
		iResponse = 0;
		}
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::DoCancel
// -----------------------------------------------------------------------------
//
void CTransportErrorHandler::DoCancel()
	{
	iTransport->CancelSend(iStatus);
	if(iResponse)
		{
		delete iResponse;
		iResponse = 0;
		}
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::RunError
// -----------------------------------------------------------------------------
//
TInt CTransportErrorHandler::RunError(TInt aError)
	{
	if(aError == KErrNoMemory)
		{
		return aError;
		}
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::SetViaToResL
// -----------------------------------------------------------------------------
//
void CTransportErrorHandler::SetViaToResL(CSIPMessage& aMessage)
	{
	TSglQueIter<CSIPHeaderBase> iter = 
	        aMessage.Headers(SIPStrings::StringF(SipStrConsts::EViaHeader));
	CSIPHeaderBase* header;
	iter.SetToFirst();
	while((header = iter++) != 0)
		{
		CSIPHeaderBase* viaHeader = 
		                    static_cast<CSIPViaHeader*>(header)->CloneL();
		CleanupStack::PushL(viaHeader);
		iResponse->AddHeaderL(static_cast<CSIPViaHeader*>(viaHeader));
		CleanupStack::Pop(viaHeader);
		}
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::SetToToResL
// -----------------------------------------------------------------------------
//
void CTransportErrorHandler::SetToToResL(CSIPMessage& aMessage)
	{
	TSglQueIter<CSIPHeaderBase> iter = 
	        aMessage.Headers(SIPStrings::StringF(SipStrConsts::EToHeader));
	CSIPHeaderBase* header = 0;
	iter.SetToFirst();
	header = iter;
	CSIPToHeader* toHeader =
		CSIPToHeader::NewLC(static_cast<CSIPToHeader&>(*header));
	iResponse->AddHeaderL(toHeader);
	CleanupStack::Pop(toHeader);
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::SetFromToResL
// -----------------------------------------------------------------------------
//
void CTransportErrorHandler::SetFromToResL(CSIPMessage& aMessage)
	{
	TSglQueIter<CSIPHeaderBase> iter = 
	        aMessage.Headers(SIPStrings::StringF(SipStrConsts::EFromHeader));
	CSIPHeaderBase* header = 0;
	iter.SetToFirst();
	header = iter;
	CSIPFromHeader* fromHeader =
		CSIPFromHeader::NewLC(static_cast<CSIPFromHeader&>(*header));
	iResponse->AddHeaderL(fromHeader);
	CleanupStack::Pop(fromHeader);
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::SetCallIdToResL
// -----------------------------------------------------------------------------
//
void CTransportErrorHandler::SetCallIdToResL(CSIPMessage& aMessage)
	{
	TSglQueIter<CSIPHeaderBase> iter =
		aMessage.Headers(SIPStrings::StringF(SipStrConsts::ECallIDHeader));
	CSIPHeaderBase* header = 0;
	iter.SetToFirst();
	header = iter;
	CSIPHeaderBase* callIdHeader = 
	                        static_cast<CSIPCallIDHeader*>(header)->CloneL();
	CleanupStack::PushL(callIdHeader);
	iResponse->AddHeaderL(static_cast<CSIPCallIDHeader*>(callIdHeader));
	CleanupStack::Pop(callIdHeader);
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::SetCSeqToResL
// -----------------------------------------------------------------------------
//
void CTransportErrorHandler::SetCSeqToResL(CSIPMessage& aMessage)
	{
	TSglQueIter<CSIPHeaderBase> iter = aMessage.Headers(
	                        SIPStrings::StringF(SipStrConsts::ECSeqHeader));
	CSIPHeaderBase* header = 0;
	iter.SetToFirst();
	header = iter;

    CSIPHeaderBase* cseqheader = static_cast<CSIPCSeqHeader*>(header)->CloneL();
    CleanupStack::PushL(cseqheader);
	iResponse->AddHeaderL(static_cast<CSIPCSeqHeader*>(cseqheader));
	CleanupStack::Pop(cseqheader);
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::~CTransportErrorHandler
// -----------------------------------------------------------------------------
//
CTransportErrorHandler::~CTransportErrorHandler()
	{
	Cancel();
	delete iResponse;
	}

// -----------------------------------------------------------------------------
// CTransportErrorHandler::CTransportErrorHandler
// -----------------------------------------------------------------------------
//
CTransportErrorHandler::CTransportErrorHandler(CTransportBase& aTransport) :
	CActive(CActive::EPriorityStandard),
	iTransport(&aTransport)
	{
	CActiveScheduler::Add(this);
	iResponse = 0;
	iId = 0;
	}
