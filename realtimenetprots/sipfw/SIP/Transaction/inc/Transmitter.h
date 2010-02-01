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
* Name          : Transmitter.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef TRANSMITTER_H
#define TRANSMITTER_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>

#include "SipStackServerDefs.h"

// FORWARD DECLARATIONS
class CUri8;
class MSipConnectionMgr;
class CSIPRequest;
class CSIPResponse;
class TSIPTransportParams;
class MSendStatus;

// CLASS DECLARATION

/*
 * CTransmitter handles sending data to ConnectionMgr.
 *
 * Every CUserAgent owns one CTransmitter instance, and the associated
 * CTransaction uses the same CTransmitter object for sending.
 *
 * In case of a 2xx response to an INVITE, the InviteUAS/InviteUAC also use the
 * CTransmitter. CUserAgent can use the same instance as the CTransaction was
 * using since they are never both sending at the same time.
 */
class CTransmitter : public CActive
	{
public: // Constructors and destructor

	/**
	 * Creates an instance of CTransmitter.
	 *	 
	 * @param aConnectionMgr ConnectionMgr to be used for sending.
	 * @return value New CTransmitter object, ownership is transferred.
	 */
	static CTransmitter* NewL(MSipConnectionMgr& aConnectionMgr);

	~CTransmitter();

public: // From CActive:

	void DoCancel();

	void RunL();

	TInt RunError(TInt aError);

public: // New functions

	/**
	 * Send a SIP request message to network. 
	 *
	 * @pre aObserver != NULL
	 *
	 * @param aReq SIP request to be sent. Ownership is not transferred.
	 * @param aAddress Address where the message will be sent
	 * @param aForceUDP ETrue if transport protocol must remain UDP
	 * @param aParams Transport parameters that are given to ConnectionMgr when
	 *	sending the SIP request.
	 * @param aOutboundProxy outbound proxy, NULL pointer if not used.
     *        The ownership is not transferred.
	 * @param aObserver Callback which CTransmitter will use after it has
	 *	served the sending of the message. Since CUserAgent and CTransaction
	 *	share a common CTransmitter object, the MSendStatus callback is passed
	 *  on every SendRequestL/SendResponseL call, so CTransmitter calls the
	 *  correct object. Ownership isn't	transferred.	 
	 */
	void SendRequestL(CSIPRequest& aReq,
					  const TInetAddr& aAddress,
					  TBool aForceUDP,
					  const TSIPTransportParams& aParams,
					  const CUri8* aOutboundProxy,
					  MSendStatus* aObserver);

	/**
	 * Send a SIP response message to network. 
	 *
	 * @pre aObserver != NULL
	 *
	 * @param aResp SIP response to be sent. Ownership is not transferred.
	 * @param aParams Transport parameters that are given to ConnectionMgr when
	 *	sending the SIP response.
	 * @param aObserver Callback which CTransmitter will use after it has
	 *	served the sending of the message. Since CUserAgent and CTransaction
	 *	share a common CTransmitter object, the MSendStatus callback is passed
	 *  on every SendRequestL/SendResponseL call, so CTransmitter calls the
	 *  correct object. Ownership isn't	transferred.
	 * @param aTransactionId TransactionId of transaction that sends the
	 *	response.
	 */
	void SendResponseL(CSIPResponse& aResp,
					   const TSIPTransportParams& aParams,
					   MSendStatus* aObserver,
					   TTransactionId aTransactionId);

	/**
	 * The user of CTransmitter indicates it doesn't want any events to the
	 * callback it has provided earlier in SendRequestL or SendResponseL.
	 *
	 * E.g. a CTransaction is ending, but it doesn't use CTransmitter::Cancel()
	 * to make sure CTransmitter won't use callback anymore, since in case some
	 * other CTransaction instance or CUserAgent is currently sending, their
	 * sending would get canceled.	 
	 *
	 * @param aObserver Callback which CTransmitter shouldn't use anymore.
	 *	In case CTransmitter is already serving another user (meaning its
	 *  iObserver != aObserver), nothing is done. Ownership isn't transferred.
	 * @return value -
	 */
	void DetachCallback(MSendStatus* aObserver);

	/**
	 * Tells ConnectionMgr to clear any references to SIP responses associated
	 * with the aTransactionId.
	 *
	 * @param aTransactionId Identifies the transaction
	 * @param aAlso2xx Indicates if 2xx responses are also included
	 */
	void CancelSendResponses(TTransactionId aTransactionId,
							 TBool aAlso2xx) const;

    /**
	 * Disconnect TCP connection to the remote host
	 *
	 * @param aRemoteAddr Address of the remote host
	 * @param aParams Transport parameters
	 */
	void TcpDisconnect(TInetAddr& aRemoteAddr,
					   const TSIPTransportParams& aParams) const;

private: // New functions, for internal use

	CTransmitter(MSipConnectionMgr& aConnectionMgr);

    /**
	 * Converts certain error codes into CSIP::ETransportFailure, to indicate
     * a transport layer error has occurred but UAC might have a chance to
     * try again.
	 *
	 * @param aError Error code received from ConnectionMgr
	 * @return Error code which will be used instead of the aError
	 */
	TInt MapErrorCode(TInt aError) const;

private: // Data

	//Callback to the object of the most recent SendRequestL/SendResponseL
	//call. Not owned.
	MSendStatus* iObserver;

    //Transport layer used for sending
	MSipConnectionMgr& iConnectionMgr;

#ifdef CPPUNIT_TEST	
	friend class CTransmitter_Test;
	friend class CInviteCTa_Completed_Test;
#endif
	};

#endif // end of TRANSMITTER_H

// End of File
