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
* Name          : sipnattraversalrequestobserver.h
* Part of       : SIP NAT Traversal Controller
* Version       : SIP/5.0 
*
*/



#ifndef MSIPNATTRAVERSALREQUESTOBSERVER_H
#define MSIPNATTRAVERSALREQUESTOBSERVER_H

// INCLUDES
#include <e32std.h>
#include <in_sock.h>

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* Callback to be implemented by the users of SIP NAT Traversal Controller.
*/
class MSIPNATTraversalRequestObserver
	{
	public:

		/**
		* The asynchronous request to NAT Traversal Controller was completed,
		* but terminal's public address is the same as its local address.
        * @param aRequestId a request ID
		* @param aCompletionCode completion code, KErrNone if successful. 
		*/
		virtual void RequestComplete(
		    TUint32 aRequestId,
		    TInt aCompletionCode ) = 0;
		    
		/**
		* The asynchronous request to NAT Traversal Controller was completed
		* and terminal's public address related to this request was resolved.
        * @param aRequestId a request ID
		* @param aPublicAddr a public IP address
		*/
		virtual void PublicAddrResolved(
		    TUint32 aRequestId,
		    const TInetAddr& aPublicAddr ) = 0;		    
	};

#endif // MSIPNATTRAVERSALREQUESTOBSERVER_H
