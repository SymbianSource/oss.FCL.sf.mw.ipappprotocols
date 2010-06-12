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

#ifndef CMSRPSERVERSUBSESSION_H_
#define CMSRPSERVERSUBSESSION_H_

#include <e32std.h>
#include <e32base.h>
#include <e32math.h>
#include <in_sock.h>


#include "MMSRPConnectionObserver.h"
#include "MMSRPMessageObserver.h"
#include "MSRPServerCommon.h"
#include "CMSRPMessageHandler.h"

const TInt KMSRPSessIdLength=12;

// Forward Declarations
class CMSRPServerSubSession;
class CMSRPServerSession;
class MMSRPConnectionManager;
class CMSRPMessageBase;
class MMSRPConnection;
class MMSRPIncomingMessage;
class CMSRPMessageHandler;
class CMSRPFromToHeaderBase;
class CMSRPToPathHeader;
class CMSRPHeaderBase;

class TStateBase;
class CStateFactory;


class CRMessageContainer: public CBase
{
public:

	CRMessageContainer();
	~CRMessageContainer();
	TBool set(const RMessage2& aMessage);
	
	TInt CRMessageContainer::Write(TInt aParam,const TDesC8& aDes,TInt aOffset=0) const;
	void ReadL(TInt aParam,TDes8& aDes,TInt aOffset=0) const;

	void Complete(TInt aReason);

	TBool Check(); 	

	TBool iStatus; 
	RMessage2 iMsg;
	
};


template <class T>
class CQueue : public CBase
	{
	public:
		inline CQueue();
		virtual ~CQueue(){};
		inline TBool Queue(T& element);
		inline T* DeQueue();
		inline TBool isEmpty();

		inline T* getHead();
		
		virtual inline T* getMatch(T* aMatch);

		TInt inline Length(){
						#ifdef _DEBUG
						if(0 == iQLenght && !iList.IsEmpty())
							{
								MSRPLOG("CQueue::Lenght!! Problem!! in the queue lenght calculations");			
							}
						#endif
						return iQLenght;
	
						};

		inline void CQueue<T>::Destroy();

	protected:
		TInt iQLenght;
	
		TSglQue<T> iList;
		
	};


template <class T>
CQueue<T>::CQueue(): iList(T::LinkOffset()),iQLenght(0){}

template <class T>
TBool CQueue<T>::Queue(T& element)
	{
	iList.AddLast(element);
	iQLenght++;
	return TRUE;
	}

template <class T>
T* CQueue<T>::DeQueue()
	{
	T* element = NULL;
	if(!(iList.IsEmpty()))
		{
		element = iList.First();
		iList.Remove(*element);
		iQLenght--;
		}

	return element;
	}

template <class T>
TBool CQueue<T>::isEmpty()
	{
		return iList.IsEmpty();
	}

template <class T>
T* CQueue<T>::getHead()
	{
		T* element = NULL;
		if(!(iList.IsEmpty()))
		{
		element = iList.First();		
		}
		return element;
	}

template <class T>
T* CQueue<T>::getMatch(T* aMatch)	
	{
		aMatch;
		return NULL;
	}

template<class T>
inline void CQueue<T>::Destroy()
	{
	T* element = NULL;
	TSglQueIter<T> iter(iList);
	element = iter++;
	
	while(element)
		{		
		iList.Remove(*element);
		delete element;
		element = iter++;	
		iQLenght--;
		}

	}

template<class T>
class CQueueMsgHandlers : public CQueue<T>
	{
	public:
		CQueueMsgHandlers();
		virtual ~CQueueMsgHandlers(){};
		virtual inline T* getMatch(T*);

		virtual inline TBool explicitRemove(T* aElementToRemove);		
		
	};

template<class T>
CQueueMsgHandlers<T>::CQueueMsgHandlers() : CQueue<T>::CQueue(){}

