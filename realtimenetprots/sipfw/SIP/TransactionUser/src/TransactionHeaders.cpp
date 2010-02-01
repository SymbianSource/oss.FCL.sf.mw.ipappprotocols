// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : TransactionHeaders.cpp
// Part of       : TransactionUser
// Version       : SIP/4.0
//



#include "uricontainer.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipcallidheader.h"
#include "siprecordrouteheader.h"
#include "sipcontactheader.h"

#include "CTransactionHeaders.h"

#ifdef CPPUNIT_TEST
#include "TestCleanupStack.h"
#endif

// -----------------------------------------------------------------------------
// CTransactionHeaders::NewL
// -----------------------------------------------------------------------------
//
CTransactionHeaders*
CTransactionHeaders::NewL(const CURIContainer* aRequestURI,
                          const CSIPToHeader* aTo,
						  const CSIPFromHeader* aFrom,
						  const CSIPCallIDHeader* aCallID,
						  TUint aCSeq,
						  RPointerArray<CSIPRecordRouteHeader>& aRecordRoute,
						  RPointerArray<CSIPContactHeader>& aContact)
	{
	CTransactionHeaders* self = new (ELeave) CTransactionHeaders(aCSeq);
	CleanupStack::PushL(self);
	self->ConstructL(aRequestURI, aTo, aFrom, aCallID, aRecordRoute, aContact);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTransactionHeaders::CTransactionHeaders
// -----------------------------------------------------------------------------
//
CTransactionHeaders::CTransactionHeaders(TUint aCSeq) : iCSeq(aCSeq)
	{
	}

// -----------------------------------------------------------------------------
// CTransactionHeaders::ConstructL
// -----------------------------------------------------------------------------
//
void CTransactionHeaders::ConstructL(const CURIContainer* aRequestURI,
                         const CSIPToHeader* aTo,
					     const CSIPFromHeader* aFrom,
					     const CSIPCallIDHeader* aCallID,
					     RPointerArray<CSIPRecordRouteHeader>& aRecordRoute,
					     RPointerArray<CSIPContactHeader>& aContact)
	{
    if (aRequestURI)
        {
        iRequestURI = CURIContainer::NewL(*aRequestURI);
        }

	if (aTo)
		{
		iTo = CSIPToHeader::NewL(*aTo);
		}

	if (aFrom)
		{
		iFrom = CSIPFromHeader::NewL(*aFrom);
		}

	if (aCallID)
		{		
		iCallID = static_cast<CSIPCallIDHeader*>(aCallID->CloneL());
		}

	if (aRecordRoute.Count() > 0)
		{
		iRecordRoute = new (ELeave) RPointerArray<CSIPRouteHeaderBase>(        
#ifdef CPPUNIT_TEST
            //For unit tests the granularity of arrays is set to 1 to cause
            //them to allocate memory every time an item is appended to array    
            1
#endif      
            );

		for (TInt i = 0; i < aRecordRoute.Count(); i++)
			{
			CSIPRecordRouteHeader* rr =
                static_cast<CSIPRecordRouteHeader*>(aRecordRoute[i]->CloneL());
            CleanupStack::PushL(rr);
			iRecordRoute->AppendL(rr);
			CleanupStack::Pop(rr);
			}
		}

	if (aContact.Count() > 0)
		{
		iContact = new (ELeave) RPointerArray<CSIPContactHeader>(
#ifdef CPPUNIT_TEST
            1
#endif      
            );

		for (TInt i = 0; i < aContact.Count(); i++)
			{
			CSIPContactHeader* contact =
				static_cast<CSIPContactHeader*>(aContact[i]->CloneL());
			CleanupStack::PushL(contact);
			iContact->AppendL(contact);
			CleanupStack::Pop(contact);
			}
		}
	}

// -----------------------------------------------------------------------------
// CTransactionHeaders::~CTransactionHeaders
// -----------------------------------------------------------------------------
//
CTransactionHeaders::~CTransactionHeaders()
	{
    delete iRequestURI;
	delete iTo;
	delete iFrom;
	delete iCallID;

	if (iRecordRoute)
		{
		iRecordRoute->ResetAndDestroy();
		}
	delete iRecordRoute;

	if (iContact)
		{
		iContact->ResetAndDestroy();
		}
	delete iContact;
	}

// -----------------------------------------------------------------------------
// CTransactionHeaders::RequestURI
// -----------------------------------------------------------------------------
//
CURIContainer* CTransactionHeaders::RequestURI()
    {
    CURIContainer* requestURI = iRequestURI;
    iRequestURI = NULL;
    return requestURI;
    }

// -----------------------------------------------------------------------------
// CTransactionHeaders::ToHeader
// -----------------------------------------------------------------------------
//
CSIPToHeader* CTransactionHeaders::ToHeader()
	{
	CSIPToHeader* to = iTo;	
	iTo = NULL;
	return to;
	}

// -----------------------------------------------------------------------------
// CTransactionHeaders::FromHeader
// -----------------------------------------------------------------------------
//
CSIPFromHeader* CTransactionHeaders::FromHeader()
	{
	CSIPFromHeader* from = iFrom;
	iFrom = NULL;
	return from;
	}

// -----------------------------------------------------------------------------
// CTransactionHeaders::CallIDHeader
// -----------------------------------------------------------------------------
//
CSIPCallIDHeader* CTransactionHeaders::CallIDHeader()
	{
	CSIPCallIDHeader* callID = iCallID;
	iCallID = NULL;
	return callID;
	}

// -----------------------------------------------------------------------------
// CTransactionHeaders::CSeqNumber
// -----------------------------------------------------------------------------
//
TUint CTransactionHeaders::CSeqNumber() const
	{
	return iCSeq;
	}

// -----------------------------------------------------------------------------
// CTransactionHeaders::RecordRoute
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPRouteHeaderBase>* CTransactionHeaders::RecordRoute()
	{
	RPointerArray<CSIPRouteHeaderBase>* recordRoute = iRecordRoute;
	iRecordRoute = NULL;
	return recordRoute;
	}

// -----------------------------------------------------------------------------
// CTransactionHeaders::Contact
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPContactHeader>* CTransactionHeaders::Contact()
	{
	RPointerArray<CSIPContactHeader>* contact = iContact;
	iContact = NULL;
	return contact;
	}
