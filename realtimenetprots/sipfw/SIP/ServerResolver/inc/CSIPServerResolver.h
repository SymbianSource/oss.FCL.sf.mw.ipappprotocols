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
* Name          : CSIPServerResolver.h
* Part of       : ServerResolver
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPSERVERRESOLVER_H
#define CSIPSERVERRESOLVER_H

//INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include <stringpool.h>
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CURIBase;
class CSIPViaHeader;
class MSIPServerResolverObserver;
class CSIPHostResolver;
class CSIPHostPort;
class CSIPURI;
class MSIPResolvingResult;
class CServerQuery;
class CURIContainer;

// CLASS DEFINITION
/**
 * CSIPServerResolver implements a
 */
class CSIPServerResolver : public CBase
	{
	public:// Constructors and destructor

    	/**
	 	* Creates a new CSIPServerResolver instance
	 	* @param aServer for server session
	 	* @param aConnection for connection
	 	* @returns a new instance of CSIPServerResolver
	 	*/
		static CSIPServerResolver* NewL( RSocketServ& aServer, 
                                		 RConnection& aConnection );

    	/**
	 	* Creates a new CSIPServerResolver instance 
	 	* and pushes it to CleanupStack
	 	* @param aServer for server session
	 	* @param aConnection for connection
	 	* @returns a new instance of CSIPServerResolver
	 	*/
		static CSIPServerResolver* NewLC( RSocketServ& aServer, 
                                      	  RConnection& aConnection );

		/**
	 	* Destructor.
	 	*/
		~CSIPServerResolver();

	public:
		
		/**
	 	* Resolves a CURIBase object to 0..n MSIPResolvingResult objects
	 	* asynchronously. MSIPServerResolverObserver is called when
	 	* resolving completes.
	 	*
	 	*
 	 	* @param aURI The URI to be resolved. 
	 	*        Not used after this function returns.
	 	* @param aSkipNaptrAndSrvQueries if ETrue skip NAPTR and SRV
	 	*        queries and perform only A/AAAA query
	 	* @param aResult an array for the resolved results.
	 	* @param aObserver	callback that will be used when resolving 
	 	*    	has been completed. The ownership is not transferred.
	 	* Function accept only one same aObserver pointer at the time,
	 	* otherwise function leave with errorcode  KErrAlreadyExists
	 	*/
		void GetByURIL( const CURIContainer& aURI,
		                TBool aSkipNaptrAndSrvQueries,
						RPointerArray< MSIPResolvingResult >& aResult,
		            	MSIPServerResolverObserver* aObserver ); 

		/**
	 	* Resolves a CSIPViaHeader object to 0..n MSIPResolvingResult objects
	 	* asynchronously. MSIPServerResolverObserver is called when
	 	* resolving completes.
	 	*
 	 	* @param aVia The Via-header to be resolved. 
	 	*        Not used after this function returns.
	 	* @param aResult an array for the resolved results.
	 	* @param aObserver	callback that will be used when resolving 
	 	*        has been completed. The ownership is not transferred.
	 	* Function accept only one same aObserver pointer at the time,
	 	* otherwise function leave with errorcode  KErrAlreadyExists
	 	*/
		void GetByViaL( CSIPViaHeader& aVia,
						RPointerArray< MSIPResolvingResult >& aResult,
						MSIPServerResolverObserver* aObserver );

		/**
	 	* Cancels all outstanding operations for the given observer.
	 	*
	 	* @param aObserver	The observer.
	 	* 
	 	*/
		void Cancel( MSIPServerResolverObserver* aObserver );
	
	private:// Constructors and destructor

		CSIPServerResolver();
	
		void ConstructL( RSocketServ& aServer,
					 	 RConnection& aConnection );
					 	 
	private:// New methods
	
		void ChooseQueryL( MSIPServerResolverObserver* aObserver,
                      	   const TDesC8& aTarget,
                      	   RPointerArray< MSIPResolvingResult >& aResult,
                      	   TBool aSipsUri,
                      	   TUint aPort,
                      	   RStringF aProtocol,
                      	   TBool aForceUDP = EFalse,
                      	   TBool aSkipNaptrAndSrvQueries = EFalse );

		void SetA_AAAAQueryL( CServerQuery* aQuery );
		
		void SetSRVQueryL( CServerQuery* aQuery );
		
		void SetNaptrQueryL( CServerQuery* aQuery );
                    
        RStringF TransportProtocolL( const CSIPURI& aURI,
                                     TBool& aForceUdp ) const;
                                     
        RStringF DefaultTransport( TBool aSipsUri ) const; 
        
        TUint DefaultPort( TBool aUseTls ) const;                           
	
	private://data
	
		CSIPHostResolver* iResolver;
		
	private: // For testing purposes

		UNIT_TEST(CSIPServerResolverTest)
	};
#endif // end of CSIPSERVERRESOLVER_H

// End of File