template<class T>
T*	CQueueMsgHandlers<T>::getMatch(T* aInCommingMsg)
	{
		// 

		TSglQueIter<T> iterator(CQueue<T>::iList);
		iterator.SetToFirst();
		T* iMatchingOwner;
		
		while((iMatchingOwner = iterator++))
			{
			if(iMatchingOwner->IsOwnerOfResponse(*aInCommingMsg))
				{
				return iMatchingOwner;
				}
			}

		return NULL;
	}

template<class T>
TBool CQueueMsgHandlers<T>::explicitRemove(T* aElementToRemove)
	{
	CQueue<T>::iList.Remove(*aElementToRemove);
	CQueue<T>::iQLenght--;
		return TRUE;
		
	}

class CMSRPServerSubSession : public CBase, MMSRPConnectionObserver, MMSRPMessageObserver
    {
public:

	enum TQueueType
		{
		TClientQueue,
		TInCommingMsgQueue	
		};
	
    static CMSRPServerSubSession* NewL( CMSRPServerSession& aServerSession, CStateFactory& aStateFactory );
    
    static CMSRPServerSubSession* NewLC( CMSRPServerSession& aServerSession, CStateFactory& aStateFactory );
       
    virtual ~CMSRPServerSubSession( );
       
    TBool ServiceL( const RMessage2& aMessage );

	// From MMSRPConnectionObserver
	void ConnectionStateL( TInt aNewState, TInt aStatus );
	
	TBool MessageReceivedL( CMSRPMessageHandler* aMsg );
	       
	void UnclaimedMessageL( CMSRPMessageHandler* aMsg );

	// From MMSRPMsgObserver
	void MessageSendCompleteL();
	
	void MessageResponseSendCompleteL(CMSRPMessageHandler& aMsg);
	
	virtual void MessageSendProgressL(TInt aBytesSent, TInt aTotalBytes);
	
	virtual void MessageReceiveProgressL(TInt aBytesSent, TInt aTotalBytes);
		
	void WriterError();
     
	CMSRPMessageHandler* ReceiveFileMsgHandler();
	
	void NotifyFileReceiveResultToClientL(CMSRPMessageHandler *msgHandler);
	
	void NotifyFileSendResultToClientL(CMSRPMessageHandler *msgHandler);
	
private:
    CMSRPServerSession& iServerSession;
    CMSRPServerSubSession( CMSRPServerSession& aServerSession, CStateFactory &aStateFactory);
    void ConstructL();

	// ProcessEventL shall only take a event. The data associated with the event can be a 
	// RMessage(Client to Server events), CSendBuffer(buffer being send out by connection manager), 
	// CMSRPMessage(received message) or other such data.
	// The associated data is stored in the context and retrieved by the relevant state when needed.
	
	TBool CMSRPServerSubSession::ProcessEventL( TMSRPFSMEvent aEvent);
        
    HBufC8* CreateSubSessionIDL( );
    
    void HandleLocalPathRequestL( const RMessage2& aMessage );
       
    void HandleConnectRequestL( const RMessage2& aMessage );
        
    void HandleListenConnectionsL( const RMessage2& aMessage );
    void HandleListenMessagesL( const RMessage2& aMessage );
    void HandleListenSendResultL( const RMessage2& aMessage );
       
    void HandleSendMessageL( const RMessage2& aMessage );
        
    void HandleCancelSendingL( const RMessage2& aMessage );
    void HandleCancelReceivingL( const RMessage2& aMessage );
    void HandleCancelSendRespListening( const RMessage2& aMessage );        
        
    MMSRPConnectionManager& ConnectionManager( );

	void CompleteClient(TInt aReason);
	TInt Write(TInt aParam,const TDesC8& aDes);
		
	TBool QueueClientSendRequestsL();

	TBool QueueSendMessages(CMSRPMessageHandler* aMessageHandlers);
	CMSRPMessageHandler* DeQueueSentMessage();

	TBool QueueReceivedMessages();

	// Utility functions.

	TBool sendResultToClientL(CMSRPMessageHandler *incommingMsgHandler);
	TBool sendMsgToClientL(CMSRPMessageHandler *incommingMsgHandler);
	TBool SendProgressToClientL(CMSRPMessageHandler *msgHandler);
	TBool ReceiveProgressToClientL(CMSRPMessageHandler *msgHandler);

	void ReadSendDataPckgL();

	TBool listnerSetupComplete();

	CMSRPServerSubSession::TQueueType getQToProcess();

	TBool QueuesEmpty();
	TBool informConnectionReadyToClient();

	void QueueLog();

	TBool checkMessageForSelfL(CMSRPMessageHandler *aMsgHandler);
	TBool matchSessionIDL(const CMSRPHeaderBase *aPathHeader, TBool local = TRUE);
	TPtrC8 extractSessionID(const TDesC8& aPathBuffer);

	CStateFactory& StateFactory();
        
private:
	
	// Session Data.
    HBufC8* iLocalSessionID;
    HBufC8* iRemoteSessionID;

	// Connection info bundle.        
    TBuf8< KMaxLengthOfHost > iRemoteHost;            
    TUint iRemotePort;
	TConnectionDirection  iConnDirection;

	// Local Host info
    TBuf< KMaxLengthOfHost > iLocalHost;

	MMSRPConnection* iConnection;
						    
	// connect package from the client
	TPckgBuf< TLocalPathMSRPData > iLocalPathMSRPDataPckg;
	TPckgBuf< TConnectMSRPData > iConnectMSRPdataPckg;
        
    // listen package from the client
    TPckgBuf< TListenMSRPData > iListenMSRPdataPckg;
        
    // send package to the client
    TPckgBuf< TSendMSRPData > iSendMSRPdataPckg;
        
    // result of sent messages package to the client
    TPckgBuf< TListenSendResultMSRPData > iSendResultListenMSRPDataPckg;

	// Temorary Event Data.
	const RMessage2* iClientMessage;
	CMSRPMessageHandler* iReceivedMsg;
	CMSRPMessageHandler* iReceivedResp;
	CMSRPMessageHandler* iReceiveFileMsgHdler;
	TBool iFileShare; //is previous state file share
	TBool iSendCompleteNotify; //
	TBool iReceiveCompleteNotify;
	TInt iBytesTransferred;
	TInt iTotalBytes;
	
	// Client Listners waiting to be completed.
	
	// Listner for response of Connect request.
	CRMessageContainer iConnectionListner; 
	// Listner for response of Listen request(iMessage set as false) and also 
	// listner for incomming messages(iMessage set as TRUE).
	CRMessageContainer iIncommingMessageListner;
	// Listner for Responses to sent messages.
	CRMessageContainer iResponseListner;

	CStateFactory& iStateFactory;	// Not owned.
	TStateBase* iState;				// Not owned.
	
	// The current message handler that is doing a MSRP Send and 
	// for which the subsession has still not rerceived a sendmessagecomplete event.
	// Once the sendmessage completed is received the message handler is put on the 
	// OutGoingMessage queue where it waits for a response to arrive.
	CMSRPMessageHandler *iCurrentMsgHandler;	

	// Queues.	
	CQueueMsgHandlers<CMSRPMessageHandler> iOutMsgQ;
	CQueueMsgHandlers<CMSRPMessageHandler> iInCommingMsgQ;
	CQueueMsgHandlers<CMSRPMessageHandler> iPendingSendMsgQ;
	CQueueMsgHandlers<CMSRPMessageHandler> iPendingForDeletionQ;		

	friend class TStateBase;
	friend class TStateIdle; 
	friend class TStateConnecting; 
	friend class TStateActive;
	friend class TStateActiveSend;
	friend class TStateWaitForClient; 
	friend class TStateFileShare;
	friend class TStateError; // Not a very good thing !! Need change.

	#ifdef __UT_TSTATEFACTORY_H__
	friend class UT_TStateFactory;
	#endif

    };

#endif /* CMSRPSERVERSUBSESSION_H_ */
