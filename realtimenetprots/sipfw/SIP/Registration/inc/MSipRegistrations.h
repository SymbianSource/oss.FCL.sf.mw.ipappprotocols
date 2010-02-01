/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MSipRegistrations.h
* Part of       : Registration
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPREGISTRATIONS_H
#define MSIPREGISTRATIONS_H

class CURIContainer;
class CSIPRouteHeader;

#include "SipStackServerDefs.h"

/**
 * An interface for R_URI checking and getting outbound proxies.
 */
class MSipRegistrations
	{
public: 
	virtual ~MSipRegistrations() {}

	/** 
	 * Function checks if the R_URI passed as a parameter 
	 * equals to the Uri in one of contact headers in the bindings.
	 *
	 * @param  aRequestUri; a request uri.
	 * @return ETrue if the R_URI is recognized and 
	 *         EFalse if it is not recognized.
	 */
	virtual TBool CheckRequestURI (CURIContainer& aRequestUri) = 0;
	
	/** 
	 * Function gets an outbound proxy for a specified TRegistartionId.
	 *
	 * @pre   HasOutboundProxy(TRegistartionId) returns ETrue;
	 * @param aRegistartionId
	 * @return outbound proxy related to the registration id,
	 *         or NULL if not found
	 */
	virtual const CSIPRouteHeader* OutboundProxy (
		TRegistrationId aRegistrationId) = 0;	 

	/** 
	 * Function checks if the outbound proxy exists for a 
	 * specified TRegistartionId.
	 *
	 * @param aRegistartionId
	 * @return ETrue if the outbound proxy exists for a specified 
	 *         TRegistartionId, other wise EFalse.
	 */
	virtual TBool HasOutboundProxy(
		TRegistrationId aRegistrationId) const = 0;

    /** 
	 * Function checks if the given SIP-URI 
     * is a URI of ANY outbound proxy stored in Registrations.
	 *
	 * @param aUri the URI to check
	 * @return ETrue if the URI is a URI of an outbound proxy, 
	 *         otherwise EFalse.
	 */
	virtual TBool IsOutboundProxy (const CURIContainer& aUri) = 0;
	
    /** 
	 * Resolving or sending a SIP request to a URI has failed.
	 * If this is an outbound proxy URI of an existing registration binding,
	 * Registrations removes the corresponding binding immediately. 
	 *
	 * @param aUri the failed URI before resolving
	 */
	virtual void URIFailed (CURIContainer& aUri) = 0;	
	
    /** 
	 * Checks if outboud proxy IP caching is enabled.
	 *
	 * @param aRegistartionId
	 * @return ETrue if the outbound proxy ip caching is enabled 
	 *         for the specified TRegistartionId, other wise EFalse.
	 */
	virtual TBool IsCacheOutboundProxyIPEanbled(
			TRegistrationId aRegistrationId) const = 0;
	
	/** 
	 * Function gets an outbound proxy for a specified TRegistartionId.
	 *
	 * @param aRegistartionId
	 * @return outbound proxy related to the registration id,
	 */
	virtual CSIPRouteHeader& OutboundProxyIPL(
			TRegistrationId aRegistrationId) const = 0;
	};

#endif // MSIPREGISTRATIONS_H

// End of File
