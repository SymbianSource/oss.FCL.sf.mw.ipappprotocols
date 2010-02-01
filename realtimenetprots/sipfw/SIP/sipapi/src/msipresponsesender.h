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
* Name        : msipresponsesender.h
* Part of     : SIP Client
* Interface   : SDK API, SIP API
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef MSIPRESPONSESENDER_H
#define MSIPRESPONSESENDER_H

// FORWARD DECLARATIONS
class CSIPClientConnection;
class CSIPResponseElements;
class CSIPDialogImplementation;

// CLASS DECLARATION

/**
*  An interface implementing the sending of response within server transaction.
*
*  @lib n/a
*/
class MSIPResponseSender
	{
    public: // Constructors and destructor
		/** 
		* destructor
		*/
		virtual ~MSIPResponseSender() {}

    public: // New functions		
        /**
		* Sends a SIP response using the provided client connection.
        *
		* @param aConnection SIP client connection to use for sending
        * @param aElements Response elements
        * @param aRequestId RequestId
        * @param aAffectsDialogState ETrue if the transaction which is
        *   sending the response, has an effect on the dialog state.
        *   EFalse otherwise. Has only meaning if the transaction is
        *   associated to a dialog.
        * @param aTargetRefresh ETrue if the request to which the response is
        *   sent, is a target refresh request.
        */
        virtual void SendResponseL(CSIPClientConnection& aConnection,      
                                   const CSIPResponseElements& aElements,
								   TUint32 aRequestId,
                                   TBool aAffectsDialogState,
                                   TBool aTargetRefresh) = 0;

		/**
		* Returns the dialog.
		*
        * @return CSIPDialogImplementation* The dialog inside which the response
        *	will be sent, or NULL if there is no dialog involved.
        *	Ownership is not transferred.
		*/
		virtual CSIPDialogImplementation* Dialog() const = 0;
    };

#endif
