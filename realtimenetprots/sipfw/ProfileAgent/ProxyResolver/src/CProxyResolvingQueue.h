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
* Name          : CProxyResolvingQueue.h
* Part of       : ProxyResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CPROXYRESOLVINGQUEUE_H
#define CPROXYRESOLVINGQUEUE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CProxyQuery;

const TUint KMinProxyResolveRequestId=1;
const TUint KMaxProxyResolveRequestId=KMaxTUint;

class CProxyResolvingQueue : public CBase
    {
public:

    static CProxyResolvingQueue* NewL();
    static CProxyResolvingQueue* NewLC();
    
    ~CProxyResolvingQueue ();
    
    // add a query
    void AddQuery( CProxyQuery& aQuery );
    // get next query
    CProxyQuery* NextQuery();
    // get next query which is opened
    CProxyQuery* NextReadyQuery();
    // detach this query from the queue (but do not delete)
    void DetachByRequestId( TUint aRequestId ); 
    // remove this query from the queue (and delete)
    void DeleteByRequestId( TUint aRequestId ); 
    // get ID for the next request
    TUint NextRequestId();

private:

    CProxyResolvingQueue();
    void ConstructL();
    
    CProxyQuery* FindQueryByRequestId( TUint aRequestId );
    
private: // Data

    TSglQue< CProxyQuery > iList;
    TSglQueIter< CProxyQuery > iListIter;    
    TUint iNextRequestId;
    
private:

    #ifdef CPPUNIT_TEST
        friend class CProxyResolvingQueueTest;
    #endif
    };

#endif // CPROXYRESOLVINGQUEUE_H

// End of File
