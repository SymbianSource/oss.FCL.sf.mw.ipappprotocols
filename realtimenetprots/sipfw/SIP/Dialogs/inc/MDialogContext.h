/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MDialogContext.h
* Part of       : SIPDialogs
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef MDIALOGCONTEXT_H
#define MDIALOGCONTEXT_H

#include <e32base.h>
#include <stringpool.h>
#include "SipStackServerDefs.h"
#include "TSIPTransportParams.h"

class MDialogOwner;
class TDialogStateBase;
class CSIPMessage;
class CSIPRequest;
class CSIPCallIDHeader;
class CSIPFromToHeaderBase;


class MDialogContext
	{
public:
	
    virtual MDialogOwner* DialogOwner() = 0;

    virtual TDialogId DialogId() const = 0;
	
	virtual TRegistrationId RegistrationId() const = 0;
	
	virtual TSIPTransportParams TransportParams(
	    const TRegistrationId& aRegistrationId) = 0;
	
    virtual void SelfDestruct() = 0;
	
    virtual TTransactionId FirstTransactionId() = 0;

    virtual void ChangeState(TDialogStateBase& aState) = 0;
	
    virtual void TerminateForkedDialogs(TTransactionId aTransactionId) = 0;

    virtual void SetCallId(CSIPCallIDHeader* aCallId) = 0;

    virtual CSIPCallIDHeader* CallId() = 0;
	
    virtual void SetLocalTag(RStringF aTag) = 0;
    
    virtual RStringF LocalTag() = 0;

    virtual void SetRemoteTagL(CSIPFromToHeaderBase* aFromTo) = 0;

    virtual RStringF RemoteTag() = 0;

	virtual TBool RemoteTagSet() = 0;
	
    virtual void SetLocalSeqNum(TUint aNum) = 0;
	
    virtual void SetRemoteTargetWithMsgL(CSIPMessage& aMsg) = 0;

    virtual void SetRemoteSeqNum(TUint aNum) = 0;
	
    virtual void SetRouteSetUsingRecordRouteL(CSIPMessage& aMessage,
                                              TBool aIsUAC,
                                              TInt aLeaveCode) = 0;

    virtual TInt RemoveTransactionItem(TTransactionId& aTransactionId) = 0;
    
    virtual TBool HasRouteSet() const = 0;
    
    virtual void AddCallIdHeaderInDialogL(CSIPMessage& aMessage) = 0;
    
    virtual void AddCSeqHeaderInDialogL(CSIPRequest& aRequest) = 0;    
 	};

#endif // MDIALOGCONTEXT_H

// End of File
