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
* Name          : RecordCleanup.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef RECORDCLEANUP_H
#define RECORDCLEANUP_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CSIPSecUserRecord;
class CSIPSecDigestObserver;


/**
 * This class is a container for passing parameters to cleanup function.
 */
class TRecordCleanup
	{
public:
	/**
	 * Constructor
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aRecord Record from where aObserver is removed in case leave
	 *		  occurs
	 * @param aObserver Observer to remove
	 */
	TRecordCleanup( CSIPSecUserRecord& aRecord,
    			    CSIPSecDigestObserver& aObserver );    

public: // Data

	//Record from where aObserver is removed
    CSIPSecUserRecord& iRecord;
    
    //Observer to remove
	CSIPSecDigestObserver& iObserver;
	};

#endif // end of RECORDCLEANUP_H

// End of File
