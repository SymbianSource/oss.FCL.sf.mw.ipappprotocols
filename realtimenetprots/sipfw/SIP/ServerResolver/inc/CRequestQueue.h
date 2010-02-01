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
* Name          : CRequestQueue.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CREQUESTQUEUE_H
#define CREQUESTQUEUE_H

//INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CServerQuery;
class MSIPServerResolverObserver;

// CLASS DEFINITION
/**
 * CRequestQueue implements a
 */
class CRequestQueue : public CBase
	{
	public:// Constructors and destructor

		/**
	 	* Creates a new CRequestQueue instance
	 	* @returns a new instance of CRequestQueue
	 	*/
		static CRequestQueue* NewL();
		
		/**
	 	* Creates a new CRequestQueue instance 
	 	* and pushes it to CleanupStack
	 	* @returns a new instance of CRequestQueue
	 	*/
		static CRequestQueue* NewLC();
		
		/**
	 	* Destructor.
	 	*/
		~CRequestQueue();
		
	public:
	
		void AddL( CServerQuery& aQuery );
		
		CServerQuery* NextQuery();
		
		void CancelQuery( MSIPServerResolverObserver* aObserver );

	private:// Constructors and destructor

		CRequestQueue();
		
	private:// data
	
		TSglQue<CServerQuery> iList;
		
		TSglQueIter<CServerQuery> iListIter;

	private: // For testing purposes

		UNIT_TEST(CHostResolverTest)
	};

#endif // end of CREQUESTQUEUE_H

// End of File
