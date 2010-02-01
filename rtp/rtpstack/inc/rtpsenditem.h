/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




#ifndef __RTPSENDITEM_H
#define __RTPSENDITEM_H

#include <e32def.h>
#include "rtputil.h"
/**
* Holds an a packet and request status for buffering if socket is busy.
*
*  @lib RtpService.dll
*/
class CRtpSendItem: public CBase
	{
public:

	/**
	Allocates and constructs a new item for RtpCommSend.
	@cat Construction and destruction
	@return New sendItem object
	@param aData Data for rtpSendItem
	*/
	static CRtpSendItem* NewL(const TDesC8& aPacket, TRequestStatus& aStatus);
    
	/**
    Allocates and constructs a new item for RtpCommSend.
    @cat Construction and destruction
    @return New sendItem object
    @param aData Data for rtpSendItem
    */
    static CRtpSendItem* NewLC(const TDesC8& aPacket, TRequestStatus& aStatus);	
    
    /**
     * Allocates and constructs a new item for RtpCommSend.
     * Stores client's request status while packet is in queue.
     * @since S60 3.2.3
     * @param aPacket rtp packet
     * @param aStatus Queue owner's request status
     * @param aClientStatus request status of the client's active class
     * @return New sendItem object
     */
	static CRtpSendItem* NewL( const TDesC8& aPacket, 
	    TRequestStatus& aStatus, TRequestStatus& aClientStatus );
	
    /**
     * Allocates and constructs a new item for RtpCommSend.
     * Stores client's request status while packet is in queue.
     * @since S60 3.2.3
     * @param aPacket rtp packet
     * @param aStatus Queue owner's request status
     * @param aClientStatus request status of the client's active class
     * @return New sendItem object
     */
    static CRtpSendItem* NewLC( const TDesC8& aPacket, 
        TRequestStatus& aStatus, TRequestStatus& aClientStatus );
	
    /**
	Destructor.
	*/
	~CRtpSendItem();
	const TDesC8& GetData();
	TRequestStatus& iStatus;
	
	/**
	 * Return client status pointer.
	 * @since S60 3.2.3
	 * @return status pointer
	 */
	TRequestStatus* ClientStatus();
	
public:
    static const TInt iOffset; 
private:
	/**
	Constructor.
	@cat Construction and destruction
	*/
	CRtpSendItem( TRequestStatus& aStatus );
    CRtpSendItem( TRequestStatus& aStatus, TRequestStatus& aClientStatus );

	/**
	Second phase constructor.
	@param aData Data for sendItem
	*/
	void ConstructL( const TDesC8& aData );
    
private:
	/** Data for the RTPSend */
	HBufC8* iPacket;

	/** Allows objects to be part of a linked list of sendItem */
	TSglQueLink iLink;
	
    /** 
	 * Holds client's status as long as packet is in queue. If NULL, the
	 * sending operation is synchronous.
	 * Not own.
	 */
	TRequestStatus* iClientStatus;
    };

#endif	// RTPSENDITEM_H
