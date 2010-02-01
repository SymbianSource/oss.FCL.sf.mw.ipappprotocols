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
* Name          : CSIPRegistrarStore.h
* Part of       : Registration
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPREGISTRARSTORE_H
#define CSIPREGISTRARSTORE_H

#include <e32base.h>
#include "SipStackServerDefs.h"
#include "_sipcodecdefs.h"

class CURIContainer;
class CSIPRegistrar;
class CDeleteMgr; 

/**
 * Class stores array of CSIPRegistrar objects. Register request 
 * sent to same registrar should use the same callId and the CSeq 
 * should be increased. Before any register request is sent, 
 * registrar store will be checked. If the registrar exists 
 * in CSIPRegistarStore, it increace the CSeq and copy 
 * the CallId and CSeq to REGISTER request. 
 */

class CSIPRegistrarStore : public CBase
	{
public:
       
   /** 
	* Create a instance of CSIPRegistrarStore.
	*
    * @ return a CSIPRegistrarStore object.
	*/
	static CSIPRegistrarStore* NewL();

   /** 
	* Create a instance of CSIPRegistrarStore nad push it to cleanup stack
	*
    * @ return a CSIPRegistrarStore object.
	*/
	static CSIPRegistrarStore* NewLC();

   /**
    * Destructor.
	*/
	~CSIPRegistrarStore();

   /**
    * Creates CSIPRegistrar object, and add it to the array of 
	* CSIPRegRegistrar objects in CSIPRegRegistrarStore.
	*
	* @ param aRegistrar; a registrar
	* @ param aCallId; CallId
	* @ param aCSeq; a CSeq Number
	* @ param aRegistrationId; a TRegistrationId
	*
	* @ return CSIPRegistrar object, the ownership is transferred.
	*/
	CSIPRegistrar* CreateRegistrarL(const CURIContainer&  aRegistrar, 
		                            const TDesC8&         aCallId,
						            TUint                 aCSeq,
						            const TRegistrationId aRegistrationId);

   /**
    * Find the registrar object, in which registrar name is same
	* as the parameter.
	* 
	* @ param aRegistrar; a registrar name.
	*
	* @ return a CSIPRegistrar object. 0 if the registrar can not be found.
	*          the ownership of the CSIPRegistrar object is not transferred.
	*/
	CSIPRegistrar* FindRegistrar(const CURIContainer& aRegistrar);

   /**
    * remove the registrar object passed as parameter from 
	* registrars array
	* 
	* @ param aRegistrar; a CSIPRegistrar object, 
	*   the ownership is not transferred.
	*
	* @ return KErrNone if the registrar object can be found from
	*          registrars array. KErrNotFound otherwise.
	*/
	TInt RemoveRegistrar (CSIPRegistrar* aRegistrar);


private:
      
	CSIPRegistrarStore();
	void ConstructL();	

private: // Data

	RPointerArray<CSIPRegistrar> iRegistrars;
	CDeleteMgr*                  iDeleteMgr;

private: // For testing purposes

	UNIT_TEST(CSIPRegistrarStoreTest)
	};

#endif // CSIPREGISTRARSTORE_H

// End of File
