/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CResHandler.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __CRESHANDLER_H__
#define __CRESHANDLER_H__

#include <e32base.h>
#include <e32std.h>
#include <e32def.h>
#include <in_sock.h>
#include "SipStackServerDefs.h"
#include "multiaddrsenderowner.h"
#include "CSipConnection.h"

class CTransportUdp;
class MTimerManager;
class CSIPServerResolver;
class CMultipleAddrResSender;


class CResponseHandler : public CBase, public MMultiAddrSenderOwner
	{
public:
	static CResponseHandler* NewL(CTransportUdp& aOwner, MTimerManager& aTimer,
								  CSIPServerResolver& aServerResolver,
								  TTransactionId aId,
								  TUint aICMPErrorWaitTime);

	static CResponseHandler* NewLC(CTransportUdp& aOwner,
								   MTimerManager& aTimer,
								   CSIPServerResolver& aServerResolver,
								   TTransactionId aId,
								   TUint aICMPErrorWaitTime);
	~CResponseHandler();

	TBool Handle(TTransactionId aId) const;
	void SendL(const TSIPTransportParams& aParams,
	           CSIPResponse& aResponse,
			   const TInetAddr& aAddr,
			   TRequestStatus& aStat);
	TBool CancelAll(TTransactionId aId, TBool aCancelAlso2xxResponses);
	void ICMPErrorL(CSipConnectionMgr::TICMPError aICMPError,
					const TInetAddr& aSourceAddr);
					
	//from MMultiAddrSenderOwner
	void SendToNetL(const TSIPTransportParams& aParams,
	                CSIPResponse& aResponse,
					const TInetAddr& aAddr,
					TRequestStatus& aStat);
	TBool CancelRequest(TRequestStatus& aStat);
	void Remove(CMultipleAddrResSender* aSender);
	
	static const TInt iOffset;
	TSglQueLink iLink;

private:
	CResponseHandler(CTransportUdp& aOwner, MTimerManager& aTimer,
					 CSIPServerResolver& aServerResolver, TTransactionId aId,
					 TUint aICMPErrorWaitTime);

	TTransactionId iId;
	CTransportUdp* iTransport;
	MTimerManager* iTimer;
	CSIPServerResolver* iServerResolver;
	TSglQue<CMultipleAddrResSender> iList;
	TSglQueIter<CMultipleAddrResSender> iListIter;
	
	TUint iICMPErrorWaitTime;
private: //For testing purposes
#ifdef CPPUNIT_TEST	
	friend class CResHandlerTest;
	friend class CMultipleAddrResSenderTest;
#endif
	};
	
#endif // end of __CRESHANDLER_H__

// End of File
