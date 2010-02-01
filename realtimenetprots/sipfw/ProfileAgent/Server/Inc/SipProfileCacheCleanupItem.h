/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofilecachecleanupitem.h
* Part of     : SIP Profile Server
* Interface   : private
* The class used for roll back with CleanupStack in case of failure
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef CSIPPROFILECACHECLEANUPITEM_H
#define CSIPPROFILECACHECLEANUPITEM_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * The class used for roll back with CleanupStack in case of failure
 */
class CSIPProfileCacheCleanupItem : public CBase
	{
	public:

	    /**
        * Cleans up resources in case of failure during
		* adding of profile. Keeps store and memory consistent.
        */
		inline void CrashRevert() 
			{
			if (iCache)
				{
				for (int i = 0; i < iCache->Count(); i++)
					{
					if ((*(iCache))[i] == iCacheItem)
						{
						iCache->Remove(i);
						delete iCacheItem;
                                                iCacheItem = NULL;
						}
					}
				}
			else
				{
				delete iCacheItem;
				}
			}

	public: // Data

		// Owned
		CSIPProfileCacheItem* iCacheItem;
		// Not owned
		RPointerArray<CSIPProfileCacheItem>* iCache;
	};

#endif
