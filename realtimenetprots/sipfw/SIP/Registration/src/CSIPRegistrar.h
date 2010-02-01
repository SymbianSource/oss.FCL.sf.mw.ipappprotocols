/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPRegistrar.h
* Part of       : Registration
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPREGISTRAR_H
#define CSIPREGISTRAR_H

#include <e32base.h>
#include "SipStackServerDefs.h"
#include "_sipcodecdefs.h"

class CSIPRequest;
class CURIContainer;
class CSIPRegistrarStore;

/**
 * Class stores registrar information (the host part of To header from 
 * REGISTER request), related callId and CSeq.
 */

class CSIPRegistrar : public CBase
	{
public:
         
   /** 
	* Create a CSIPRegistrar object and adds it to the clean up stack.
	* 
	* @ param  aRegistrar; a registrar name.
	* @ param  aCallId; a CallId.
	* @ param  aCSeqNumber; aCSeq number.
	* @ param  aRegistrationnId; Registration Id.
	* @ param  aRegistrarStore; store of registrar.
	*
	* @ return a CSipRegistrar instance. 
	*/
	static CSIPRegistrar* NewLC(const CURIContainer&  aRegistrar,
		                        const TDesC8&         aCallId,
								TUint                 aCSeqNumber,
								const TRegistrationId aRegistrationId,
								CSIPRegistrarStore&   aRegistrarStore);
 
   /**
    * Destructor.
	*/
	~CSIPRegistrar();

   /** 
	* Get registrar
	*
	* @ return registrar infor.
	*/
	const CURIContainer& Registrar() const;

   /** 
	* increase the CSeq number
	*/
	void IncreaseCSeqNumber();

   /** 
	* get CSeq number
	*
	* @ return CSeq number
	*/
	TUint CSeqNumber() const;

   /**
    * fill in the callId and CSeq to the request passed as parameter.
	*
	* @ param aSIPRequest; a sip request.
	*/
	void FillCallIdAndCSeqL(CSIPRequest& aSIPRequest);

   /**
    * add TRegistrationId of created binding to the registrationIds array. 
	*
	* @ param aRegistrationId; a TRegistrationId.
	*/
	void AddRegistrationIdL(const TRegistrationId aRegistrationId);

   /**
    * remove RegistrationId of related binding from registrationIds array 
	*
	* @ param aRegistrationId; a TRegistrationId.
	* @ return ETrue if the registrar is added to delete manager,
	*          EFalse otherwise.
	*/
	TBool BindingRemoved(const TRegistrationId aRegistrationId);

   /**
    * check if registrationId exists. if it exists, return the index
	* otherwise return KErrNotFound.
    */
	TInt FindRegistrationId(const TRegistrationId aRegistrationId);

private:
       
	CSIPRegistrar(TUint aCSeqNumber, CSIPRegistrarStore& aRegistrarStore);

	void ConstructL(const CURIContainer&  aRegistrar,
		            const TDesC8&         aCallId,
					const TRegistrationId aRegistrationId);	

private: // Data

	// the Call-ID field identifies registration requests 
	// to the same registar.
	HBufC8*  iCallId;

	// the host in To header of sip request
	CURIContainer* iRegistrar;

	// the sequence number in CSeq header, 
	// it guarantees proper ordering of REGISTER request.
	TUint    iCSeqNumber;

	// a referenct to the store of registrars array
	CSIPRegistrarStore&     iRegistrarStore;

	// registration id array
	RArray<TRegistrationId> iRegistrationIds;

private: // For testing purposes

	UNIT_TEST(CSIPRegistrarTest)
	};

#endif // CSIPREGISTRAR_H

// End of File
