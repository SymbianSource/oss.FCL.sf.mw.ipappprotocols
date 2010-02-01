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
* Name        : SipProfileCSServer.h
* Part of     : SIP Profile Server
* Interface   : private
* The class for ITC server
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPPROFILECSSERVER_H
#define SIPPROFILECSSERVER_H

//  INCLUDES

#include <e32base.h>
#include "SipProfileCSServer.pan"
#include "SipProfileServerCore.h"


// FORWARD DECLARATIONS
class CSipProfileCSServerCloseTimer;
class CSIPProfileServerCore;

// CLASS DECLARATION
/**
 * The class providing ITC server functionality.
 */
class CSIPProfileCSServer: public CPolicyServer
	{
	public : 
    
	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSIPProfileCSServer* NewL(CSIPProfileServerCore& aServerCore);

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSIPProfileCSServer* NewLC(CSIPProfileServerCore& aServerCore);

	    /**
        * Destructor
        */
		~CSIPProfileCSServer();

	    /**
        * Initialize server
		* @return KErrNone if successfull
        */
		static TInt ThreadFunction(TAny* aNone);

	    /**
        * Increment session counter
        */
		void IncrementSessions();

	    /**
        * Decrement session counter
        */
		void DecrementSessions();

	    /**
        * Stops server if execution no longer needed
        */
		void ServerCanStop();

	    /**
        * Stops server abnormally
        */
		void ShutdownL();

	    /**
        * Stops server abnormally, deletes profiles.dat
        */
		void CleanShutdownL();

	protected: // From CActive
		
	    /**
        * Called if server fails
		* @param aError error code
		* @return KErrNone, errors are ignored
        */
		TInt RunError(TInt aError);

	private:

	    /**
        * Constructor
		* @param aPriority priority of server
        */
		CSIPProfileCSServer(CSIPProfileServerCore& aServerCore,
		                    TInt aPriority);

	    /**
        * Symbian 2nd phase constructor
        */
		void ConstructL();

	    /**
        * Send panic to client
		* @param aMessage identifies client
		* @param aReason panic reason code
        */
		static void PanicClient(const RMessage2& aMessage, 
                             TSIPProfileCSPanic aReason);

	    /**
        * Panic server
		* @param aReason panic reason code
        */
		static void PanicServer(TSIPProfileCSPanic aReason);

	    /**
        * Initialize server
        */
		static void ThreadFunctionL(RMutex& aMutex);
		
		/**
		* Signals a client's semaphore,
		* that is waiting for the server to be started.
		* @return KErrNone if semaphore was found and signalled
		*/ 
		static TInt SignalClientSemaphore();

	private: // From CServer
		
	    /**
        * Create new ITC session
		* @param aVersion version of client
		* @param aMessage identifies client
		* @return new session instance
        */
		CSession2* NewSessionL (const TVersion &aVersion, 
                                const RMessage2& aMessage) const;

	private:

		TUint                          iSessionCount;
		CSIPProfileServerCore&         iServerCore;
		CSipProfileCSServerCloseTimer* iCloseTimer;


	private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPProfileCSServerTest;
		friend class CSIPProfileAlrTest;
		friend class CSIPProfileStateTest;
		friend class CSIPProfileServerCoreTest;
		friend class CSIPProfileCSSessionTest;
#endif
	};

#endif // SIPPROFILECSSERVER_H
