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
* Name          : CSIPHostResolver.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPHOSTRESOLVER_H
#define CSIPHOSTRESOLVER_H

//INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "MSIPHostResolver.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CRequestQueue;
class CServerQuery;
class MSIPServerResolverObserver;

// CLASS DEFINITION
/**
 * CSIPHostResolver implements a
 */
class CSIPHostResolver : public CActive, public MSIPHostResolver
	{
	public:// Constructors and destructor

		/**
	 	* Creates a new CSIPHostResolver instance
	 	* @param aServer for server session
	 	* @param aConnection for connection
	 	* @returns a new instance of CSIPHostResolver
	 	*/
		static CSIPHostResolver* NewL( RSocketServ& aServer, 
                                   	   RConnection& aConnection );
	    /**
	 	* Creates a new CSIPHostResolver instance 
	 	* and pushes it to CleanupStack
	 	* @param aServer for server session
	 	* @param aConnection for connection
	 	* @returns a new instance of CSIPHostResolver
	 	*/
		static CSIPHostResolver* NewLC( RSocketServ& aServer, 
                                    	RConnection& aConnection );
                                    	
		/**
	 	* Destructor.
	 	*/
		~CSIPHostResolver();
		
	public:// From MSIPHostResolver
	
		RHostResolver& Resolver();
		
		TRequestStatus& RequestStatus();
		
		void SetIPListFailed( TBool aFailed );

	public:
	
		void GetByQueryL( CServerQuery* aQuery );	

		void CancelGetByURI( MSIPServerResolverObserver* aObserver );

		void RunL(); 

		TInt RunError( TInt aError );

		void DoCancel();

	private:// Constructors and destructor

		void ConstructL( RSocketServ& aServer, RConnection& aConnection );

		CSIPHostResolver();
		
	private:

		void ResolveNext();

	private:// Data
		
		CRequestQueue* iRequestQueue;

		CServerQuery* iQuery;

		RHostResolver iResolver;
		
		TBool iObserverCancel;
		
		TBool iFailed;
		
	private: // For testing purposes

		UNIT_TEST(CSIPHostResolverTest)
		UNIT_TEST(CSIPServerResolverTest)
	};

#endif // end of CSIPHOSTRESOLVER_H

// End of File
