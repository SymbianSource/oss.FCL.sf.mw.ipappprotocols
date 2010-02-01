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
* Name          : MIpSecMechanismParams.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef MIPSECMECHANISMPARAMS_H
#define MIPSECMECHANISMPARAMS_H

class RSADB;
class RIpsecPolicyServ;
class MTimerManager;
class MSIPTransportMgr;
class CSIPSecurityHeaderBase;
class CSipSecAgreeContext;
class MSIPTransportRemovalObserver;
class TSIPTransportParams;
class CSIPRequest;

// IpSec Mechanism parameters
class MIpSecMechanismParams
	{
public:

	virtual TUint T1() = 0;
	virtual RSADB& Sadb() = 0;
	virtual RIpsecPolicyServ& PolicyServer() = 0;
	virtual TUint SeqNumber() = 0;
	virtual MTimerManager& TimerMan() = 0;
	virtual MSIPTransportMgr& TransportMan() = 0;
	virtual void UpdateSecCliL(RPointerArray<CSIPSecurityHeaderBase>& aSecCliHeaders) = 0;
	virtual void ContextCleared(CSipSecAgreeContext* aContext) = 0;
    virtual MSIPTransportRemovalObserver* TransportRemovalObserver() = 0;
    virtual void ProcessInitialReqisterL( TSIPTransportParams& aTransportParams,
                                          CSIPRequest& aRequest ) = 0;
	};

#endif // MIPSECMECHANISMPARAMS_H


// End of File
