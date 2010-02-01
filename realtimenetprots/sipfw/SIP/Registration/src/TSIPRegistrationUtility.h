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
* Name          : TSIPRegistrationUtility.h
* Part of       : SIP Registration
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/



#ifndef TSIPREGISTRATIONUTILITY_H
#define TSIPREGISTRATIONUTILITY_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MTransactionUser;
class MSigComp;
class CSIPRequest;
class CSIPRegistrationBindingBase;
class CSIPRouteHeader;
class CSIPToHeader;
class CSIPContactHeader;
class CSIPAddress;


class TSIPRegistrationUtility
	{
public:

	static HBufC8* LocalAddressLC(
	    const TUint aIapId,
	    MTransactionUser& aTU);

	static void CheckRegisterRequestL(
	    CSIPRequest& aRequest,
	    const CSIPRouteHeader* aOutboundProxy, 
	    const TUint32 aIapId,
	    MTransactionUser& aTU);

	static void CheckUpdateRegisterRequestL(
		CSIPRequest& aRequest, 
		CSIPRegistrationBindingBase& aBinding);

	static void CheckUnregisterRequestL(
		CSIPRequest& aRequest, 
		CSIPRegistrationBindingBase& aBinding);

	static void CheckFetchingRequestL(CSIPRequest& aRequest);

	static void CheckOutboundProxyCompParameterL(
		CSIPRouteHeader& aOutboundProxy,
		MSigComp& aSigComp);

	static void CheckToHeaderL(CSIPToHeader& aTo);

	static void CheckRegisterContactL(
	    CSIPRequest& aRequest,
		const TUint32 aIapId,
        MTransactionUser& aTU);

	static void CheckUpdateRegisterContactL(
		CSIPRequest& aRequest,
		CSIPRegistrationBindingBase& aBinding);

	static void CheckRegisterExpiresValueL(CSIPRequest& aRequest);

	/*
	* Removes the expires param from the request message
	*/
	static void RemoveRegisterExpiresParam(CSIPRequest& aRequest);

	static void CheckUpdateRegisterExpiresValueL(
		CSIPRequest& aRequest,
		CSIPRegistrationBindingBase& aBinding);

	static void CheckSIPAddressInContactHeaderL(
		CSIPContactHeader& aContact,
		const TUint32 aIapId,
        MTransactionUser&  aTU);
        
    static void CheckSipsSchemeInAorAndOtherAddressL(
        const CSIPToHeader& aAor,
        const CSIPAddress& aAddress);
    };

#endif // TSIPREGISTRATIONUTILITY_H

// End of File
