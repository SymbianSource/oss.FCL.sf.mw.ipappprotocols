/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofilequeueitem.h
* Part of     : sip profile agent
* Interface   : 
* Version     : SIP 4.1
*
*/




/**
 @internalComponent
*/

#ifndef TSIPPROFILEQUEUEITEM_H
#define TSIPPROFILEQUEUEITEM_H

//  INCLUDES
#include <e32std.h>

//FORWARD DECLARATIONS
class CSIPConcreteProfile;

// CLASS DECLARATION
/**
* An item for queue of pending profile registartions 
* @lib sipprofilefsm.lib
*/
class TSIPProfileQueueItem
	{
	public:  // Constructors and destructor

        /**
        * Constructor
		* @param aConcreteProfile the SIP profile
		* @param aRetryRegistration tells wheteher 
		*        an initial registration or a retry should be attempted.
        */
		TSIPProfileQueueItem(
			CSIPConcreteProfile& aConcreteProfile,
			TBool aRetryRegistration);

	public: // Data

		CSIPConcreteProfile& iConcreteProfile;
        TBool iRetryRegistration;
	};

#include "sipprofilequeueitem.inl"

#endif // TSIPPROFILEQUEUEITEM_H
