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
* Name          : MSipDialogs.h
* Part of       : SIPDialogs
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPDIALOGS_H
#define MSIPDIALOGS_H

#include "SipStackServerDefs.h"

class MTransactionOwner;
class CSIPRequest;
class CSIPResponse;


/**
 * This class defines transaction owner for a dialog.
 * Transaction uses this class.
 */
class MSipDialogs
	{
    public:
	    
	    /**
	    * Destructor.
	    */
	    virtual ~MSipDialogs() { }

	    /**
	    * Method for getting transaction owner for the dialog defined 
	    * in the request. If request is malformed, call-id matches one 
	    * of dialogs but to-tag or from-tag not match dialog's defined ones,
	    * error response is created and transaction owner is equal to 0.
        *
	    * @param aRequest SIP request
	    * @param aResponse SIP response
	    * @return aTransactionOwner Transaction owner or 0 if dialog not exists.
	    */
	    virtual MTransactionOwner* TransactionOwnerL(
	        CSIPRequest& aRequest,
	        CSIPResponse** aResponse) = 0;
	};

#endif // MSIPDIALOGS_H

// End of File
