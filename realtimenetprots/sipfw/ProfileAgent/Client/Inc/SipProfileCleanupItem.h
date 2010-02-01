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
* Name        : sipprofilecleanupitem.h
* Part of     : SIP Profile Client
* Interface   : private
* The cleanup class used when roll back in case of failure
* Version     : 1.0
*
*/



#ifndef CSIPPROFILECLEANUPITEM_H
#define CSIPPROFILECLEANUPITEM_H

/**
*@internalComponent
*/

//  INCLUDES

#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPProfile;
class CSIPConcreteProfile;
class CSIPProfileRegistryBase;
class CSIPProfileITC;

// CLASS DECLARATION

/**
 * Class used with CleanupStack to provide roll back mechanism
 * in case of failure.
 */
class CSIPProfileCleanupItem : public CBase
	{
	public: 

		/** 
		* Constructor
		*/
		CSIPProfileCleanupItem(CSIPProfileITC* aITC);

		/** 
		* Clean up in case of failure
		*/
		void CrashRevertL();

	public: 

		TUint iProfileId;
		TBool iStored;
		CSIPConcreteProfile* iConcreteProfile;
		CSIPProfile* iManagedProfile;
		CSIPProfileITC* iITC;
	};

#endif
