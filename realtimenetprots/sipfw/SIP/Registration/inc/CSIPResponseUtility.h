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
* Name          : CSIPResponseUtility.h
* Part of       : SIP Registrations
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPRESPONSEUTILITY_H
#define CSIPRESPONSEUTILITY_H

// INCLUDES
#include <e32base.h>

// FORWARD DELARATION
class CSIPResponse;
class CSIPContactHeader;
class CSIPURI;
class MRegistrationContext;
class CSIPViaHeader;


class CSIPResponseUtility : public CBase
	{
public:

   /** 
    * @ param aSIPResponse; a CSIPResponse.
	* @ return ETrue if the response code is 3xx-6xx, EFalse otherwise.
	*/ 
	static TBool IsError(CSIPResponse& aSIPResponse);

   /** 
	* Get the contact header by comparing the uri of contact in response
	* and the uri in binding. there maybe more than one contact 
	* headers in response. only the one which uri equals the uri 
	* of Contact in binding is returned.
	*
	* @param aContactInBinding; contact header in binding.
	* @param aSIPResponse; a CSIPResponse.
	* @return CSIPContactHeader object. it can be 0 if the contact
	*          can not be found from the corresponding binding or
	*          there is no contact header in response.
	*/ 
	static CSIPContactHeader* ContactHeader(
		CSIPContactHeader& aContactInBinding,
		CSIPResponse&      aSIPResponse);

   /** 
	* Get the callId from response. leave if there is no callId header
	* in response.
	*
	* @ param aSIPResponse; a CSIPResponse.
	*/ 
	static HBufC8* CallIdL(CSIPResponse& aSIPResponse);

   /** 
	* Get the CSeqNumber from response. 
	* leave if there is no CSIPCSeqHeader in response.
	*
	* @param aSIPResponse; a CSIPResponse.
	*/
	static TUint CSeqValueL(CSIPResponse& aSIPResponse);

   /** 
	* Get Expires value. function first checks the expires parameter
	* from contact header of response, if there is no expires parameter,
	* function checks the expires header from response, if there is expires
	* header, it copies the expires header to expires parameter and remove 
	* expires header. if there is no expires header neither, function copies
	* the expires parameter from binding.
	*
	* @param aBindingContact.
	* @param aSIPResponse.
	* @return KErrNone, if the response is ok, 
	*         otherwise corresponding error code.
	*/
	static TUint ExpireValueL(CSIPContactHeader& aBindingContact,
		                      CSIPResponse&      aSIPResponse);

	static TInt Check2XXResponse(MRegistrationContext& aRegistration,
		                         CSIPResponse& aResponse);
	};

#endif // CSIPRESPONSEUTILITY_H

// End of File
