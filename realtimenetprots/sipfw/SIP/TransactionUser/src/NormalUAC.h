/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : NormalUAC.h
* Part of       : TransactionUser
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef NORMALUAC_H
#define NORMALUAC_H

// INCLUDES
#include "UserAgentClient.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CNormalUAC : public CUserAgentClient
	{
public: // Constructor and destructor

	static CNormalUAC* NewL(CUserAgentCreateParams& aParams,							
                            MSipConnectionMgr& aConnectionMgr,
                            MSipUriResolver& aResolver,
							CSIPSec& aSIPSec,
							TUint32 aCSeqNumber);

	~CNormalUAC() {}

public: // From CUserAgentClient

	void ReceiveResponseL(CSIPResponse* aResp,
						  const CUserAgentState& aResolve,
						  const CUserAgentState& aWaitResponse,
						  const CUserAgentState& aWaitAckFromApp,
						  const CUserAgentState& aWaitTransactionToEnd);

	/**
	 * The old transaction will not send the request anymore and does not need
     * to use NormalUAC's CTransmitter. The possibly pending send request made
     * by the old CNormalClientTransaction is canceled.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
     *
     * @param aNewTransactionId TransactionId that is given to the old
     *  transaction.
	 */
    void PrepareTxForNewRequestL(TTransactionId& aNewTransactionId);
    
    void CreateTransactionL();

public: // New functions

	static CNormalUAC& Ptr(CUserAgent& aUserAgent);

protected:

	CNormalUAC(CUserAgentCreateParams& aParams,
			   MSipUriResolver& aResolver,
			   CSIPSec& aSIPSec,
			   TUint32 aCSeqNumber);    

private: // For testing purposes

#ifdef CPPUNIT_TEST	
	friend class CNormalUAC_WaitResponse_Test;
#endif
	};

#endif // end of NORMALUAC_H

// End of File
