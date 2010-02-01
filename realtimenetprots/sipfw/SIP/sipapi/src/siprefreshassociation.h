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
* Name          : siprefreshassociation.h
* Part of       : SIPAPI
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPREFRESHASSOCIATION_H
#define CSIPREFRESHASSOCIATION_H

// FORWARD DECLARATIONS

class CSIPRefresh;
class CSIPMessageElements;
class CSIPClientTransaction;
class MTransactionAssociation;

// CLASS DECLARATION

/**
 * @brief This interface defines methods for CSIPRefresh to communicate with
 *  the object which owns it.
 *
 *  This class is for internal use only. 
 */
class MSIPRefreshAssociation
	{
public:

    /**
	* Destructor	
	*/
    virtual ~MSIPRefreshAssociation() {}

    /**
	* Update or terminate the refresh.
    *
    * @param aRefresh Refresh instance requesting the operation
	* @param aElements Optional SIP message headers and body. Ownership is
    *   transferred.
    * @param aTerminate ETrue if the refresh is terminated, EFalse if it is
    *	updated.
    * @return Client transaction. Ownership is transferred.
    * @leave KErrNotFound if no such refresh exists        
    * @leave KErrSIPInvalidRegistrationState if trying to access a REGISTER
    *		 refresh. 
    * @leave KSIPErrInvalidDialogState if trying to access a dialog
    *		 association being refreshed.
	*/
    virtual CSIPClientTransaction*
        UpdateRefreshL(CSIPRefresh& aRefresh,
                       CSIPMessageElements* aElements,
                       TBool aTerminate) = 0;

	/**
	* When CSIPRefresh is deleted, it detaches itself from the associated
	* object, using this function.
    *
    * @param aRefresh Refresh instance being deleted
	* @param aRefreshId Refresh identifier
	*/
    virtual void DeletingRefresh(CSIPRefresh& aRefresh,
                                 TUint32 aRefreshId) = 0;

    /**
	* Obtains a handle to MTransactionAssociation.
	*
	* @return MTransactionAssociation handle
	*/
	virtual MTransactionAssociation& TransactionAssociation() = 0;
		
	/**
	* Determines if the request being refreshed is a stand-alone request.
	*
	* @return KErrNone if stand-alone
	*		  KErrSIPInvalidRegistrationState if request is REGISTER
	*		  KErrSIPInvalidDialogState if request is related to a dialog
	*		  association	
	*/
	virtual TInt CheckIfStandAlone() = 0;
	};

#endif

// End of File
