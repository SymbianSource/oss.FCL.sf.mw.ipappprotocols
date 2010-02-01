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
* Name          : CTransactionHeaders.h
* Part of       : TransactionUser
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef CTRANSACTIONHEADERS_H
#define CTRANSACTIONHEADERS_H

#include <e32base.h>

#include "MTransactionHeaders.h"


class CTransactionHeaders :
	public CBase,
	public MTransactionHeaders
	{
public:
	/**
	 * Creates a new instance.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
     * @param aRequestURI Request-URI, ownership isn't transferred
	 * @param aTo To-header, ownership isn't transferred
	 * @param aFrom From-header, ownership isn't transferred
	 * @param aCallID CallID, ownership isn't transferred.
	 * @param aCSeq CSeq number
	 * @param aRecordRoute Record-route headers, ownership isn't transferred
	 * @param aContact Contact headers, ownership isn't transferred
	 * @return value New CTransactionHeaders object, ownership is transferred.
	 */
	static CTransactionHeaders*
        NewL(const CURIContainer* aRequestURI,
             const CSIPToHeader* aTo,
			 const CSIPFromHeader* aFrom,
			 const CSIPCallIDHeader* aCallID,
			 TUint aCSeq,
			 RPointerArray<CSIPRecordRouteHeader>& aRecordRoute,
			 RPointerArray<CSIPContactHeader>& aContact);

	~CTransactionHeaders();

    CURIContainer* RequestURI();

	CSIPToHeader* ToHeader();

	CSIPFromHeader* FromHeader();

	CSIPCallIDHeader* CallIDHeader();

	TUint CSeqNumber() const;

	RPointerArray<CSIPRouteHeaderBase>* RecordRoute();

	RPointerArray<CSIPContactHeader>* Contact();

private:
	CTransactionHeaders(TUint aCSeq);

	void ConstructL(const CURIContainer* aRequestURI,
                    const CSIPToHeader* aTo,
				    const CSIPFromHeader* aFrom,
					const CSIPCallIDHeader* aCallID,
					RPointerArray<CSIPRecordRouteHeader>& aRecordRoute,
					RPointerArray<CSIPContactHeader>& aContact);

    CURIContainer*    iRequestURI; //can be NULL
	CSIPToHeader*     iTo; //can be NULL
	CSIPFromHeader*	  iFrom; //can be NULL
	CSIPCallIDHeader* iCallID; //can be NULL

	//CSeq sequence number
	TUint iCSeq;

	RPointerArray<CSIPRouteHeaderBase>* iRecordRoute;
	RPointerArray<CSIPContactHeader>* iContact;


#ifdef CPPUNIT_TEST
	friend class CTransactionUser_Test;
#endif
	};

#endif // end of CTRANSACTIONHEADERS_H

// End of File
