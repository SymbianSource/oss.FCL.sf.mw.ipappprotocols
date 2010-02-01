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
* Name        : sipdialogearly.h
* Part of     : SIPAPI
* Interface   : 
* Version     :
*
*/




/**
 @internalComponent
*/

#ifndef CDIALOGEARLY_H
#define CDIALOGEARLY_H

// INCLUDES
#include "sipdialogstate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CDialogEarly : public CDialogState
	{
public: // Constructors and destructor

    static CDialogEarly* NewL();
	~CDialogEarly();

public: // From CDialogState

    CSIPDialog::TState State() const;


    //Possible events in this state    

    //INVITE can not be sent in Early state

    CSIPClientTransaction*
        SendSubscribeL(const CSIPDialogImplementation& aDialog,
                       CSIPSubscribeDialogAssoc& aAssoc,
                       CSIPMessageElements* aElements,
                       CSIPRefresh* aRefresh) const;

    CSIPClientTransaction*
        SendNonTargetRefreshRequestL(const CSIPDialogImplementation& aDialog,
                                     CSIPDialogAssocImplementation& aAssoc,
                                     RStringF aMethod,
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

public: // New functions
	void SetNeighbourStates(CDialogState& aConfirmed,
                            CDialogState& aTerminated);

private: // Constructor

	CDialogEarly();

private: // Data

    //States which can be reached from Early state
    CDialogState* iConfirmed;
    CDialogState* iTerminated;
	};

#endif
