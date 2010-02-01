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
* Name        : sipdialogterminated.h
* Part of     : SIPAPI
* Interface   : 
* Version     :
*
*/




/**
 @internalComponent
*/

#ifndef CDIALOGTERMINATED_H
#define CDIALOGTERMINATED_H

// INCLUDES
#include "sipdialogstate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CDialogTerminated : public CDialogState
	{
public:
    static CDialogTerminated* NewL();
	~CDialogTerminated();

    CSIPDialog::TState State() const;	


    //Possible events in this state    

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

protected:
	CDialogTerminated();
	};

#endif
