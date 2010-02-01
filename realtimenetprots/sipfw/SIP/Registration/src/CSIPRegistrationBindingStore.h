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
* Name          : CSIPRegistrationBindingStore.h
* Part of       : Registration
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGISTRATIONBINDINGSTORE_H
#define CSIPREGISTRATIONBINDINGSTORE_H

#include <e32base.h>
#include "SipStackServerDefs.h"
#include "_sipcodecdefs.h"

class CSIPRegistrarStore;
class CSIPRegistrationBindingBase;
class CSIPFromToHeaderBase;
class CSIPRequest;
class CSIPContactHeader;
class CURIContainer;
class CSIPRouteHeader;
class CDeleteMgr;
class MRegistrationOwner;


class CSIPRegistrationBindingStore : public CBase
	{
public:
         
   /** 
	* Create a instance of CSIPRegistrationBindingStore.
	* 
	* @param aRegistrarStore; a refrence of CSIPRegistrarStore.
	*
    * @return a CSIPRegistrationBindingStore object.
	*/
	static CSIPRegistrationBindingStore* NewL(CSIPRegistrarStore& 
		                                      aRegistrarStore);

   /** 
	* Create a instance of CSIPRegistrationBindingStore 
	* and push it to CleanupStack.
	* 
	* @param aRegistrarStore; a refrence of CSIPRegistrarStore.
	*
    * @return a CSIPRegistrationBindingStore object.
	*/
	static CSIPRegistrationBindingStore* NewLC(CSIPRegistrarStore& 
		                                       aRegistrarStore);

	~CSIPRegistrationBindingStore();

   /**
    * Find the binding by giving the MTransactionOwner and register request.
	*
	* @param aRegistrationOwner; a reference of MRegistrationOwner.
	* @param aSIPRequest; a request. 
	* 
	* @return CSIPRegistrationBindingBase object. 
	*          the ownership is not transferred.
	*/
	CSIPRegistrationBindingBase* FindBinding(
		const MRegistrationOwner&  aRegistrationOwner,
		CSIPRequest&               aSIPRequest);
		
   /**
    * Find the binding by ID and owner.
	*
	* @param aRegistrationId; a TRegistrationId.
	* @param aOwner the owner that must match to the original owner
	*        used if not NULL.
	* @return CSIPRegistrationBindingBase object.
	*          the ownership is not transferred.
	*/
	CSIPRegistrationBindingBase* FindBinding(
		const TRegistrationId& aRegistrationId,
		const MRegistrationOwner* aOwner=NULL);

   /**
    * Find a registration binding by transaction id.
	* @param aTransactionId transaction id.
	* @return CSIPRegistrationBindingBase instance or NULL if not found,
	*         the ownership is not transferred.
	*/
	const CSIPRegistrationBindingBase* FindBindingForTransaction(
		const TTransactionId& aTransactionId);	

   /**
    * Remove the binding by giving a CSIPRegistrationBaseClass.
	*
	* @param aBinding; a CSIPRegistrationBindingBase object.
	*
	* @return KErrNone if the binding was found and removed. 
	*         KErrNotFound otherwise
	*/
	TInt RemoveBinding(const CSIPRegistrationBindingBase* aBinding);

   /**
    * Remove the binding by giving a IAP id.
	*
	* @param aIapId.
	*/
	void RemoveBindingByIAPId(const TUint32 aIapId);

   /**
    * Remove the binding by giving a registraion id.
	*
	* @param aRegistrationId
    * @param aOwner the owner that must match to the original owner
	* @return KErrNone if the binding was found and removed. 
	*         KErrNotFound otherwise    
	*/
	TInt RemoveBinding(const TRegistrationId& aRegistrationId,
	                   const MRegistrationOwner* aOwner);

   /**
    * when registration owner stored in binding is same as the
	* one passed as parameter. remove the binding if the binding
	* is not pending, otherwise assign RegistrationOwner contained in
	* binding to 0.
	*
	* @param aRegistrationOwner; a refernce of MRegistrationOwner.
	*
	* @return KErrNone when RegistrationOwner is found, 
	*          KErrNotFound otherwise.
	*/
	TInt ClearRegistrationOwner(const MRegistrationOwner* aRegistrationOwner);

   /**
    * Fills the hostport in the Contact-header.
	* If the user part is not filled in the Contact-header,
	* fills it with a copy from a registration binding found
	*
	* @param aFromToHeader; a CSIPFromToHeaderBase object.
	* @param aContact; a reference of CSIPContactHeader.
	*
	* @return ETrue, if binding was found, EFalse otherwise.
	*/
	TBool FindContactByFromL(const CSIPFromToHeaderBase* aFromToHeader,
		                     CSIPContactHeader& aContact);

   /**
    * Get registrar store.
	*
	* @return CSIPRegistrarStore; a refrence of CSIPRegistrarStore.
	*/
	CSIPRegistrarStore& RegistrarStore();

   /**
    * Get next TRegistrationId.
	*
	* @return TRegistrationId.
	*/
	TRegistrationId NextRegistrationId ();

   /**
    * Checks if the RequestURI passed as a parameter 
	* equals to the Uri in one of contact headers in the bindings.
	*
	* @param aRequestUri; a refrence of CURIContainer
	*
	* @return ETrue if the parameter equals the uri in contact of binding.
	*         EFalse if none of uri in contact of binding equals the
	*         parameter uri.
	*/
	TBool CheckRequestURI(CURIContainer& aRequestUri);

	/** 
	 * Function sets an outbound proxy for a registration binding.
	 * Leaves if the binding was not found. 
	 *
	 * @pre aOutboundProxy != 0
	 * @param  aRegistrationId; the id of the registration binding
	 *         for which the outbound proxy should be set.
	 * @param  aOutboundProxy; an outbound proxy to be used with the
	 *         binding to be created. The ownership is transferred.
	 * @param  aOwner the owner that must match to the original owner         
	 */	
	void SetOutboundProxyL(const TRegistrationId& aRegistrationId,
	                       CSIPRouteHeader* aOutboundProxy,
	                       const MRegistrationOwner* aOwner);
	                        
   /**
    * Get OutboundProxy from the binding found by passing TRegistrationId.
	*
	* @pre HasOutboundProxy(aRegistrationId) == ETrue
	* @param aRegistrationId; TRegistrationId
	* @return outbound proxy or NULL if not found, 
	*         the ownership is not transferred
	*/
	const CSIPRouteHeader* OutboundProxy(TRegistrationId aRegistrationId);

   /**
    * check if OutboundProxy exists from the binding by passing 
	* TRegistrationId.
	*
	* @param aRegistrationId; TRegistrationId
	* @return ETrue if out bound proxy exits for the binding found by 
	*         TRegistrationId, EFalse otherwise.
	*/
	TBool HasOutboundProxy(TRegistrationId aRegistrationId);

   /** 
    * Function checks if the given SIP-URI 
    * is a URI of ANY outbound proxy stored in Registrations.
	*
	* @param aUri the URI to check
	* @return ETrue if the URI is a URI of an outbound proxy, 
	*         otherwise EFalse.
	*/
    TBool IsOutboundProxy(const CURIContainer& aUri);

   /**
    * Remove the OutboundProxy from the binding if it can be found by 
	* using the registrationid passed as parameter.
	*
	* @param aRegistrationId; TRegistrationId
	* @param  aOwner the owner that must match to the original owner
	* @return KErrNone if the binding can be found and removed.
	*/
	TInt RemoveOutboundProxy(const TRegistrationId& aRegistrationId,
	                         const MRegistrationOwner* aOwner);
	
    /** 
	 * Resolving or sending a SIP request to a URI has failed.
	 * If this is an outbound proxy URI of an existing registration binding,
	 * removes the corresponding binding immediately. 
	 *
	 * @param aUri the failed URI before resolving
	 */	
	void URIFailed(CURIContainer& aUri);

   /**
    * find pending registrtions to the same AOR given as parameter.
	*/
	TBool RegisterPendingToAOR (const CURIContainer& aAOR);

	void AddBindingL(CSIPRegistrationBindingBase* aBinding);
	
	void DetachBinding(CSIPRegistrationBindingBase* aBinding);

private:

	CSIPRegistrationBindingStore(CSIPRegistrarStore& aRegistrarStore);
	void ConstructL();
	
	/** 
	 * Compares the two URIs
	 * It checks the host part and port part of the URI
	 * If the port is default port, the check is ignored
	 *
	 * @param aProxy the outboundproxy in the reg binding
	 * @param aUri the failed URI before resolving
	 */	
	TBool CompareUri(const CURIContainer& aProxy, CURIContainer& aUri);
	
	
private: // Data

	// bindings array
	RPointerArray<CSIPRegistrationBindingBase> iBindings;

	// a refrence to store of registrars array
	CSIPRegistrarStore& iRegistrarStore;

	// new registrationId given to new binding
	TRegistrationId     iNextRegistrationId;

	CDeleteMgr*         iDeleteMgr;

private: // For testing purposes

	UNIT_TEST(CSIPRegistrationMgrTest)
	UNIT_TEST(CSIPRegistrationBindingStoreTest)
	UNIT_TEST(CSIPRegistrationUtilityTest)
	};

#endif // CSIPREGISTRATIONBINDINGSTORE_H

// End of File
