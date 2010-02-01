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
* Name          : CConnectionContainer.h
* Part of       : ConnectionMgr
* See class definition below.
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __CCONNECTIONCONTAINER_H__
#define __CCONNECTIONCONTAINER_H__

// INCLUDES
#include <in_sock.h>
#include <commdbconnpref.h>
#include <extendedconnpref.h>

// FORWARD DECLARATIONS
class MSIPNetworkObserver;
class CConnectionMonitor;

// CLASS DEFINITION
/**
 *
 */
class CConnectionContainer
	: public CActive
	{
	public: // Constructors and destructor


		/**
		 * Static constructor.
		 *
		 * @param aObserver Reference to observer
		 * @return An initialized instance of this class.
		 */
		static CConnectionContainer* NewL( MSIPNetworkObserver& aObserver );

		/// Destructor
		~CConnectionContainer();

	public: // New methods

		RConnection& Connection();
		TInt OpenConnection( TUint32 aIapId, RSocketServ& aServer );
		void CloseConnection();
		TInt MonitorConnectionClosure( RSocketServ& aServer );

	protected: // From CActive

		void DoCancel();
		void RunL();

	private: // Constructors

		/// Default constructor. Not implemented.
		CConnectionContainer();
		CConnectionContainer( MSIPNetworkObserver& aObserver );
		void ConstructL();
		
    private:
    
        TInt OpenConnection( RSocketServ& aServer );

	private: // Data

		MSIPNetworkObserver& iObserver;
		RConnection	iConnection;
		TConnPrefList  iPrefList;
		TExtendedConnPref iExtPrefs;
		CConnectionMonitor* iMonitor;
	};

#endif	// __CCONNECTIONCONTAINER_H__
