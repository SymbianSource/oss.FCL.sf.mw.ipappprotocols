/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : UserAgentState.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef USERAGENTSTATE_H
#define USERAGENTSTATE_H

// INCLUDES
#include <e32base.h>

#include "SipStackServerDefs.h"
#include "CSipConnectionMgr.h"
#include "Lwtimer.h"	// TTimerId

// FORWARD DECLARATIONS
class TInetAddr;
class RStringF;
class CSIPRequest;
class CSIPResponse;
class CURIContainer;

class MTransactionOwner;
class CUserAgent;

// CLASS DECLARATION

/**
 * Base class for all states of all UserAgent objects. 
 */
class CUserAgentState : public CBase
	{
public:
	virtual ~CUserAgentState();

	virtual void SendRequestL(CUserAgent& aUserAgent,
							  CSIPRequest* aReq,
							  TRegistrationId aRegisterId,
							  const CURIContainer& aRemoteTarget) const;
	virtual void SendResponseL(CUserAgent& aUserAgent,
							   CSIPResponse* aResp,
							   const TSIPTransportParams& aParams) const;
	virtual void SendCancelL(CUserAgent& aUserAgent,
							 TTransactionId aInviteTaId) const;
	virtual void SendAckL(CUserAgent& aUserAgent,
						  CSIPRequest* aAck,
						  MTransactionOwner& aObserver,
						  const TSIPTransportParams& aParams,
						  TRegistrationId aRegisterId,
						  const CURIContainer& aRemoteTarget,
						  TBool aDeleteRequest,
						  RStringF aBranch) const;

	virtual void AddressResolvedL(CUserAgent& aUserAgent) const;
	virtual void ResolvingFailed(CUserAgent& aUserAgent) const;

	virtual void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

    virtual void ReceiveL(CUserAgent& aUserAgent, CSIPRequest* aRequest) const;
	virtual void ReceiveL(CUserAgent& aUserAgent,
						  CSIPResponse* aResponse) const;

	virtual void OwnerFoundL(CUserAgent& aUserAgent,
							 MTransactionOwner* aOwner) const;
	virtual void OwnerNotFoundL(CUserAgent& aUserAgent,
								CSIPResponse* aResp) const;

	virtual void SendCompleteL(CUserAgent& aUserAgent) const;
	virtual void SendFailedL(CUserAgent& aUserAgent, TInt aErrcode) const;

	virtual void SIPSecCacheUpdatedL(CUserAgent& aUserAgent,
									 TBool aSuccess) const;

	virtual void TimerExpiredL(CUserAgent& aUserAgent,
							   TTimerId aTimerId,
							   TAny* aTimerParam) const;
    virtual void RequestRouterErrorL(CUserAgent& aUserAgent) const;

	virtual void IcmpErrorL(CUserAgent& aUserAgent,
							const TInetAddr& aAddress,
							CSipConnectionMgr::TICMPError aError) const;

	/**
	 * Tells if the UA is waiting an address to be resolved.	 
	 * @return value ETrue if UA is resolving, EFalse otherwise
	 */
	virtual TBool IsResolving() const;

	/**
	 * Tells whether the UA can continue without the outgoing SIP message.
	 * @param aFinalRespPassed ETrue if a final response has been passed to
	 *	upper layer, EFalse otherwise
	 * @return value ETrue if the UserAgent can continue, EFalse otherwise
	 */
	virtual TBool CanContinueWithoutOutgoingMsg(TBool aFinalRespPassed) const;

	/**
	 * Finds out if this is an UAS that must be stopped now that upper layer
	 * clears the callback. If this is an INVITE UAS which waiting for upper
	 * layer to provide a final response, then the UAS must be stopped.	 
	 * @return value ETrue UAS must stop
	 *			     EFalse otherwise
	 */
	virtual TBool ShouldUASStop() const;

	/**
	 * A CANCEL request has been received from network. If the UA is in such
	 * a state that it can be canceled, it'll do so and returns ETrue.
	 * @return value ETrue UA is canceled
	 *				 EFalse otherwise
	 */
	virtual TBool CancelL(CUserAgent& aUserAgent) const;

protected:

	CUserAgentState();

	void HandleTimerExpirationL(CUserAgent& aUserAgent,
								TTimerId aTimerId,
								TAny* aTimerParam) const;	
	};

#endif // end of USERAGENTSTATE_H

// End of File
