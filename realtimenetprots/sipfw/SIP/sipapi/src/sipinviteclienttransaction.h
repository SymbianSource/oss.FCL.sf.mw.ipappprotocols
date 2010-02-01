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
* Name        : sipinviteclienttransaction.h
* Part of     : SIP Client
* Interface   :
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef CSIPINVITECLIENTTRANSACTION_H
#define CSIPINVITECLIENTTRANSACTION_H

// INCLUDES
#include "sipclienttransaction.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class for managing SIP INVITE client transactions.
*  It provides means for associating the transaction simultaneously with
*  multiple dialog associations, in case of forking INVITE.
*
*  @lib sipclient
*/
class CSIPInviteClientTransaction: public CSIPClientTransaction
	{
	public: // Destructor

        /**
        * Destructor
		*/
		IMPORT_C ~CSIPInviteClientTransaction();

    public: // From CSIPClientTransaction

        IMPORT_C TBool CancelAllowed() const;

	public: // From CSIPTransactionBase, for internal use

        void Detach(const MTransactionAssociation& aAssociation);
        
	public: // Constructors, for internal use

        /**
		* Instantiates a CSIPInviteClientTransaction object, leaves on failure.
        *        
        * @param aAssociation Object to which the transaction is associated
        *   with.
		* @return SIP INVITE client transaction, ownership is transferred.
		*/
		static CSIPInviteClientTransaction*
            NewL(MTransactionAssociation& aAssociation);

        /**
		* Instantiates a CSIPInviteClientTransaction object and pushes it into
        * CleanupStack, leaves on failure.
        *
        * @param aAssociation Object to which the transaction is associated
        *   with.
		* @return SIP client transaction, ownership is transferred.
		*/
		static CSIPInviteClientTransaction*
            NewLC(MTransactionAssociation& aAssociation);

	public: // New functions, for internal use

        void AddAssociationL(MTransactionAssociation& aAssociation);

	private: // Constructors

		CSIPInviteClientTransaction(MTransactionAssociation& aAssociation);

        void ConstructL();

	private: // Data
	
        //CSIPInviteClientTransaction may be associated to multiple invite
        //dialog associations when INVITE has forked.
        //
        //CSIPInviteClientTransaction sets CSIPTransactionBase::iAssociation
        //to point to the first item in iAssociationArray, so that the
        //methods in base classes accessing CSIPTransactionBase::iAssociation
        //work.
        RPointerArray<MTransactionAssociation> iAssociationArray;
	};

#endif
