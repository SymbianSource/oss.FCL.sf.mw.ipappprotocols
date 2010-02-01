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
* Name          : MSipCSSession.h
* Part of       : SIP Server Core
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPCSSESSION_H
#define MSIPCSSESSION_H

//  INCLUDES
#include <e32std.h>
#include "SipStackServerDefs.h"

// FORWARD DECLARATIONS
class MTransactionOwner;
class MSIPSecUser;

// CLASS DECLARATION
/**
* An interface for querying SIP C/S session related information. 
*
*  @lib sipserver.exe
*/
class MSipCSSession
    {
	public:
	
	    virtual MTransactionOwner& FindOwner(TUint32 aIapId) = 0;

        virtual const MSIPSecUser* SIPSecUser(
            const TRegistrationId& aRegistrationId) = 0;
	    
	    virtual TBool ByPassSIPSec() const = 0;  
    };

#endif // MSIPCSSESSION_H

// End of File
