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
* Name          : ResponseQueueItem.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef RESPONSEQUEUEITEM_H
#define RESPONSEQUEUEITEM_H

// INCLUDES
#include <e32base.h>
#include "TSIPTransportParams.h"

// FORWARD DECLARATIONS
class CSIPResponse;


/**
 * This class contains a SIP response and the related transport parameters.
 */
class CResponseQueueItem : public CBase
	{
public: // Constructor and destructor

	/**
	 * Creates a new instance of CResponseQueueItem. CResponseQueueItem never
	 * takes ownership of a 2xx response that is sent to INVITE.
	 *
	 * @pre aResp != NULL
	 *
	 * @param aResp SIP response, ownership is not transferred yet.
	 *	Ownership transfer is done later by using SetResponseOwnership(ETrue)
	 * @param aParams Transport parameters
	 * @param aRespToInvite ETrue if aResp is a response to INVITE
	 * @return new CResponseQueueItem object, ownership is transferred
	 */
	static CResponseQueueItem* NewLC(CSIPResponse* aResp,
									 const TSIPTransportParams& aParams,
									 TBool aRespToInvite);

	~CResponseQueueItem();

public: // New functions

	/**
	 * Takes or releases the ownership of iResp member. If iResp is 2xx and
	 * iRespToInvite is ETrue, the ownership is not taken, regardless of the
	 * value of aOwnsResponse.
	 *
	 * @param aOwnsResponse If ETrue, the CResponseQueueItem owns the response
	 *	stored in iResp.
	 */
	void SetResponseOwnership(TBool aOwnsResponse);

	/**	
	 * Returns the stored SIP response.
	 *
	 * @return SIP response.
	 */
	CSIPResponse& Response();

	/**
	 * Returns the stored transport parameters.
	 *
	 * @return Transport parameters
	 */
	const TSIPTransportParams& TransportParams() const;

	/**	
	 * Checks if the response is to an INVITE request.
	 *	 
	 * @return ETrue Response is to an INVITE request, EFalse otherwise
	 */
	TBool IsRespToInvite() const;

private: // Constructor, for internal use

	/**	
	 * Constructor
	 *
	 * @pre aUri != NULL
	 *
	 * @param aResp SIP response, ownership is transferred
	 * @param aParams Transport parameters	 
	 */
	CResponseQueueItem(CSIPResponse* aResp,
					   const TSIPTransportParams& aParams,
					   TBool aRespToInvite);

private: // Data
	
	CSIPResponse* iResp;
	TSIPTransportParams iParams;	

	//If ETrue, the SIP response in iResp is a response to INVITE.
	//If iResp is 2xx and iRespToInvite is ETrue, iOwnsResponse remains always
	//EFalse, as UAS owns the 2xx response to INVITE.
	TBool iRespToInvite;

	//If ETrue, CResponseQueueItem owns the response in iResp.
	TBool iOwnsResponse;
	};

#endif // end of RESPONSEQUEUEITEM_H

// End of File
