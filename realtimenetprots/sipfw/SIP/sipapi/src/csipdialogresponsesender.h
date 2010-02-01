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
* Name        : sipdialogresponsesender.h
* Part of     : SIP Client
* Interface   : SDK API, SIP API
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPDIALOGRESPONSESENDER_H
#define CSIPDIALOGRESPONSESENDER_H

//  INCLUDES
#include <e32base.h>
#include "msipresponsesender.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  For sending responses that are related to dialog, 
*
*  @lib n/a
*/

class CSIPDialogResponseSender : public CBase, public MSIPResponseSender
	{
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.	
        */
        static CSIPDialogResponseSender*
        	NewL(CSIPDialogImplementation& aDialog);

		/** 
		* Destructor
		*/
		~CSIPDialogResponseSender();

    public: // From MSIPResponseSender
		
        void SendResponseL(CSIPClientConnection& aConnection,                                   
                           const CSIPResponseElements& aElements,
						   TUint32 aRequestId,
                           TBool aAffectsDialogState,
                           TBool aTargetRefresh);

    	CSIPDialogImplementation* Dialog() const;

    private: // Constructor
        CSIPDialogResponseSender(CSIPDialogImplementation& aDialog);

	private: // Data
	
        CSIPDialogImplementation& iDialog;
    };

#endif
