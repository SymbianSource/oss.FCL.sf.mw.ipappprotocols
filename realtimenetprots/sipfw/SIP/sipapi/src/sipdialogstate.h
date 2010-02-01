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
* Name        : sipdialogstate.h
* Part of     : SIPAPI
* Interface   : 
* Version     :
*
*/




/**
 @internalComponent
*/

#ifndef CSIPDIALOGSTATE_H
#define CSIPDIALOGSTATE_H

// INCLUDES
/*#include <e32base.h>
#include <stringpool.h>*/
#include "sipdialog.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPMessageElements;
class CSIPResponseElements;
class CSIPClientTransaction;
class CSIPServerTransaction;
class CSIPDialogAssocImplementation;
class CSIPInviteDialogAssoc;
class CSIPNotifyDialogAssoc;
class CSIPSubscribeDialogAssoc;
class CSIPReferDialogAssoc;
class CSIPRefresh;
class CConnectionCallback;

// CLASS DECLARATION

/*
 * Base class for all dialog states.
 */
class CDialogState : public CBase
	{
public: // Destructor

	virtual ~CDialogState();

public: // New functions

    virtual CSIPDialog::TState State() const = 0;

    virtual CSIPClientTransaction*
        SendNonTargetRefreshRequestL(const CSIPDialogImplementation& aDialog,
                                     CSIPDialogAssocImplementation& aAssoc,
                                     RStringF aMethod,
                                     CSIPMessageElements* aElements) const;

    virtual CSIPClientTransaction*
        SendInviteL(const CSIPDialogImplementation& aDialog,
                    CSIPInviteDialogAssoc& aAssoc,
                    CSIPMessageElements* aElements) const;

    virtual CSIPClientTransaction*
        SendPrackL(CSIPInviteDialogAssoc& aAssoc,
                   CSIPMessageElements* aElements) const;

    virtual CSIPClientTransaction*
        SendUpdateL(CSIPInviteDialogAssoc& aAssoc,
                    CSIPMessageElements* aElements) const;

    virtual void SendAckL(CSIPInviteDialogAssoc& aAssoc,
                          const CSIPClientTransaction& aTransaction,
			              CSIPMessageElements* aElements) const;

    virtual CSIPClientTransaction*
        SendByeL(CSIPInviteDialogAssoc& aAssoc,
                 CSIPMessageElements* aElements) const;

	virtual CSIPClientTransaction*
		SendNotifyL(CSIPNotifyDialogAssoc& aAssoc,
                    CSIPMessageElements* aElements) const;

	virtual CSIPClientTransaction*
		SendReferL(const CSIPDialogImplementation& aDialog,
				   CSIPReferDialogAssoc& aAssoc,
                   CSIPMessageElements* aElements) const;
                    	 
    virtual void SendResponseL(CSIPDialogImplementation& aDialog,
                               const CSIPResponseElements& aElements,
						       TUint32 aRequestId,
                               TBool aAffectsDialogState,
                               TBool aTargetRefresh) const;

    virtual void ConnectionLost(CSIPDialogImplementation& aDialog) const = 0;

    virtual CSIPClientTransaction*
        SendSubscribeL(const CSIPDialogImplementation& aDialog,
                       CSIPSubscribeDialogAssoc& aAssoc,
                       CSIPMessageElements* aElements,
                       CSIPRefresh* aRefresh) const;
                       
	virtual CSIPClientTransaction*
        UpdateL(const CSIPDialogImplementation& aDialog,
                CSIPSubscribeDialogAssoc& aAssoc,
                CSIPMessageElements* aElements) const;

    virtual CSIPClientTransaction*
        SendUnsubscribeL(CSIPSubscribeDialogAssoc& aAssoc,
                         CSIPMessageElements* aElements) const;

    virtual TBool
        IncomingResponseL(CSIPDialogImplementation& aDialog,
                          CSIPResponseElements* aElements,
		                  TUint32 aRequestId,
                          TUint32 aDialogId,
                          CConnectionCallback& aCallback) const;

    virtual TBool
        IncomingResponseL(CSIPDialogImplementation& aDialog,
                          CSIPResponseElements* aElements,
		                  TUint32 aRequestId,
                          TUint32 aRefreshId,
                          TUint32 aDialogId,
                          CConnectionCallback& aCallback) const;

    virtual TBool
        IncomingRequestL(CSIPDialogImplementation& aDialog,
                         CSIPServerTransaction* aTransaction,
                         CConnectionCallback& aCallback) const;                                  

    virtual TBool ErrorOccured(CSIPDialogImplementation& aDialog,
                               TInt aError,
                               TUint32 aRequestId,
                               CConnectionCallback& aCallback) const;

    virtual TBool ErrorOccured(CSIPDialogImplementation& aDialog,
                               TInt aError,
                               TUint32 aRefreshId,
                               TUint32 aRequestId,
                               CConnectionCallback& aCallback) const;

protected: // New functions

    TBool HandleErrorOccurred(CSIPDialogImplementation& aDialog,
                              TInt aError,
                              TUint32 aRequestId,
                              CConnectionCallback& aCallback,
                              CDialogState& aTerminated) const;

    TBool HandleRefreshErrorOccurred(CSIPDialogImplementation& aDialog,
                              		 TInt aError,                              
                              		 TUint32 aRefreshId,
                              		 CConnectionCallback& aCallback,
                              		 CDialogState& aTerminated) const;

    TBool RequestReceivedL(CSIPDialogImplementation& aDialog,
                           CSIPServerTransaction* aTransaction,                               
                           CConnectionCallback& aCallback) const;

    void SendResponseWithinDialogL(CSIPDialogImplementation& aDialog,
                                   const CSIPResponseElements& aElements,
				    		       TUint32 aRequestId,
                                   TBool aTargetRefresh) const;

    /**
	* A response within dialog has been been received. The response might
    * create a new dialog if INVITE has forked.
    *
    * @param aDialog Dialog instance
    * @param aElements Response elements
    * @param aRequestId Identifies the transaction    
    * @param aDialogId DialogId
    * @param aCallback For selecting a callback function and its parameters
    * @param aEarly Early state of dialog state machine
    * @param aConfirmed Confirmed state of dialog state machine
    * @param aTerminated Terminated state of dialog state machine    
    *
    * @return ETrue if the caller should make a callback function call,
    *   EFalse otherwise
	*/
    TBool IncomingResponseWithinDialogL(CSIPDialogImplementation& aDialog,
                                      CSIPResponseElements* aElements,
		                              TUint32 aRequestId,
                                      TUint32 aDialogId,
                                      CConnectionCallback& aCallback,
                                      const CDialogState& aEarly,
                                      const CDialogState& aConfirmed,
                                      const CDialogState& aTerminated) const;

    CSIPInviteDialogAssoc*
        CheckIfForkingL(CSIPDialogImplementation& aDialog,
                        TUint32 aDialogId,
                        CSIPResponseElements* aElements,
                        CSIPClientTransaction& aTransaction) const;

    CSIPInviteDialogAssoc*
        CreateForkedInviteAssocLC(CSIPDialogImplementation& aDialog,
                                  CSIPResponseElements& aElements) const;

    /**
	* A response to a refreshed request has been received.
    *
    * @param aDialog Dialog instance
    * @param aElements Response elements
    * @param aRequestId Identifies the transaction
    * @param aRefreshId Identifies the refresh
    * @param aCallback For selecting a callback function and its parameters
    * @param aEarly Early state of dialog state machine
    * @param aConfirmed Confirmed state of dialog state machine
    * @param aTerminated Terminated state of dialog state machine    
    *
    * @return ETrue if the caller should make a callback function call,
    *   EFalse otherwise
	*/
    TBool ResponseToRefreshL(CSIPDialogImplementation& aDialog,
                             CSIPResponseElements* aElements,
                             TUint32 aRequestId,
                             TUint32 aRefreshId,
                             CConnectionCallback& aCallback,
                             const CDialogState& aEarly,
                             const CDialogState& aConfirmed,
                             const CDialogState& aTerminated) const;

private: // New functions

	CSIPClientTransaction* InvalidStateL() const;

private: // For testing purposes

    UNIT_TEST(CSIP_Test)
	};

#endif
