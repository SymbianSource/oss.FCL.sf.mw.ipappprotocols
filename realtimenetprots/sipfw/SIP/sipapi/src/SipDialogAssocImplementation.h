/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : SipDialogAssocImplementation.h
* Part of     : SIP Client
* Interface   : SDK API, SIP Client API
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPDIALOGASSOCIMPLEMENTATION_H
#define CSIPDIALOGASSOCIMPLEMENTATION_H

// INCLUDES
#include <stringpool.h>
#include "_sipcodecdefs.h"
#include "transactionassociation.h"
#include "siprefreshassociation.h"

// FORWARD DECLARATIONS
class CSIPDialogAssocBase;
class CSIPClientTransaction;
class CSIPRefresh;
class CSIPDialog;
class CSIPMessageElements;
class CSIPServerTransaction;
class CSIPHeaderBase;
class TMessageHeaderCleanup;
class CSIPDialogImplementation;
class CSIPConnectionImplementation;

// CLASS DECLARATION

/**
*  Implementation class of CSIPDialogAssocBase.
*/
class CSIPDialogAssocImplementation :
    public CBase,
    public MTransactionAssociation,
    public MSIPRefreshAssociation
	{
	public: // Constructors and destructor

		static CSIPDialogAssocImplementation*
			NewL(RStringF aType,
				 CSIPDialogAssocBase& aAssoc,
				 CSIPDialog& aDialog);

		static CSIPDialogAssocImplementation*
			NewL(RStringF aType,
				 CSIPDialogAssocBase& aAssoc,
				 CSIPDialog& aDialog,
				 CSIPServerTransaction& aTransaction);
										
		/**
		* Destructor
		*/
		virtual ~CSIPDialogAssocImplementation();    

	public: //From MTransactionAssociation

        CSIPClientConnection& ClientConnectionL();
        
        CSIPClientConnection* ClientConnection();

        CSIPConnection& SIPConnectionL();

        void AddTransactionL(CSIPTransactionBase& aTransaction);

        void RemoveTransaction(const CSIPTransactionBase& aTransaction);

	public: //From MSIPRefreshAssociation

        CSIPClientTransaction*
            UpdateRefreshL(CSIPRefresh& aRefresh,
                           CSIPMessageElements* aElements,
                           TBool aTerminate);

        void DeletingRefresh(CSIPRefresh& aRefresh, TUint32 aRefreshId);
        
        MTransactionAssociation& TransactionAssociation();

		TInt CheckIfStandAlone();

	public: // New functions

		/**
		* Gets the dialog this dialog association belongs to
		* @return associated dialog
		*/
		const CSIPDialog& Dialog() const;

		/**
		* Gets dialog this dialog association belongs to
		* @return associated dialog
		*/
		CSIPDialog& Dialog();

		/**
		* Creates SIP request and sends it to the remote target.
		* This function should be used for sending SIP extension
		* requests within dialog assocation that do not cause 
		* sip dialog associations to be created. In SIP terms
		* this request is non target refresh request.
		* @pre SIPDialog().State() != CSIPDialog::EInit &&
		*      SIPDialog().State() != CSIPDialog::ETerminated
        * @pre Dialog().Connection().State() == EActive
		* @pre aMethod != "REGISTER" |"INVITE" | "BYE" | "CANCEL" | "ACK" |
        *   "SUBSCRIBE" | "NOTIFY" | "UPDATE" | "PRACK"
		* @param aElements contains optional SIP message headers and body.
        *   Ownership is transferred.
		* @return SIP extension client transaction. Ownership is transferred.
		* @leave KErrSIPInvalidDialogState If dialog's state is not correct
		* @leave KErrArgument if invalid value has been set to aMethod		
		* @leave KErrSIPResourceNotAvailable if a required SIP Client API
		*	object has been deleted
		*/
		virtual CSIPClientTransaction*
            SendNonTargetRefreshRequestL(RStringF aMethod,
                                         CSIPMessageElements* aElements);

        /**
        * Gets dialog association type.
        *
        * @return dialog association type e.g. "INVITE", "SUBSCRIBE" etc...
        */
        RStringF Type() const;		
	
        /**
        * Searches for a transaction with the given aRequestId.        
        *
        * @param aRequestId RequestId of a transaction
        * @return Found transaction, or NULL if not found. Ownership is not
        *   transferred
        */
        CSIPTransactionBase* FindTransaction(TUint32 aRequestId);        

        /**
		* Checks if this dialog association has a transaction of the given
        * type.
        *
        * @param aType Transaction type
        * @param aIsClientTransaction ETrue if searching for a client
        *   transaction, EFalse if searching for a server transaction
		* @return ETrue if such a transaction exists, EFalse otherwise
		*/
        TBool HasTransaction(RStringF aType, TBool aIsClientTransaction);

        /**
        * The connection used by dialog association has been lost.
        * Those classes derived from CSIPDialogAssocImplementation, which can contain
        * refreshes must implement this function for terminating the refresh.                
        */
        virtual void ConnectionLost();       	

		/**
        * Makes a copy of aHeader and adds it into aElements and also into
        * headerCleanup.
        * @param aHeader SIP header
        * @param aElements Message elements
        * @param aHeaderCleanup Information to detach and delete the copy of
        *	aHeader from aElements in case leave occurs.
        */
		static void CopyHeaderL(const CSIPHeaderBase& aHeader,
								CSIPMessageElements& aElements,
								TMessageHeaderCleanup& aHeaderCleanup);

	protected:

        /*
		* Constructor
		*/
        CSIPDialogAssocImplementation(CSIPDialogAssocBase& aAssoc,
        							  CSIPDialog& aDialog);
        void ConstructL(RStringF aType);
        void ConstructL(RStringF aType, CSIPServerTransaction& aTransaction);

    private: // Data

        //Dialog association type
        RStringF iType;

		CSIPDialogAssocBase& iAssoc;

        CSIPDialog& iDialog;

        //Existing transactions that are related to this dialog association.
        //CSIPDialogAssocImplementation doesn't own them.
        RPointerArray<CSIPTransactionBase> iTransactions;

		TBool iStringPoolOpened;

	private: // For testing purposes

        UNIT_TEST(CSIPInviteDialogAssoc_Test)
	};

#endif
