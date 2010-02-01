/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofileitchelper.h
* Part of     : SIP Profile Client
* Interface   : private
* The wrapper class to hide SOS ITC access
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILEITCHELPER_H
#define CSIPPROFILEITCHELPER_H

//  INCLUDES

#include <e32base.h>
#include <s32mem.h>
#include <bamdesca.h>
#include "sipprofilecs.h"

// FORWARD DECLARATIONS

class RSIPProfile;
class CSIPConcreteProfile;
class CSIPConcreteProfileHolder;
class TSIPProfileSlots;
class CSIPProfilePlugins;

// CLASS DECLARATION

/**
*  The class hides SOS ITC access. Separation done mainly for simplifying testing.
*
*  @lib sipprofilecli.lib
*/
class CSIPProfileITCHelper: public CBase
	{
	public:

	    /**
        * Constructor
		* @param aSip class providing ITC connection
        */
		CSIPProfileITCHelper(RSIPProfile& aSip);

		/**
        * Genegal send of any ITC function
		* @param aArgs data to be passed
		* @param aFunction function to be called
        */
		void SendL(TIpcArgs& aArgs, TSipProfileItcFunctions aFunction);

		/**
        * Send for TSIPProfileSlots class with any ITC function
		* @param aIds data to be passed
		* @param aITCFunction function to be called
        */
		void SendL(TSIPProfileSlots& aIds, 
					TSipProfileItcFunctions aITCFunction);

		/**
        * Send for TSIPProfileSlots class and narrator with any ITC function
		* @param aIds data to be passed
		* @param aITCFunction function to be called
		* @param aNarrator data that extra parameter for function
        */
		void SendL(TSIPProfileSlots& aIds, 
					TSipProfileItcFunctions aITCFunction, 
					const TDesC8& aNarrator);

		/**
        * Send for CSIPConcreteProfile class with any ITC function
		* @param aITCFunction function to be called
		* @param aProfile profile to be passed
        */
		void SendL(TSipProfileItcFunctions aITCFunction,
					const CSIPConcreteProfile& aProfile);

		/**
        * Send for TSIPProfileSlots and CSIPConcreteProfile class 
		* with any ITC function
		* @param aIds data to be passed
		* @param aITCFunction function to be called
		* @param aProfile profile to be passed
        */
		void SendL(TSIPProfileSlots& aIds,
					TSipProfileItcFunctions aITCFunction,
					const CSIPConcreteProfile& aProfile);

		/**
        * Send for TSIPProfileSlots class with any ITC function, does not leave
		* @param aIds data to be passed
		* @param aITCFunction function to be called
		* @return KErrNone if OK, otherwise error code.
        */
		TInt Send(TSIPProfileSlots& aIds, TSipProfileItcFunctions aITCFunction);

	private:
	
		/**
        * Templete method for serializing passed data
		* @param aElements data to be serialized
		* @return serialized data.
        */
		template<class T> CBufFlat* ExternalizeLC(const T& aElements);

	private:

		RSIPProfile& iSip;
		TIpcArgs iITCMsgArgs;
	};

#endif
