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
* Name          : CancelUAS.h
* Part of       : TransactionUser
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef CANCELUAS_H
#define CANCELUAS_H

// INCLUDES
#include "NormalUAS.h"

// FORWARD DECLARATIONS
class RStringF;

// CLASS DECLARATION

/*
 * This class represents a CANCEL UserAgent server. It is created for handling
 * a CANCEL request received from network and it sends a final response back to
 * remote endpoint without passing the CANCEL to upper layer.
 */
class CCancelUAS : public CNormalUAS
	{
public:
	static CCancelUAS* NewL(CUserAgentCreateParams& aParams,
							MSipConnectionMgr& aConnectionMgr,
							MSIPRequestRouter& aRouter,
                            MSipDialogs& aDialogs);

	/**
	 * CANCEL has been received from network.
	 *
	 * @pre aCancel != NULL
	 * @post
	 *
	 * @see
	 *
	 * @param aCancel CANCEL request, ownership is transferred
	 * @param aFinalRespSent State which UAS will enter	 
	 */
	void CancelReceivedL(CSIPRequest* aCancel,
						 const CUserAgentState& aFinalRespSent);

	static CCancelUAS& Ptr(CUserAgent& aUserAgent);

private:
	CCancelUAS(CUserAgentCreateParams& aParams,
               MSipDialogs& aDialogs,
               MSIPRequestRouter& aRouter);

	//uses CreateTransactionL of CNormalUAS

	void ConstructL(MSipConnectionMgr& aConnectionMgr);

	/**
	 * Sends 200 response.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *	 
	 * @param aFinalRespSent State which UAS will enter.
	 * @param aToTag To tag	 
	 */
	void Send200L(const CUserAgentState& aFinalRespSent, RStringF aToTag);
	};

#endif // end of CANCELUAS_H

// End of File
