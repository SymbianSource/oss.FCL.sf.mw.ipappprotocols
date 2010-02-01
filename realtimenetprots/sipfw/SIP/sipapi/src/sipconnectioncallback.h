/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipconnectioncallback.h
* Part of     : SIPAPI
* Interface   : 
* Version     :
*
*/




/**
 @internalComponent
*/

#ifndef SIPCONNECTIONCALLBACK_H
#define SIPCONNECTIONCALLBACK_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MSIPConnectionObserver;
class CSIPTransactionBase;
class CSIPRegistrationBinding;
class CSIPDialog;
class CSIPDialogAssocBase;

// CLASS DECLARATION

/*
 * This class is used for making a delayed callback through
 * MSIPConnectionObserver. Calling the MSIPConnectionObserver is delayed to
 * avoid making the call while call stack contains methods from CSIPDialog or
 * CSIPRegistrationBinding objects since application is allowed to delete
 * CSIPRegistrationBinding and dialog association objects (when the last
 * dialog association of a CSIPDialog is deleted, also the CSIPDialog is
 * deleted).
 */
class CConnectionCallback : public CBase
	{
    public:
        //Identify which method of the MSIPConnectionObserver will be called.
        //Only those methods of the MSIPConnectionObserver, which are used by
        //CSIPRegistrationBinding or CSIPDialog have a constant here.
        enum TCallbackMethod
			{
            ENone = 0,
            EIncomingRequest,
            EIncomingResponse,
            EIncomingResponseNewAssoc,
            EIncomingResponseInRegistration,
            EErrorOccuredTransaction,
            EErrorOccuredDialog,
            EErrorOccuredDialogTransaction,
            EErrorOccuredInRegistration,
            EErrorOccuredInRegistrationWithTransaction,
            EInviteCanceled
            };

	public : // Constructors and destructor

        static CConnectionCallback* NewL();

        ~CConnectionCallback();

    public: // New functions
    
        /**
		* Selects the callback method and sets the parameters to be passed for
        * the callback. The callback is not yet called.
        *
        * @pre aCallbackId != ENone
		* @param aCallbackId Identifies the callback method to be used
        * @param aTransaction Transaction to be passed to the callback or NULL
        *   if the callback doesn't take a transaction as a parameter.
        *   Depending on the selected callback, the ownership may be
        *   transferred.
        * @param aRegistration Registration to be passed to the callback or NULL
        *   if the callback doesn't take a registration as a parameter.
        *   Ownership is not transferred.
        * @param aDialog Dialog to be passed to callback or NULL if the
        *   callback doesn't take a dialog as a parameter. Ownership is not
        *   transferred.
        * @param aDialogAssoc Dialog association to be passed to the callback or
        *   NULL if the callback doesn't take a dialog association as a
        *   parameter. Depending on the selected callback, the ownership may be
        *   transferred.
        * @param aError Error value to be passed to the callback. Not all
        *   callbacks take this as their parameter
        * @param aObjectToDelete Instructs CConnectionCallback to delete the
        *	object pointed by aObjectToDelete, after the callback method has
        *	been called. aObjectToDelete typically points to one of the other
        *	parameters of Set() method. Ownership is transferred.
		*/
        void Set(TCallbackMethod aCallbackId,
                 CSIPTransactionBase* aTransaction,
                 CSIPRegistrationBinding* aRegistration = NULL,
                 CSIPDialog* aDialog = NULL,
                 CSIPDialogAssocBase* aDialogAssoc = NULL,
                 TInt aError = KErrNone,
                 CBase* aObjectToDelete = NULL);       

        /**
		* Makes a call to the selected callback method, passing the appropriate
        * parameters to it. After the callback, if an object has been ordered
        * to be deleted using the SetObjectToDelete(), it is deleted.
        * Before calling this method, Set() must've been used to select the
        * callback and to set the parameters. Once ExecuteCallback() has been
        * called, it can't be called again before another Set() has been called.
        *
        * @pre iCallbackId != ENone
        * @post iObjectToDelete == NULL
		* @param aObserver Callback interface
        */
        void ExecuteCallback(MSIPConnectionObserver& aObserver);

    private:

        CConnectionCallback();
        
        void CallIncomingRequest(MSIPConnectionObserver& aObserver);

        void CallIncomingResponse(MSIPConnectionObserver& aObserver);

        void CallIncomingResponseNewAssoc(MSIPConnectionObserver& aObserver);

        void CallIncomingResponseInRegistration(
            MSIPConnectionObserver& aObserver);

        void CallErrorOccuredTransaction(MSIPConnectionObserver& aObserver);

        void CallErrorOccuredDialog(MSIPConnectionObserver& aObserver);

        void CallErrorOccuredDialogTransaction(
										MSIPConnectionObserver& aObserver);

        void CallErrorOccuredInRegistration(MSIPConnectionObserver& aObserver);

        void CallErrorOccuredInRegistrationWithTransaction(
            MSIPConnectionObserver& aObserver);

		void CallInviteCanceled(MSIPConnectionObserver& aObserver);

	private: // Data

        //Indicates the callback method to use
        TCallbackMethod iCallbackId;

        //These pointers are interpreted as parameters to the selected
        //callback method. CConnectionCallback doesn't own any of these.
        CSIPTransactionBase* iTransaction;
        CSIPRegistrationBinding* iRegistration;
        CSIPDialog* iDialog;
        CSIPDialogAssocBase* iDialogAssoc;

        //Error code passed to certain callback methods
        TInt iError;

        //If a parameter needs to be deleted after the callback has been done,
        //user will add it here. This object is owned by CConnectionCallback.
        CBase* iObjectToDelete;
    };

#endif
