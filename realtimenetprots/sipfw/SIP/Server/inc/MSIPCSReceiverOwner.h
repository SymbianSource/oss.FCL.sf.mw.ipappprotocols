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
* Name          : MSIPCSReceiverOwner.h
* Part of       : SIP / SIP Server Core
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPCSRECEIVEROWNER_H
#define MSIPCSRECEIVEROWNER_H

//  INCLUDES
#include <e32std.h>
#include "SipStackServerDefs.h"
#include "TSIPTransportParams.h"

// FORWARD DECLARATIONS
class MSIPSecUser;

// CLASS DECLARATION
/**
* A callback for a client/server receiver to fetch information from its owner.
*
*  @lib sipserver.exe
*/
class MSIPCSReceiverOwner
    {
	public:

		/**
     	* Gets the transport parameters
     	*
     	* @returns the transport parameters
	 	*/
    	virtual const TSIPTransportParams& TransportParams() const = 0;

		/**
     	* Gets the transport parameters related to the registration.
     	*
     	* @param aRegistrationId a registration ID. Can be zero.
     	*
     	* @returns the transport parameters
	 	*/
    	virtual TSIPTransportParams TransportParams(
    	    const TRegistrationId& aRegistrationId) = 0;
    	    
	    /**
	    * Gets the MSIPSecUser related to the given registration id.
        *
        * @param aRegistrationId the registration id
        *
        * @returns the user related to the given registration id if found,
        *          otherwise NULL.
	    */    
        virtual const MSIPSecUser* SIPSecUser(
            const TRegistrationId& aRegistrationId) = 0; 
    };

#endif // MSIPCSRECEIVEROWNER_H

// End of File
