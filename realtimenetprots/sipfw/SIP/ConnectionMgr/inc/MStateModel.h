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
* Name        : MStateModel.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __MSTATEMODEL_H__
#define __MSTATEMODEL_H__

// INCLUDES
#include <in_sock.h>	// TInetAddr
#include "SipStackServerDefs.h"	// TTransactionId
#include "sipnetworkobserver.h"

// FORWARD DECLARATIONS
class CSIPRequest;
class CSIPResponse;
class TSIPTransportParams;
class CUri8;

// CLASS DEFINITION
/**
 * MStateModel defines an interface for the CSIPConnection state model
 */
class MStateModel
	: public MSIPNetworkObserver
	{
	public: // Enumerations

		/// Network states
		enum TState
			{
			EInit,
			EInactiveMonitoring,
			EInactiveConnecting,
			EActiveResolving,
			EActiveResolved,
			ESuspended,
			EUnavailable,
			EMaxStates
			};

	public:	// Constructors and destructor

		/// Virtual destructor
		virtual ~MStateModel() {};

	public: // Abstract methods

		virtual void SendL( const TSIPTransportParams& aParams,
		                    CSIPRequest& aRequest,
		                    TBool aForceUDP,
		                    const CUri8* aOutboundProxy,
							const TInetAddr& aRemoteAddr,
							TRequestStatus& aStatus ) = 0;

 		virtual void SendL( const TSIPTransportParams& aParams,
 		                    CSIPResponse& aResponse,
                  			TTransactionId aId,
							TRequestStatus& aStatus ) = 0;

		virtual MStateModel::TState StateValue() const = 0;

		virtual void EnterState( MStateModel::TState aState,
								 TInt aError ) = 0;
		
		virtual TInt Error() const = 0;

	};

#endif // __MSTATEMODEL_H__
