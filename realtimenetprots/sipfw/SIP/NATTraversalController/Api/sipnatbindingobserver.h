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
* Name          : sipnatbindingobserver.h
* Part of       : SIP NAT Traversal Controller
* Version       : SIP/5.0 
*
*/



#ifndef MSIPNATBINDINGOBSERVER_H
#define MSIPNATBINDINGOBSERVER_H

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* Callback to receive notifications about flow failures for a NAT binding.
*/
class MSIPNATBindingObserver
	{
	public:

		/**
		* The flow failed for a NAT binding.
		* @param aError a reason why the flow failed. 
		*/
		virtual void FlowFailure( TInt aError ) = 0;
	};

#endif // MSIPNATBINDINGOBSERVER_H
