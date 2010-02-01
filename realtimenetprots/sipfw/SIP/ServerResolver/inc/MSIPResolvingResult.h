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
* Name          : MSIPResolvingResult.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef MSIPRESOLVINGRESULT_H
#define MSIPRESOLVINGRESULT_H

//INCLUDES
#include <e32std.h>
#include <in_sock.h>

class RStringF;

class MSIPResolvingResult
	{
	public:

    	/**
     	* Destructor.
	 	*/
    	virtual ~MSIPResolvingResult () {}

    	/**
     	* Gets the resolved address.
     	*
     	* @returns the resolved address
     	*/
		virtual TInetAddr& Address() = 0;

    	/**
     	* Gets the resolved protocol.
     	*
     	* @returns the resolved protocol
     	*/
		virtual RStringF Protocol() = 0;
		
		/**
     	* Sets this address failed.
     	* Must be called when a transport error related to
     	* this result occures.
     	* @param aFailed the address has failed
     	*/
		virtual void SetFailed(TBool aFailed) = 0;

    	/**
     	* Checks if sending to the address has failed.
     	* @return ETrue if sending to the address has failed.
     	*         Otherwise EFalse.
     	*/
		virtual TBool Failed() = 0;

		/**
     	* Checks if the protocol can not be changed and must remain UDP.
     	* @return ETrue must use UDP.
     	*	      Otherwise EFalse.
     	*/
		virtual TBool ForceUDP() = 0;
	};

#endif // MSIPRESOLVINGRESULT_H
