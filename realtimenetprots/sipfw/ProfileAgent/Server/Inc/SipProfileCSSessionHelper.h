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
* Name        : sipprofilecssessionhelper.h
* Part of     : SIP Profile Server
* Interface   : private
* The class wrapping IPC read and write operations 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILECSSESSIONHELPER_H
#define CSIPPROFILECSSESSIONHELPER_H

//  INCLUDES

#include "sipprofilecs.h"
#include "sipconcreteprofileobserver.h"

// FORWARD DECLARATIONS

class CSIPProfileCSServer;
class CSIPProfileServerCore;
class CSIPConcreteProfile;
class CSIPConcreteProfileHolder;
class TSIPProfileSlots;
class TSIPProfileTypeInfo;

// CLASS DECLARATION

/**
 * The class used for caching and reference counting of profiles.
 */
class CSIPProfileCSSessionHelper : public CBase
	{
	public:

	    /**
        * Gets IPC service function
		* @param aMessage handle for IPC communication
		* @return service request function
        */
		TInt ServiceFunction(const RMessage2& aMessage) const;

	    /**
        * Completes IPC function
		* @param aMessage handle for IPC communication
		* @param aCode return code
        */
		void CompleteService(
		    const RMessage2& aMessage,
		    TInt aCode) const;

	    /**
        * Gets IPC argument as integer
		* @param aItcArgIndex index of argument
		* @param aMessage handle for IPC communication
        */
		TUint IPCArgTUintL(
		    TSipProfileItcArguments aItcArgIndex,
            const RMessage2&  aMessage) const;

	    /**
        * Gets serialized object from IPC
		* @param aIpcArgIndex index of argument
		* @param aMessage handle for IPC communication
        */
		HBufC8* ReadLC(
		    TSipProfileItcArguments aIpcArgIndex,
		    const RMessage2&  aMessage) const;
		    
	    /**
        * Gets id slots to write return values of request
		* @param aMessage handle for IPC communication
        */
		TSIPProfileSlots ReadSipIdsL(const RMessage2& aMessage) const;

	    /**
        * Writes return values in id slots 
		* @param aMessage handle for IPC communication
		* @param aIds structure holding return values
        */
		void WriteL(
		    const RMessage2& aMessage,
		    const TSIPProfileSlots& aIds);

	    /**
        * Writes serialized object as return value 
		* @param aItcArgIndex index of argument
		* @param aMessage handle for IPC communication
		* @param aDes serialized object
        */
		void WriteL(
		    TSipProfileItcArguments aArg,
		    const RMessage2& aMessage, 
            const TDesC8& aDes);
            
         void ReadSIPProfileTypeInfoL(
            const RMessage2&  aMessage,
            TSIPProfileTypeInfo& aTypeInfo) const;
	};

#endif // CSIPPROFILECSSESSIONHELPER_H
