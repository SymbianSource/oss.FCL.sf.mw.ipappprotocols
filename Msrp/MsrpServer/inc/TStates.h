/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef TSTATES_H_
#define TSTATES_H_

#include "MSRPServerCommon.h"

class TStateBase;
class CMSRPServerSubSession;
class CMSRPMessageHandler;

enum TStates
{
    EIdle = 0,
    EConnecting,    
    EWaitForClient,    
    EActive,
    EActiveSend,
    EFileShare,
	EError,
    EEndLine
};


class TStateBase
{ 
public:
    virtual TStateBase* EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext)=0;
    virtual TStateBase* HandleStateErrorL(CMSRPServerSubSession *aContext);
    virtual TStateBase * processIncommingMessageL(CMSRPServerSubSession *aContext, CMSRPMessageHandler* incommingMsg = NULL);
    TStateBase * processPendingMessageQL( CMSRPServerSubSession *aContext );
    virtual TStateBase * handlesResponseL(CMSRPServerSubSession *aContext,
                    CMSRPMessageHandler *incommingMsgHandler);

    virtual TStateBase* handleRequestsL(CMSRPServerSubSession *aContext,
                    CMSRPMessageHandler *incommingMsgHandler);
    
    TStateBase* handleClientListnerCancelL(CMSRPServerSubSession * aContext, 
                                        TMSRPFSMEvent aEvent);
    TStateBase* handleConnectionStateChangedL(CMSRPServerSubSession *aContext);
    TStateBase* handleQueuesL(CMSRPServerSubSession *aContext);
	TStateBase* handleConnectionErrorsL(CMSRPServerSubSession *aContext);
    virtual TStates identity()=0;
    TStateBase* handleInCommingMessagesL(CMSRPServerSubSession *aContext);
    TStateBase* handleSendFileL(CMSRPServerSubSession *aContext);
    TStateBase* handleReceiveFileL(CMSRPServerSubSession *aContext);
};


class TStateIdle : public TStateBase
{
public:
    TStateBase* EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext);
    TStates identity();
    
private:    
    TStateBase* handleLocalMSRPPathRequestL( CMSRPServerSubSession *aContext);
    TStateBase* handleConnectRequestL(CMSRPServerSubSession *aContext);
    TStateBase* handleListenRequestL(CMSRPServerSubSession *aContext);

    TBool initializeConnectionL(CMSRPServerSubSession *aContext);    
    
};


class TStateConnecting : public TStateBase
{
public:
    TStateBase* EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext);
    TStates identity();    

};


class TStateWaitForClient : public TStateBase
{
public:
    TStateBase* EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext);
    TStates identity();
    
private:
    TStateBase* fileSendCompleteL(CMSRPServerSubSession *aContext);
    TStateBase * handleResponseSentL( CMSRPServerSubSession *aContext);
    
};


class TStateActive : public TStateBase
{
public:
    TStateBase* EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext);
    TStates identity();

private:
    TStateBase* handleSendDataL(CMSRPServerSubSession *aContext);
    
};


class TStateFileShare : public TStateBase
{
public:
    TStateBase* EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext);
    TStates identity();

private:
    TStateBase * processIncommingMessageL(CMSRPServerSubSession *aContext, CMSRPMessageHandler* incommingMsg = NULL);
    
    TStateBase* fileSendCompleteL(CMSRPServerSubSession *aContext);
    TStateBase * handlesResponseL(CMSRPServerSubSession *aContext,
                          CMSRPMessageHandler *incommingMsgHandler);
    
    TStateBase* handleRequestsL(CMSRPServerSubSession *aContext,
                    CMSRPMessageHandler *incommingMsgHandler);
    TStateBase * handleResponseSentL( CMSRPServerSubSession *aContext);
    
    TStateBase * handleSendProgressL( CMSRPServerSubSession *aContext);
    
    TStateBase * handleReceiveProgressL( CMSRPServerSubSession *aContext);
    
};


class TStateActiveSend : public TStateBase
{
public:
    TStateBase* EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext);
    TStates identity();
    
private:    
    TStateBase * MessageSendCompleteL(CMSRPServerSubSession *aContext); 
    
};


class TStateError : public TStateBase
{
public:
    TStateBase* EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext);
    TStates identity();    

};


#endif /* TSTATES_HPP_ */
