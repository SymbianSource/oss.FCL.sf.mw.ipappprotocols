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
* Name          : CErrorHandler.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CERRORHANDLER_H__
#define __CERRORHANDLER_H__

#include <e32base.h>
#include <in_sock.h>
#include "SipStackServerDefs.h"

class CTransportBase;
class CSIPMessage;
class CSIPResponse;
class RStringF;
class TSIPTransportParams;

class CTransportErrorHandler : public CActive
	{
public:
	static CTransportErrorHandler* NewL(CTransportBase& aTransport);
	static CTransportErrorHandler* NewLC(CTransportBase& aTransport);

	void SendErrorResponseL(const TSIPTransportParams& aParams,
	                        TUint aResponseCode,	
							RStringF aReasonPhrase, 
							CSIPMessage& aMessage,
							const TInetAddr& aAddr);

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	~CTransportErrorHandler();
private:
	void SetViaToResL(CSIPMessage& aMessage);
	void SetToToResL(CSIPMessage& aMessage);
	void SetFromToResL(CSIPMessage& aMessage);
	void SetCallIdToResL(CSIPMessage& aMessage);
	void SetCSeqToResL(CSIPMessage& aMessage);
	void CreateResponseL(TUint aResponseCode,	
							RStringF aReasonPhrase, 
							CSIPMessage& aMessage);

	CTransportErrorHandler(CTransportBase& aTransport);
	
	CTransportBase* iTransport;
	CSIPResponse* iResponse;
	TInetAddr iAddr;
	TTransactionId iId;
private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class CTransErrorHandlerTest;
#endif
	};

#endif // end of __CERRORHANDLER_H__

// End of File
