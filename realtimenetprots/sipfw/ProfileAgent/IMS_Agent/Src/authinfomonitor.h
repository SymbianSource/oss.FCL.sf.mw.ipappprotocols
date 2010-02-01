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
* Name          : authinfomonitor.h
* Part of       : ProfileAgent
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef CAUTHINFOMONITOR_H
#define CAUTHINFOMONITOR_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RMobilePhone;
class MAuthInfoObserver;

// CLASS DEFINITION
/**
 * Monitors for any changes to the authorization information located in ISIM.
 */
class CAuthInfoMonitor : public CActive
	{
	public: // Constructors and destructor

		/**
	    * Creates a new instance.
	    * @pre aPhone has been opened
		* @param aObserver Callback to receive notification when authorization
		*		 data changes.
		* @param aPhone RMobilePhone		
	    * @return Return New object, ownership is transferred.
	    */
		static CAuthInfoMonitor* NewL( MAuthInfoObserver& aObserver,
									   RMobilePhone& aPhone );

		~CAuthInfoMonitor();

  	public: // From CActive

  		void DoCancel();

		void RunL();

		TInt RunError( TInt aError );

	private: // Constructors

		CAuthInfoMonitor( MAuthInfoObserver& aObserver,
						  RMobilePhone& aPhone );		

	private: // New functions

		void MonitorChanges();

	private: // Data	    

		MAuthInfoObserver& iObserver;

		RMobilePhone& iPhone;
	};

#endif // CAUTHINFOMONITOR_H
