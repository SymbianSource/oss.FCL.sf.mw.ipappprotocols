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
* Name          : CServerQuery.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSERVERQUERY_H
#define CSERVERQUERY_H

//INCLUDES
#include <e32base.h>
#include "MSIPServerQuery.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPQueryConditionBase;
class MSIPServerResolverObserver;
class CSIPQueryData;
class MSIPHostResolver;

// CLASS DEFINITION
/**
 * CServerQuery implements a
 */
class CServerQuery : public CBase, public MSIPServerQuery
	{
	public:// Constructors and destructor

		/**
	 	* Creates a new CServerQuery instance
	 	* @param aObserver callback that will be used when resolving 
	 	*    	has been completed. The ownership is not transferred.
	 	* @param aQueryData for storing the query data. 
	 	*		The ownership is transferred.
	 	* @returns a new instance of CServerQuery
	 	*/
		static CServerQuery* NewL( MSIPServerResolverObserver* aObserver,
							  	   CSIPQueryData* aQueryData );

    	/**
	 	* Creates a new CServerQuery instance 
	 	* and pushes it to CleanupStack
	 	* @param aObserver callback that will be used when resolving 
	 	*    	has been completed. The ownership is not transferred.
	 	* @param aQueryData for storing the query data. 
	 	*		The ownership is transferred.
	 	* @returns a new instance of CServerQuery
	 	*/
		static CServerQuery* NewLC( MSIPServerResolverObserver* aObserver, 
							   	    CSIPQueryData* aQueryData );
							   	    
		/**
	 	* Destructor.
	 	*/			   	    
		~CServerQuery();
					   
	public:// From MSIPServerQuery
	
		CSIPQueryData& QueryData();

	public:
	
		void Query( MSIPHostResolver& aResolver );
		 
		TBool HandleQueryResultL( TInt aStatus );
		 
		MSIPServerResolverObserver* ServerResolverObserver(); 
	
		void SetQueryConditionL( CSIPQueryConditionBase* aQueryCondition ); 
		
	public:// Data
	
		static const TInt iOffset;
		
		TSglQueLink iLink;

	private:// Constructors and destructor

		void ConstructL( MSIPServerResolverObserver* aObserver, 
					 	 CSIPQueryData* aQueryData );
					 	 
		CServerQuery();
		
	private:// Data
	
		CSIPQueryConditionBase* iQueryCondition;
		
		//Not owned
		MSIPServerResolverObserver* iObserver;
		
		CSIPQueryData* iQueryData;
		
	private: // For testing purposes

		UNIT_TEST( CServerQueryTest)	
    
	};
#endif // end of CSERVERQUERY_H

// End of File
