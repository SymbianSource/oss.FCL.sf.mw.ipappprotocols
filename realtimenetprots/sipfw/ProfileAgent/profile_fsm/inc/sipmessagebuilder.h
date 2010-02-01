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
* Name        : sipmessagebuilder.h
* Part of     : sip profile fsm
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef SIPMESSAGEBUILDER_H
#define SIPMESSAGEBUILDER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <stringpool.h>

//FORWARD DECLARATIONS
class CSIPHeaderBase;
class CSIPToHeader;
class CSIPContactHeader;
class CSIPRouteHeader;
class CSIPExtensionHeader;
class CUri8;

// CLASS DECLARATION
/**
* Helper class for adding creating SIP headers.
*
*  @lib sipprofilefsm.lib
*/
class SIPMessageBuilder
	{
	public://New functions

		/**
		* Creates contact header
		* @param aUser user-part
		* @param aExpiresValue to be set as the expires parameter
		* @param aParams contains contact header parameters
		* @param aTransport transport-parameter to be added
		*        If aTransport.DesC() == 0, no parameter will be added.
		* @param aSigComp indicates the usage of SigComp
		* @return contact header
		*/
		IMPORT_C static CSIPContactHeader* CreateContactLC(
			const TDesC8& aUser,
			TInt aExpiresValue,
			const MDesC8Array& aParams,
			RStringF aTransport,
			TBool aSigComp=EFalse);

		/**
		* Creates route header
		* @param a route in textual format
		* @param aSigComp indicates the usage of SigComp
		* @return route header
		*/
		IMPORT_C static CSIPRouteHeader* CreateRouteLC(
		    const TDesC8& aRoute,
		    TBool aSigComp=EFalse);

		/**
		* Creates to header
		* @param aUri user's aor
		* @return to header
		*/
		IMPORT_C static CSIPToHeader* CreateToLC(const TDesC8& aUri);

		/**
		* Creates IMC1 specific authorization header
		* @param aUri a uri directive
		* @param aRealm a realm directive
		* @param aPrivateId a private Id or username
		* @return authorization header
		*/
		IMPORT_C static CSIPExtensionHeader* CreateAuthorizationLC(
			const TDesC8& aUri,
			const TDesC8& aRealm, 
			const TDesC8& aPrivateId);
			
		/*
		* Creates a SIP header from its textual presentation and
		* push the created object to CleanupStack.
		* @param aHeaderDes a SIP header as text including both name and value
		* @return a new instance of CSIPHeaderBase.
		*         The ownership is transferred.
		*/
		IMPORT_C static CSIPHeaderBase* CreateHeaderLC(
		    const TDesC8& aHeaderDes);
		
		/*
		* Parses a SIP header parameter from its textual presentation.
		*
		* @param aParam the parameter including name and optionally 
		*        the value of the parameter. For example "name=value" or "name"
		* @param aName on return contains the name-part of the parameter
		* @param aValue on return contains the value-part of the parameter	
		* @return ETrue if the parameter has a value (aValue parameter was set).
		*         EFalse if parameter does not have a value-part.
		*/		
		IMPORT_C static TBool ParseParamL(
		    const TDesC8& aParam,
		    TPtrC8& aName,
		    TPtrC8& aValue);			

	private:
		
	    /**
		* C++ default constructor.
		*/
		SIPMessageBuilder();
		
		static void AddUriParamL(
		    CUri8& aUri, 
		    const TDesC8& aName,
		    const TDesC8& aValue);
	};

#endif // SIPMESSAGEBUILDER_H
