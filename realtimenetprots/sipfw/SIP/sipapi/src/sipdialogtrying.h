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
* Name        : sipdialogtrying.h
* Part of     : SIPAPI
* Interface   : 
* Version     :
*
*/




/**
 @internalComponent
*/

#ifndef CDIALOGTRYING_H
#define CDIALOGTRYING_H

// INCLUDES
#include "sipdialogstate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CDialogTrying : public CDialogState
	{
public: // Constructors and destructor

    static CDialogTrying* NewL();
	~CDialogTrying();

public: // From CDialogState

    CSIPDialog::TState State() const;
    
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
		SendReferL(const CSIPDialogImplementation& aDialog,
				   CSIPReferDialogAssoc& aAssoc,
                   CSIPMessageElements* aElements) const;
                   
    void SendResponseL(CSIPDialogImplementation& aDialog,
                       const CSIPResponseElements& aElements,
					   TUint32 aRequestId,
                       TBool aAffectsDialogState,
                       TBool aTargetRefresh) const;    

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

	void SetNeighbourStates(CDialogState& aEarly,
                            CDialogState& aConfirmed,
                            CDialogState& aTerminated);

private: // Constructor

	CDialogTrying();

private: // Data

    //States which can be reached from Trying state    
    CDialogState* iEarly;
    CDialogState* iConfirmed;
    CDialogState* iTerminated;

private: // For testing purposes

    UNIT_TEST(CSIPDialogTrying_Test)
	};

#endif
