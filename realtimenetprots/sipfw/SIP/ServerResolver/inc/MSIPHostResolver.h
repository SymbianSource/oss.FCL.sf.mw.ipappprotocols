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
* Name          : MSIPHostResolver.h
* Part of       : ServerResolver
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPHOSTRESOLVER_H
#define MSIPHOSTRESOLVER_H

//INCLUDES
#include <e32std.h>

class MSIPHostResolver
	{
	public:

    	/**
     	* Destructor.
	 	*/
    	virtual ~MSIPHostResolver () {}

    	/**
     	* Gets the reference of RHostResolver.
     	*
     	* @returns the RHostResolver
     	*/
		virtual RHostResolver& Resolver() = 0;
		
		virtual TRequestStatus& RequestStatus() = 0;
		
		virtual void SetIPListFailed( TBool aFailed ) = 0;
	};

#endif // MSIPHOSTRESOLVER_H
