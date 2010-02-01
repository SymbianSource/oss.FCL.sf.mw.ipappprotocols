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
// Name          : COutgoingData.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "COutgoingData.h"
#include "siprouteheader.h"
#include "sipaddress.h"
#include "sipuri.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipcontactheader.h"
#include "sipfromheader.h"
#include "siphostport.h"
#include "MSigCompController.h"
#include "TSIPTransportParams.h"



const TInt COutgoingData::iOffset = _FOFF(COutgoingData, iLink);

// -----------------------------------------------------------------------------
// COutgoingData::NewL
// -----------------------------------------------------------------------------
//
COutgoingData* COutgoingData::NewL (const TSIPTransportParams& aTransportParams,
                                    CSIPMessage& aMessage,
									const TInetAddr& aAddr,
								    TUint aOrigTransport, 
									TRequestStatus& aStatus, 
									MSigCompController& aSigCompHandler,
									TBool aForceUDP)
	{
	COutgoingData* self = NewLC(aTransportParams,
	                            aMessage,
		                        aAddr,
								aOrigTransport,
								aStatus,
								aSigCompHandler,
								aForceUDP);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// COutgoingData::NewLC
// -----------------------------------------------------------------------------
//
COutgoingData* COutgoingData::NewLC (const TSIPTransportParams& aTransportParams,
                                     CSIPMessage& aMessage,
									 const TInetAddr& aAddr, 
									 TUint aOrigTransport,
									 TRequestStatus& aStatus, 
									 MSigCompController& aSigCompHandler,
									 TBool aForceUDP)
	{
	COutgoingData* self = new (ELeave) COutgoingData(aTransportParams,
	                                                 aMessage,
		                                             aAddr,
		                                             aOrigTransport,
												     aSigCompHandler,
												     aStatus,
												     aForceUDP);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// COutgoingData::NewL
// -----------------------------------------------------------------------------
//
COutgoingData* COutgoingData::NewL (const TSIPTransportParams& aTransportParams,
                                    CSIPMessage& aMessage,
									TRequestStatus& aStatus, 
									MSigCompController& aSigCompHandler)
	{
	COutgoingData* self = NewLC (aTransportParams,
	                             aMessage,
		                         aStatus,
								 aSigCompHandler);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// COutgoingData::NewLC
// -----------------------------------------------------------------------------
//
COutgoingData* COutgoingData::NewLC (const TSIPTransportParams& aTransportParams,
                                     CSIPMessage& aMessage,
									 TRequestStatus& aStatus, 
									 MSigCompController& aSigCompHandler)
	{
	COutgoingData* self = new (ELeave) COutgoingData(aTransportParams,
	                                                 aMessage,
												     aSigCompHandler,
												     aStatus);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// COutgoingData::COutgoingData
// -----------------------------------------------------------------------------
//
COutgoingData::COutgoingData (const TSIPTransportParams& aTransportParams,
                              CSIPMessage& aMessage,
							  MSigCompController& aSigCompHandler,
							  TRequestStatus& aStatus)
 : iTransportParams(aTransportParams),
   iMessage(aMessage),
   iSigCompHandler(aSigCompHandler),
   iForceUDP(EFalse)
	{
	aStatus = KRequestPending;
	iStatus = &aStatus;
	}

// -----------------------------------------------------------------------------
// COutgoingData::COutgoingData
// -----------------------------------------------------------------------------
//
COutgoingData::COutgoingData (const TSIPTransportParams& aTransportParams,
                              CSIPMessage& aMessage,
							  const TInetAddr& aAddr,
							  TUint aOrigTransport,
							  MSigCompController& aSigCompHandler,
							  TRequestStatus& aStatus,
							  TBool aForceUDP)
 : iTransportParams(aTransportParams),
   iMessage(aMessage),
   iSigCompHandler(aSigCompHandler),
   iOrigTransport(aOrigTransport),
   iForceUDP(aForceUDP)
	{
	iAddress = aAddr;
	aStatus = KRequestPending;
	iStatus = &aStatus;
	}

// -----------------------------------------------------------------------------
// COutgoingData::~COutgoingData
// -----------------------------------------------------------------------------
//
COutgoingData::~COutgoingData()
	{
	delete iData;
	}

// -----------------------------------------------------------------------------
// COutgoingData::DataL
// -----------------------------------------------------------------------------
//
TPtr8 COutgoingData::EncodeL(TBool aStreambasedProtocol)
	{
	if(iData)
	    {
	    // Encoded data existed already
	    return iData->Ptr(0);
	    }
	    
	if(iSigCompHandler.IsSupported())
		{
		if(iMessage.IsRequest())
			{
			iData = iSigCompHandler.EncodeL(iTransportParams,
			                                static_cast<CSIPRequest&>(iMessage),
										    iAddress, 
										    aStreambasedProtocol,
											iCompressed);
			}
		else
			{
			iData =
				iSigCompHandler.EncodeL(iTransportParams,
				                        static_cast<CSIPResponse&>(iMessage), 
										iAddress, 
										aStreambasedProtocol, 
										iCompressed);
			}
		}
	else
		{
		if(aStreambasedProtocol)
			{
			iData = iMessage.ToTextHeaderPartL();
			}
		else
			{
			iData = iMessage.ToTextL();
			}
		}
	return iData->Ptr(0);
	}

// -----------------------------------------------------------------------------
// COutgoingData::Sent
// -----------------------------------------------------------------------------
//
void COutgoingData::Sent()
	{
	delete iData;
	iData = 0;
	}

// -----------------------------------------------------------------------------
// COutgoingData::TransportParams
// -----------------------------------------------------------------------------
//	
const TSIPTransportParams& COutgoingData::TransportParams() const
    {
    return iTransportParams;
    }

// -----------------------------------------------------------------------------
// COutgoingData::ForceUDP
// -----------------------------------------------------------------------------
//
TBool COutgoingData::ForceUDP() const
    {
    return iForceUDP;
    }
    
// -----------------------------------------------------------------------------
// COutgoingData::HeaderPartL
// -----------------------------------------------------------------------------
//
TPtr8 COutgoingData::HeaderPartL()
	{
	__ASSERT_DEBUG(iData == 0,
		User::Panic(_L("Sent() was not called"),
		KErrGeneral));
		
	iData = iMessage.ToTextHeaderPartL();
	return iData->Ptr(0);
	}

// -----------------------------------------------------------------------------
// COutgoingData::Message
// -----------------------------------------------------------------------------
//
CSIPMessage& COutgoingData::Message()
	{
	return iMessage;
	}

// -----------------------------------------------------------------------------
// COutgoingData::SetAddress
// -----------------------------------------------------------------------------
//
void COutgoingData::SetAddress(const TInetAddr& aAddress)
	{
	iAddress = aAddress;
	}
