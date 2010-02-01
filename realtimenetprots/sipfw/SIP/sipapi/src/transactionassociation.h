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
* Name          : transactionassociation.h
* Part of       : SIPAPI
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef __TRANSACTIONASSOCIATION_H__
#define __TRANSACTIONASSOCIATION_H__


// FORWARD DECLARATIONS
class CSIPTransactionBase;
class CSIPClientConnection;
class CSIPConnection;

// CLASS DECLARATION

/**
 * @brief This interface defines methods for transaction to communicate with
 *  the object it is associated with.
 */
class MTransactionAssociation
	{
public:
    /**
	* Destructor	
	*/
    virtual ~MTransactionAssociation() {}

    /**
	* Obtains the associated CSIPClientConnection instance. If
    * CSIPClientConnection doesn't exist, function leaves with an error code.
    *	
	* @return CSIPClientConnection instance
	*/
    virtual CSIPClientConnection& ClientConnectionL() = 0;
    
    /**
	* Obtains the associated CSIPClientConnection instance.
    *	
	* @return CSIPClientConnection instance, or NULL if it doesn't exist.
	*/
    virtual CSIPClientConnection* ClientConnection() = 0;

    /**
	* Obtains the associated CSIPConnection instance. If CSIPConnection doesn't
    * exist, this function leaves.
    *	
	* @return CSIPConnection instance
	*/
    virtual CSIPConnection& SIPConnectionL() = 0;

    /**
	* Bind a transaction with the association.
	* 
	* @param aTransaction Transaction to bind.
	*/
    virtual void AddTransactionL(CSIPTransactionBase& aTransaction) = 0;

    /**
	* Clears all references to aTransaction from the association.
	* 
	* @param aTransaction Transaction to which references are removed.	
	*/
    virtual void RemoveTransaction(const CSIPTransactionBase& aTransaction) = 0;
	};

#endif // end of __TRANSACTIONASSOCIATION_H__

// End of File
