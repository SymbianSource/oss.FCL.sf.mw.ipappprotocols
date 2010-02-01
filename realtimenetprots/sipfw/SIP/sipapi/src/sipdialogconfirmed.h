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
* Name        : sipdialogconfirmed.h
* Part of     : SIPAPI
* Interface   : 
* Version     :
*
*/




/**
 @internalComponent
*/

#ifndef CDIALOGCONFIRMED_H
#define CDIALOGCONFIRMED_H

// INCLUDES

#include "sipdialogstate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CDialogConfirmed : public CDialogState
	{
public: // Constructor and destructor

    static CDialogConfirmed* NewL();
	~CDialogConfirmed();

public: // New functions

    CSIPDialog::TState State() const;

	void SetNeighbourStates(CDialogState& aTerminated);


    //Possible events in this state

    CSIPClientTransaction*
        SendInviteL(const CSIPDialogImplementation& aDialog,
                    CSIPInviteDialogAssoc& aAssoc,
                    CSIPMessageElements* aElements) const;

    CSIPClientTransaction*
        SendSubscribeL(const CSIPDialogImplementation& aDialog,
                       CSIPSubscribeDialogAssoc& aAssoc,
                       CSIPMessageElements* aElements,
                       CSIPRefresh* aRefresh) const;

	CSIPClientTransaction*
		UpdateL(const CSIPDialogImplementation& aDialog,
                CSIPSubscribeDialogAssoc& aAssoc,
                CSIPMessageElements* aElements) const;
                	  
	CSIPClientTransaction*
		SendReferL(const CSIPDialogImplementation& aDialog,
				   CSIPReferDialogAssoc& aAssoc,
                   CSIPMessageElements* aElements) const;

    CSIPClientTransaction*
        SendNonTargetRefreshRequestL(const CSIPDialogImplementation& aDialog,
                                     CSIPDialogAssocImplementation& aAssoc,
                                     RStringF aMethod,
                                     CSIPMessageElements* aElements) const;

    TBool IncomingResponseL(CSIPDialogImplementation& aDialog,
                            CSIPResponseElements* aElements,
		                    TUint32 aRequestId,
                            TUint32 aDialogId,
                            CConnectionCallback& aCallback) const;

    TBool IncomingResponseL(CSIPDialogImplementation& aDialog,
                            CSIPResponseElements* aElements,
		                    TUint32 aRequestId,
                            TUint32 aRefreshId,
                            TUint32 aDialogId,
                            CConnectionCallback& aCallback) const;

    TBool IncomingRequestL(CSIPDialogImplementation& aDialog,
                           CSIPServerTransaction* aTransaction,
                           CConnectionCallback& aCallback) const;                          

    void SendAckL(CSIPInviteDialogAssoc& aAssoc,
                  const CSIPClientTransaction& aTransaction,
			      CSIPMessageElements* aElements) const;

    void SendResponseL(CSIPDialogImplementation& aDialog,
                       const CSIPResponseElements& aElements,
					   TUint32 aRequestId,
                       TBool aAffectsDialogState,
                       TBool aTargetRefresh) const;

    CSIPClientTransaction* SendPrackL(CSIPInviteDialogAssoc& aAssoc,
                                      CSIPMessageElements* aElements) const;

    CSIPClientTransaction* SendUpdateL(CSIPInviteDialogAssoc& aAssoc,
                                       CSIPMessageElements* aElements) const;

    CSIPClientTransaction* SendByeL(CSIPInviteDialogAssoc& aAssoc,
                                    CSIPMessageElements* aElements) const;

    CSIPClientTransaction*
        SendUnsubscribeL(CSIPSubscribeDialogAssoc& aAssoc,
                         CSIPMessageElements* aElements) const;

	CSIPClientTransaction* SendNotifyL(CSIPNotifyDialogAssoc& aAssoc,
                    				   CSIPMessageElements* aElements) const;
                    
    TBool ErrorOccured(CSIPDialogImplementation& aDialog,
                       TInt aError,
                       TUint32 aRequestId,
                       CConnectionCallback& aCallback) const;

    TBool ErrorOccured(CSIPDialogImplementation& aDialog,
                       TInt aError,
                       TUint32 aRefreshId,
                       TUint32 aRequestId,
                       CConnectionCallback& aCallback) const;

    void ConnectionLost(CSIPDialogImplementation& aDialog) const;

private: //Data

	CDialogConfirmed();

    //States which can be reached from Early state    
    CDialogState* iTerminated;
	};

#endif
