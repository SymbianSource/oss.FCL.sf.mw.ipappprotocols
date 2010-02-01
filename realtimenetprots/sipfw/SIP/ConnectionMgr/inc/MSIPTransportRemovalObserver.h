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
* Name          : MSIPTransportRemovalObserver.h
* Part of       : SIPSec
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPTRANSPORTREMOVALOBSERVER_H
#define MSIPTRANSPORTREMOVALOBSERVER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION
/**
* An interface for callbacks of asynchronous transport removing.
*
*  @lib N/A
*/
class MSIPTransportRemovalObserver
    {
	public:
	
		/** 
		* Removal completed.
		*/
		virtual void RemovalCompleted( TUint32 aTransportId ) = 0;

	};

#endif // MSIPTRANSPORTREMOVALOBSERVER_H

// End of File
