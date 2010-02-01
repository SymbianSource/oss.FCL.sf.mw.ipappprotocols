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
* Name          : MSIPRequestRouterObserver.h
* Part of       : SIP Server Core
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef MSIPREQUESTROUTEROBSERVER_H
#define MSIPREQUESTROUTEROBSERVER_H

#include <e32base.h>
class MTransactionOwner;
class CSIPResponse;


class MSIPRequestRouterObserver
	{
	public:

		/**
		* Destructor.
    	*/
    	virtual ~MSIPRequestRouterObserver () { }
    
		/**
		* An owner for the incoming SIP request was found
		* @param aRequestId the request ID
		* @param aOwner the resolved owner. The ownership is not transferred.
    	*/
		virtual void OwnerFoundL(TUint32 aRequestId,
		                         MTransactionOwner* aOwner) = 0;

		/**
		* An owner was not found.
        * The implementation of this callback should generate 
        * a SIP error response with the provided elements and 
        * send it to network.
		* @param aRequestId a request ID
		* @param aSIPResponse a SIP error response. 
		*        The ownership is transferred.
		*/
		virtual void OwnerNotFoundL(TUint32 aRequestId,
								    CSIPResponse* aSIPResponse) = 0;

		/**
		* Error occurred during resolution.
        * This callback will also be called if one of the callbacks 
        * OwnerFoundL or OwnerNotFoundL leaves using the leave code.
        * @param aRequestId a request ID
		* @param aError error code. 
		*/
		virtual void ErrorOccurred(TUint32 aRequestId, TInt aError) =0;
	};

#endif // MSIPREQUESTROUTEROBSERVER_H
