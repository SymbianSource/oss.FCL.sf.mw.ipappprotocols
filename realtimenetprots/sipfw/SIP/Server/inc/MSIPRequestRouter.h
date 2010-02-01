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
* Name          : MSIPRequestRouter.h
* Part of       : SIP Server Core
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPREQUESTROUTER_H
#define MSIPREQUESTROUTER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MSIPRequestRouterObserver;
class CSIPRequest;

// CLASS DECLARATION
/**
* An interface for finding transaction owners for incoming SIP requests.
*
*  @lib sipserver.exe
*/
class MSIPRequestRouter
    {
	public:

    	virtual ~MSIPRequestRouter () { }

		/**
     	* An asynchronous function to get the owner for an incoming SIP request.
     	*
     	* @param aRequest a SIP request
     	* @param aIapId the id of the IAP where the request was received from.
     	* @param aObs the observer to be called when the asynchronous 
     	*        request completes.
     	* @returns a request ID to be used when matching 
     	*          the asynchronous request completion callback.
	 	*/
    	virtual TUint32 TransactionOwnerL (CSIPRequest& aRequest,
                                           TUint32 aIapId,
                                           MSIPRequestRouterObserver& aObs) = 0;

   
		/**
	 	* Cancels an outstanding routing request.
     	*
     	* @param aRequestId identifies the request to be cancelled.
     	*        Returned earlier from TransactionOwnerL.
	 	*/    
    	virtual void Cancel (TUint32 aRequestId) = 0;
    };

#endif // MSIPREQUESTROUTER_H

// End of File
