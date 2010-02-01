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
* Name        : sipprofileslots.h
* Part of     : SIP Profile
* Interface   : private
* The class used for ITC communication 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TSIPPROFILESLOTS_H
#define TSIPPROFILESLOTS_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Class is a holder for multiple CSIPConcreteProfile instances
 * It provides serialization and deserialization functions
 */

class TSIPProfileSlots
	{
	public:

		/**
		 * Default constructor
		 */
		TSIPProfileSlots() 
			: iProfileId(0),
			  iEventId(0),
			  iSlot1(0),
			  iSlot2(0),
			  iError(0){}
	
		/**
		 * Copy constructor
		 */
		TSIPProfileSlots(const TSIPProfileSlots& aIds) 
			: iProfileId(aIds.iProfileId),
			  iEventId(aIds.iEventId),
			  iSlot1(aIds.iSlot1),
			  iSlot2(aIds.iSlot2),
			  iError(aIds.iError){}

	public:

		TUint32 iProfileId;
		TUint32 iEventId;
		TUint32 iSlot1;
		TUint32 iSlot2;
		TInt 	iError;
	};

#endif 
