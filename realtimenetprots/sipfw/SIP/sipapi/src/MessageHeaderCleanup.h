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
* Name        : MessageHeaderCleanup.h
* Part of     : SIP Client
* Interface   : SDK API, SIP Client API
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef MESSAGEHEADERCLEANUP_H
#define MESSAGEHEADERCLEANUP_H


//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPHeaderBase;
class CSIPMessageElements;


/**
* Class for detaching user headers from CSIPMessageElements in case leave
* occurs.
*/
class TMessageHeaderCleanup
	{
	public:
		TMessageHeaderCleanup(CSIPMessageElements& aElements);		

		void AddHeader(CSIPHeaderBase& aHeader);

		static void Cleanup(TAny* aHeaderCleanup);

	private:
		CSIPMessageElements& iElements;
		
		CSIPHeaderBase* iHeader;
		CSIPHeaderBase* iHeader2;
		CSIPHeaderBase* iHeader3;
	};

#endif
