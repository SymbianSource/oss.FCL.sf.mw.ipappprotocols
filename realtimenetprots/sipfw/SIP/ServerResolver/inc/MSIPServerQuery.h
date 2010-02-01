/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MSIPServerQuery.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPSERVERQUERY_H
#define MSIPSERVERQUERY_H

//INCLUDES
#include <e32std.h>
#include <in_sock.h>
#include <es_sock.h>

// FORWARD DECLARATIONS
class CSIPQueryData;

class MSIPServerQuery
	{
	public:

    	/**
     	* Destructor.
	 	*/
    	virtual ~MSIPServerQuery() {}

    	/**
     	* Gets the reference of CSIPQueryData.
     	*
     	* @returns the CSIPQueryData
     	*/
		virtual CSIPQueryData& QueryData() = 0;
	};

#endif // MSIPSERVERQUERY_H
