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
* Name          : RouteSet.h
* Part of       : TransactionUser
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef ROUTESET_H
#define ROUTESET_H

#include <e32base.h>

#include "SipStackServerDefs.h"

class CSIPRequest;
class CSIPHeaderBase;
class CSIPRouteHeader;
class CURIContainer;
class MSipRegistrations;


/*
 * This class is used for storing and manipulating a route set.
 */
class CRouteSet : public CBase
	{
public:	
	/**
	 * Creates the route set and copies all Route headers from the aReq.
	 * If aReq doesn't have Route headers, the possibly existing outbound
	 * proxy is added into the route set.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aReq SIP request
	 * @param aRegistrations Registration subsystem
	 * @param aRegisterId Registration identifier
	 * @return New CRouteSet object, ownership is transferred
	 */
	static CRouteSet* NewL(CSIPRequest& aReq,
						   MSipRegistrations& aRegistrations,
						   TRegistrationId aRegisterId);
						   

	/**
	 * Creates a deep copy of aRouteSet.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aRouteSet CRouteSet to be copied	 
	 * @return New CRouteSet object, ownership is transferred
	 */
	static CRouteSet* NewL(const CRouteSet& aRouteSet);


	~CRouteSet();


	/**
	 * Copies the Route-headers present in the route set, to aReq.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aReq SIP request where Route headers are added
	 * @param aIndex Indicates the first entry in route set, from where the
	 *	entries are copied.
	 *	0 = copy all entries, 1 = copy from the second entry onwards, etc.
	 * @return -
	 */
    void CopyToRequestL(CSIPRequest& aReq, TUint aIndex = 0) const;


	/**
	 * Returns the URI of the first entry of the route set.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param
	 * @return CURIContainer* URI of the first entry of the route set,
	 *	ownership is not transferred. NULL if route set is empty.
	 */
	const CURIContainer* TopUri() const;

	
	/**
	 * Replaces the topmost route with a new value.
	 *	 
	 * @pre IsEmpty() == EFalse
	 * @post
	 *
	 * @see
	 *
	 * @param aContact 
	 * @return -
	 */
	void ReplaceTopRouteL();
	
	
    /**
	 * Checks if the topmost route has LR parameter.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *	 
	 * @return ETrue if topmost route has LR parameter, EFalse if it doesn't
     *  have, or if the route set is empty.
	 */
	TBool IsLrParamInTopRoute() const;


	/**
	 * Checks if the route set is empty.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *	 
	 * @return ETrue if route set is empty, EFalse otherwise.
	 */
	TBool IsEmpty() const;
	
	
	/**
	 * Checks if the original request contained route headers when the request
	 * first was passed to TransactionUser.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *	 
	 * @return ETrue if preconfigured route exists, EFalse otherwise.
	 */
	TBool PreconfigRouteExists() const;
	
	
	/**
	 * Returns the next hop URI.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aRemoteTarget Remote target
	 * @return CURIContainer& Next hop URI
	 */
	CURIContainer& NextHop(CURIContainer& aRemoteTarget) const;


	/**
	 * Sets a aRoute as the first route entry.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aRoute Route to be added
	 * @param aOverwrite ETrue if aRoute will replace the current first route
	 *	EFalse if aRoute is appended to the beginning	 
	 */
	void AddToBeginningL(const CSIPRouteHeader& aRoute, TBool aOverwrite);


	/**
	 * Check if the URI is present in the route set.
	 *
	 * @param aUri URI
	 * @return ETrue: aUri is present in the route set,
	 *	EFalse: otherwise
	 */
	TBool IsInSet(const CURIContainer& aUri) const;

private:
	CRouteSet();

	void ConstructL(CSIPRequest& aReq,
					MSipRegistrations& aRegistrations,
				    TRegistrationId aRegisterId);

	void ConstructL(const CRouteSet& aRouteSet);


	/**
	 * Creates a copy of aRouteHeader and adds it the route set.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aRouteHeader Route header to be added	 
	 */
	void AddRouteL(const CSIPHeaderBase& aRouteHeader);


    //Route headers of the route set
    RPointerArray<CSIPRouteHeader> iRoutes;
    
    //ETrue if a preconfigured route set exists (=the original request contained
    //Route headers filled by upper layer).
    TBool iPreconfigRouteExists;

#ifdef CPPUNIT_TEST	
	friend class CRouteSet_Test;
	friend class CUserAgentClient_Test;
#endif
	};

#endif // end of ROUTESET_H

// End of File
