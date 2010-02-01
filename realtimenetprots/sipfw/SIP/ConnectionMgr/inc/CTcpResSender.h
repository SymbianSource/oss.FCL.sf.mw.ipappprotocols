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
* Name          : CTcpResSender.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CTCPRESSENDER_H__
#define __CTCPRESSENDER_H__

#include <in_sock.h>
#include <e32base.h>
#include "MSIPServerResolverObserver.h"

class CSIPServerResolver;
class CSipConnection;
class COutgoingData;
class MSIPResolvingResult;
class TSIPTransportParams;

class CTcpResponseSender : public CBase, public MSIPServerResolverObserver
	{
public:
	CTcpResponseSender( const TSIPTransportParams& aParams,
	                    CSIPServerResolver& aHostResolver, 
					    CSipConnection& aOwner );
	~CTcpResponseSender();

	void Connected( const TInetAddr& aAddr );
	TBool IsInProgress( COutgoingData& aData );
	void ConnectionFailedL();
	void ResolveL( COutgoingData& aData );
	TBool CancelSend( TRequestStatus &aStatus );
	void SetData( COutgoingData* aData );
	
	static const TInt iSenderOffset;
	TSglQueLink iSenderLink;
	
public: // From MSIPServerResolverObserver

    void CompletedL();
	void ErrorOccured( TInt aError );
    
private:
	void SendL();
	void CompleteRequest( TRequestStatus* aStatus, TInt aReason );
	MSIPResolvingResult* FirstUnfailedResult();
	TBool IndexWithinBounds();
	
	const TSIPTransportParams& iTransportParams;
	CSIPServerResolver& iHostResolver;
	CSipConnection* iOwner;
	COutgoingData* iData;
	RPointerArray<MSIPResolvingResult> iResultArray;
	TInt iCurrentIndex;
	TBool iSending;
	TBool iResolving;
private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class CTcpResponseHandlerTest;
#endif
	};

#endif // end of __CTCPRESSENDER_H__

// End of File
