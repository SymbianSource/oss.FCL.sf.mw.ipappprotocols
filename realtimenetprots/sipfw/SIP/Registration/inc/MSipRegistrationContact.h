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
* Name          : MSipRegistrationContact.h
* Part of       : Registration
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPREGISTRATIONCONTACT_H
#define MSIPREGISTRATIONCONTACT_H

#include "SipStackServerDefs.h"

class CSIPContactHeader;
class CSIPFromToHeaderBase;

/**
 * An interface to query a contact header.
 */
class MSipRegistrationContact
	{
public:

   /**
    * Fills the hostport in the Contact-header.
	* If the user part is not filled in the Contact-header,
	* fills it with a copy from a registration binding (if found)
	* or from the From-/To-header. 
	*
    * @param aIapId the IAP id indicating which local IP address 
    *        to use when filling the host part of the Contact-header
    * @param aRegistrationId the registration ID
	* @param aFromToHeader; the uri must be a SIP uri                     
	* @param aContactHeader; the uri must be a SIP uri
	*/
	virtual void ContactL(TUint32                     aIapId,
	                      TRegistrationId             aRegistrationId,
                          const CSIPFromToHeaderBase* aFromToHeader,
		                  CSIPContactHeader&          aContact) = 0;
	};

#endif // MSIPREGISTRATIONCONTACT_H

// End of File
