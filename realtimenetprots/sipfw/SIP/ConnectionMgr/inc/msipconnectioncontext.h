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
* Name          : MSipConnectionContext.h
* Part of       : ConnectionMgr
* See class definition below
* Version       : SIP/4.0
*
*/



#ifndef __MSIPCONNECTIONCONTEXT_H__

/**
* @internalComponent
*/
#define __MSIPCONNECTIONCONTEXT_H__

// INCLUDES
#include <in_sock.h>
#include "MStateModel.h"

// FORWARD DECLARATIONS
class CSIPRequest;
class CSIPResponse;
class TSIPTransportParams;

// CLASS DEFINITION
/**
 *
 */
class MSIPConnectionContext
	{
	public: // Abstract methods
		enum TBearerType
  		{
  		EModemBearer = 0,
  		ELanBearer
		};

		virtual ~MSIPConnectionContext() {}

		virtual void SendToTransportL( const TSIPTransportParams& aParams,
		                               CSIPRequest& aRequest,
		                               TBool aForceUDP,
		                               const CUri8* aOutboundProxy,
									   const TInetAddr& aRemoteAddr,
									   TRequestStatus& aStatus ) = 0;

	 	virtual void SendToTransportL( const TSIPTransportParams& aParams,
	 	                               CSIPResponse& aResponse,
	                      			   TTransactionId aId,
									   TRequestStatus& aStatus ) = 0;

		virtual TInt OpenConnection() = 0;

		virtual void CloseConnection() = 0;

		virtual void CreateTransportResourcesL() = 0;
        virtual void CreateDefaultTransportsL() = 0;

		virtual void ReleaseTransportResources( TBool aReleaseAllResources ) = 0;
		
		virtual TBool DoTransportResourcesExist() const = 0;

		virtual void ResolveLocalAddress( TInetAddr& aAddress ) = 0;

		virtual void StartMonitoringL( TInt aError ) = 0;

		virtual void StopMonitoring() = 0;
		
		virtual TBool ContinueMonitoring( TInt aError ) = 0;
		
		virtual TInt MonitorConnectionClosure() = 0;

		virtual void StateModelChanged( MStateModel::TState aState ) = 0;
		
		virtual void Destroy() = 0;
		
		virtual TInt ConnectionType() = 0;

	};
#endif // __MSIPCONNECTIONCONTEXT_H__



