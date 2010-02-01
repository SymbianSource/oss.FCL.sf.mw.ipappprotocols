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
* Name        : CPacketContextWatcher.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __CPACKETCONTEXTWATCHER_H__
#define __CPACKETCONTEXTWATCHER_H__

// INCLUDES
#include <etelpckt.h>
#include <es_enum.h>

// FORWARD DECLARATIONS
class CPacketContextMonitor;

// CLASS DEFINITION
/**
 *
 */
class CPacketContextWatcher
	: public CActive
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor..
		 *
		 * @return An initialized instance of this class.
		 */
		static CPacketContextWatcher* NewL(
		                CPacketContextMonitor& aParent,
		                RSocketServ& aSocketSrv,
		                TConnectionInfo aConnectionInfo );
		
		/**
		 * Static constructor..
		 *
		 * @return An initialized instance of this class.
		 */
		static CPacketContextWatcher* NewLC(
		                CPacketContextMonitor& aParent,
		                RSocketServ& aSocketSrv,
		                TConnectionInfo aConnectionInfo );

		/// Destructor
		~CPacketContextWatcher();
		
	public:
	
	    void WatchStatusChange();
	    
	    TBool IsContextAvailable();

	protected: // From CActive

		void DoCancel();
		void RunL();

	private: // Constructors

		/// Constructor
		CPacketContextWatcher( CPacketContextMonitor& aParent,
		                       RSocketServ& aSocketSrv,
		                       TConnectionInfo aConnectionInfo );

		/// Default ccnstructor, not implemented.
		CPacketContextWatcher();

		/// 2nd phase constructor
		void ConstructL();

	private: // Data

		CPacketContextMonitor& iParent;

		RSocketServ& iSocketSrv;
		
		TPckgBuf<TConnectionInfo> iConnectionInfo;
		
		RConnection iConnection;
        
        TNifProgressBuf iProgress;
        
        TBool iAvailable;
	
	};

#endif // __CPACKETCONTEXTWATCHER_H__

