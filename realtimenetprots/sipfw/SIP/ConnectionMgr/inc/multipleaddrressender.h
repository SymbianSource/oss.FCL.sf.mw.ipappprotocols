/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : multipleaddrressender.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MULTIPLEADDRRESSENDER_H__
#define __MULTIPLEADDRRESSENDER_H__
 
#include <e32base.h>
#include "CSipConnectionMgr.h"
#include "Lwtimer.h"
#include "MSIPServerResolverObserver.h"


class CSIPResponse;
class MMultiAddrSenderOwner;
class CSIPServerResolver;
class MSIPResolvingResult;
class TSIPTransportParams;

class CMultipleAddrResSender : public CActive, 
                               public MExpirationHandler,
                               public MSIPServerResolverObserver
	{
public:
	CMultipleAddrResSender (const TSIPTransportParams& aParams,
	                        MMultiAddrSenderOwner& aMyOwner, 
							MTimerManager& aTimer,
							CSIPServerResolver& aServerResolver,
							TUint aICMPErrorWaitTime
							);
	~CMultipleAddrResSender();
	
	void RunL();
	
	void DoCancel();
	
	TInt RunError(TInt aError);
	
	void SendL(CSIPResponse& aResponse,
			   const TInetAddr& aAddr,
			   TRequestStatus& aStat);
	
	TInt CancelResponse(TRequestStatus& aStat);
	
	void ICMPErrorL(CSipConnectionMgr::TICMPError aICMPError,
					const TInetAddr& aErrAddress);
	
	void TimerExpiredL(TTimerId aTimerId, TAny* aTimerParam);
	
	TBool Has2xxResponse() const;
	
	static const TInt iMultisenderOffset;
	TSglQueLink iMultisenderLink;
	
public: // From MSIPServerResolverObserver

    void CompletedL();
	void ErrorOccured( TInt aError );

private:

    enum TSenderStates
        {
        EInitialized = 0,
        ESendingFirst,
        EResolving,
        ESendingList
        };

	void ICMPErrorWithCurrentAddressL();
	void WaitForICMPErrorsL();	
	void SendNextL();
	void ResolveL();
	void SendToNetL( MSIPResolvingResult& aResult );
	void CompleteClientRequest (TInt aReason);
	
	TBool IndexWithinBounds();	
	MSIPResolvingResult* FirstUnfailedResult();

	//data
	const TSIPTransportParams& iTransportParams;
	MMultiAddrSenderOwner* iOwner;
	MTimerManager& iTimer;
	CSIPResponse* iResponse;
	CSIPServerResolver& iServerResolver;
	
	//Duration (milliseconds) how long the ICMP errors prepared to be handled.	
	TUint iICMPErrorWaitTime;
	
	TRequestStatus* iClientStatus;
	RPointerArray<MSIPResolvingResult> iResultArray;
	TInetAddr iFirstAddr;
	TInt iCurrentIndex;
	TBool iInUse;
	TSenderStates iState;
	TTimerId iTimerId;
	TBool iICMPErrorOccuredWhileActive;	

private: //For testing purposes	
#ifdef CPPUNIT_TEST
	friend class CMultipleAddrResSenderTest;
#endif
	};

#endif // end of MULTIPLEADDRRESSENDER_H

// End of File
