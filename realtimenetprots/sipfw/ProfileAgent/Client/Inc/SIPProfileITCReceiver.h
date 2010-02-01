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
* Name        : sipprofileitcreceiver.h
* Part of     : SIP Profile Client
* Interface   : private
* The class to receive asyncronous ITC events
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILEITCRECEIVER_H
#define CSIPPROFILEITCRECEIVER_H

//  INCLUDES

#include <e32base.h>
#include <s32mem.h>
#include "sipprofilecs.h"
#include "sipprofileslots.h"

// FORWARD DECLARATIONS

class RSIPProfile;
class MSIPConcreteProfileObserver;
class MSipProfileAlrObserver;

// CLASS DECLARATION

/**
*  The class is active object that receives asyncronous events from server
*
*  @lib sipprofilecli.lib
*/
class CSIPProfileITCReceiver: public CActive
	{
	public:

	    /**
        * Two-phased constructor.
		* @param aSip a client providing connection to SIP profile server.
		* @param aObserver observer that events are transferred to 
		* @return new instance
        */
		static CSIPProfileITCReceiver* NewL(
			RSIPProfile& aSipProfile, MSIPConcreteProfileObserver& aObserver);

	    /**
        * Two-phased constructor.
		* @param aSip a client providing connection to SIP profile server.
		* @param aObserver observer that events are transferred to 
		* @return new instance
        */
		static CSIPProfileITCReceiver* NewLC(
			RSIPProfile& aSipProfile, MSIPConcreteProfileObserver& aObserver);

	    /**
        * Destructor.
        */
		~CSIPProfileITCReceiver();
		
	public: // New functions
		
		/**
        * Sets the observer for ALR events. 
		* @param aAlrObserver observer that ALR events 
		* are transferred to.
		* @return KErrAlreadyExists if iAlrObserver already 
		* exists, else KErrNone.
        */
		TInt SetALRObserver(MSipProfileAlrObserver* aALRObserver);

	private:
	
	    /**
        * Constructor.
		* @param aSip a client providing connection to SIP profile server.
		* @param aObserver observer that events are transferred to 
        */
		CSIPProfileITCReceiver(
			RSIPProfile& aSipProfile, MSIPConcreteProfileObserver& aObserver);

	    /**
        * Symbian 2nd phase constructor.
        */
		void ConstructL();

	    /**
        * Method that is being called when event occurs
        */
		void Receive();

	    /**
        * Method that is being called when event occurs
		* @param aIds class holding event data
        */
		void IncomingEventL(TSIPProfileSlots& aIds);

	    /**
        * Method that is being called when failure event occurs
		* @param aError error id
		* @param aIds class holding event data
		*/
		void ErrorOccuredL(TInt aError, TSIPProfileSlots& aIds);

	private:

		// From CActive:
		void RunL();

		// From CActive:
		TInt RunError(TInt aError);

		// From CActive:
		void DoCancel();

	private:

		RSIPProfile& iSip;
		MSIPConcreteProfileObserver& iObserver; 
		TIpcArgs                      iITCMsgArgs;
		TPckgBuf<TSIPProfileSlots>    iIdsPckg;
		MSipProfileAlrObserver* 	  iALRObserver;
		
	private: // The unit test class declared as a friend
#ifdef CPPUNIT_TEST
	friend class CSIPProfileITCReceiverTest;	
#endif
	
	};

#endif
