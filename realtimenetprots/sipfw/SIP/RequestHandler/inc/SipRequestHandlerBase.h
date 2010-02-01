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
* Name          : SipRequestHandlerBase.h
* Part of       : SIP Request Handler
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREQUESTHANDLERBASE_H
#define CSIPREQUESTHANDLERBASE_H

#include <e32base.h>
#include <ecom/ecom.h>
class CSIPRequest;
class MSIPRequestHandlerObserver;

const TUid KSIPRequestHandlerIFUid = { 0x101fed86 };


class CSIPRequestHandlerBase : public CBase
    {
	public: // Constructors and destructor
	
		/**
		* Destructor.
		*/
		virtual inline ~CSIPRequestHandlerBase();

    public: // New functions
    
		/**
		* Registers handled client
		* @param aChannel the communication channel UID of the connected client
		*/
		virtual void RegisterL(TUid aChannel) = 0;

		/**
		* Deregisters handled client
		* @param aChannel the communication channel UID of the connected client
		* @return KErrNone if successful, KErrNotFound if not found
		*/
		virtual TInt Deregister(TUid aChannel) = 0;

		/**
		* Request to provide the communication channel for the SIP request
		* received outside the SIP dialog
		* @param aSIPRequest SIP request
		* @return a request ID
		*/
		virtual TUint32 ChannelL(CSIPRequest& aSIPRequest) = 0;
		
		/**
		* Cancels a particular outstanding request
		* @param aRequestId a request ID to be canceled
		*/
		virtual void Cancel(TUint32 aRequestId) = 0;

	public: // Data

		/// Unique key for implementations of this interface.
		TUid iInstanceKey;

    protected: // Constructors:

        inline CSIPRequestHandlerBase(MSIPRequestHandlerObserver& aObserver);
        
    protected: // Data
    
    	MSIPRequestHandlerObserver& iObserver;
    };
    
#include "SipRequestHandlerBase.inl"

#endif // CSIPREQUESTHANDLERBASE_H
