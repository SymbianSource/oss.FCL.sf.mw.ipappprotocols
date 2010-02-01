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
* Name        : csipresponsesender.h
* Part of     : SIP Client
* Interface   : SDK API, SIP API
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPRESPONSESENDER_H
#define CSIPRESPONSESENDER_H

//  INCLUDES
#include <e32base.h>
#include "msipresponsesender.h"

// CLASS DECLARATION

/**
*  For sending SIP responses outside dialog
*
*  @lib n/a
*/
class CSIPResponseSender : public CBase, public MSIPResponseSender
	{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.	
        */

		static CSIPResponseSender* NewL();

		/** 
		* Destructor
		*/
		~CSIPResponseSender();

    public: // From MSIPResponseSender

        void SendResponseL(CSIPClientConnection& aConnection,                                   
                           const CSIPResponseElements& aElements,
						   TUint32 aRequestId,
                           TBool aAffectsDialogState,
                           TBool aTargetRefresh);

		CSIPDialogImplementation* Dialog() const;

    private:
        CSIPResponseSender();
    };

#endif
