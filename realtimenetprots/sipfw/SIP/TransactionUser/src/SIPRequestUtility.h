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
* Name          : SIPRequestUtility.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPREQUESTUTILITY_H
#define SIPREQUESTUTILITY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CURIContainer;
class CSIPRequest;
class CRouteSet;
class CSIPRouteHeader;

// CLASS DECLARATION
/*
 * Miscellaneous SIP request related functions
 */
class SIPRequestUtility
	{
public:
    enum TMaxForwards
		{
		/** Max-forwards header is filled with this value */
		EMaxForwardsValue = 70
        };

    /**
	 * Checks that the request contains everything required to be filled by the
	 * upper layers. If request isn't ok, function leaves.
	 *
	 * @param aReq SIP request
	 */	
	static void CheckOutgoingRequestL(CSIPRequest& aReq);

    /**
	 * Fills the Request-URI of aReq with the value in aUri.
	 *
	 * @param aReq SIP request
	 * @param aUri URI to be filled into aReq	 
	 */
	static void SetRequestUriL(CSIPRequest& aReq, const CURIContainer& aUri);

	/**
	 * Fills Route-headers and Request URI into the aReq.	 
	 *
	 * @param aReq SIP request to be filled
     * @param aRouteSet Route set
	 * @param aRemoteTarget Remote target	 
	 */	
	static void FillRouteAndRequestUriL(CSIPRequest& aReq,
								 		const CRouteSet& aRouteSet,
								 		const CURIContainer& aRemoteTarget);		

	/**
	 * Creates a Route header from the aUri.
	 *
	 * @param aUri URI to be put into Route header
	 * @return value CSIPRouteHeader* New header. Ownership is transferred.
	 */
	static CSIPRouteHeader* CreateRouteHeaderLC(const CURIContainer& aUri);

    /**
	 * Adds Max-Forwards header into the SIP request.
	 *
	 * @param aReq SIP request
	 */
	static void FillNewMaxForwardsL(CSIPRequest& aReq);

    /**
	 * Adds "Supported: sec-agree" header to SIP request.
	 *
	 * @param aReq SIP request
	 */	
    static void FillSupportedSecAgreeL(CSIPRequest& aReq);
    
    /**
	 * Cleanup method for releasing the temporary Route-headers in case a leave
     * occurs during re-ordering the Route-headers of the SIP request.
	 *
     * @param aRouteHeaders Pointer to RPointerArray holding the Route-headers
	 */
    static void CleanupRouteHeaders(TAny* aRouteHeaders);
	};

#endif // end of SIPREQUESTUTILITY_H

// End of File
