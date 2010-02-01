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
* Name          : MSIPSecSecurityMechanismObserver.h
* Part of       : SIPSec
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_MECHANISM_OBSERVER_H__
#define __SIPSEC_MECHANISM_OBSERVER_H__

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class MSIPSecSecurityMechanismObserver
	{
public: // New pure virtual functions	

 	/**
    * SIPSec plugin has completed the asynchronous processing of a SIP response
    * that was initiated in MSIPSecSecurityMechanism::ResponseReceivedL.
    * @param aSuccess Tells if the response was successfully processed.
    * @param aUseSecurityServerHeaders ETrue if SIPSec FW can use the
    *		 Security-Server headers present in the SIP response.
    *		 EFalse otherwise
    */
	virtual void CacheUpdated( TBool aSuccess,
							   TBool aUseSecurityServerHeaders ) = 0;

	/**
    * SIPSec plugin has completed the asynchronous processing of a SIP response
    * that was initiated in MSIPSecSecurityMechanism::ParametersUpdatedL.
    * @param aSuccess Tells if the processing result is successful.
    */
	virtual void SAReady( TBool aSuccess ) = 0;
	};

#endif
