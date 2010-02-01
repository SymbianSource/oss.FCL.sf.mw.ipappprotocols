// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : CNATTraversalControllerExampleExample.cpp
// Part of       : SIP NAT Traversal Controller Example
// Version       : 
//



#include "CNATTraversalControllerExample.h"
#include "sipnattraversalcontrollerinitparams.h"
#include "sipnatbindingobserver.h"
#include "sipnattraversalrequestobserver.h"

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::NewL
// -----------------------------------------------------------------------------
//
CNATTraversalControllerExample* CNATTraversalControllerExample::NewL(
    TAny* aInitParams)
	{
	__ASSERT_ALWAYS (aInitParams != NULL, User::Leave(KErrArgument));
	TSIPNATTraversalControllerInitParams* params =
		static_cast<TSIPNATTraversalControllerInitParams*>(aInitParams);
    CNATTraversalControllerExample* self = 
        new(ELeave)CNATTraversalControllerExample(params->iSocketServ);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::CNATTraversalControllerExample
// -----------------------------------------------------------------------------
//
CNATTraversalControllerExample::CNATTraversalControllerExample(
    RSocketServ& aSocketServ)
 : iSocketServ(aSocketServ),
   iRequestOrder(TNATTraversalPendingRequest::RequestOrder)
	{
	}

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::ConstructL
// -----------------------------------------------------------------------------
//
void CNATTraversalControllerExample::ConstructL()
    {
    iDeltaTimer = CDeltaTimer::NewL(CActive::EPriorityStandard); 
    }

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::~CNATTraversalControllerExample
// -----------------------------------------------------------------------------
//
CNATTraversalControllerExample::~CNATTraversalControllerExample()
	{
	for (TInt i=0; i < iPendingRequests.Count(); i++)
	    {
	    if (iDeltaTimer)
	        {
	        iDeltaTimer->Remove(iPendingRequests[i].DeltaTimerEntry());
	        }
	    }
	iPendingRequests.Close();
	delete iDeltaTimer;
	}

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::ResolvePublicAddrL
// From CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
TUint32 CNATTraversalControllerExample::ResolvePublicAddrL(
    TUint32 /*aIapId*/,
    RConnection& /*aConnection*/,
    const TInetAddr& /*aLocalAddr*/,
    const TDesC8& /*aDomain*/,
    RSocket& /*aUdpSocket*/,
    const TInetAddr& /*aNextHopAddr*/,
    MSIPNATBindingObserver* aBindingObserver,
    MSIPNATTraversalRequestObserver& aRequestObserver)
    {
    TUint32 requestId = NextRequestId();
    
    if (!BindingExists(aBindingObserver))
        {
        // Initiate actions to fetch the public IP address of aUdpSocket.
        // For example STUN protocol (RFC 3489) can be used for this purpose.
        // When the public IP address and port have been discovered
        // MSIPNATTraversalRequestObserver::PublicAddrResolved should be called.
        }
    else
        {
        // Start a timer with a minimum value to make the call asynchronous
        TNATTraversalPendingRequest request(requestId,&aRequestObserver,*this);
        iPendingRequests.InsertInOrderL(request,iRequestOrder);
        TInt index = iPendingRequests.FindInOrder(request,iRequestOrder);
        iDeltaTimer->Queue(1,iPendingRequests[index].DeltaTimerEntry());
        }
        
    return requestId;
    }

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::Cancel
// From CSIPNATTraversalController
// -----------------------------------------------------------------------------
//        
void CNATTraversalControllerExample::Cancel(TUint32 aRequestId)
    {
    TNATTraversalPendingRequest tmp(aRequestId,0,*this);
    TInt index = iPendingRequests.FindInOrder(tmp,iRequestOrder);
    if (index >= 0)
        {
        iDeltaTimer->Remove(iPendingRequests[index].DeltaTimerEntry());
        iPendingRequests.Remove(index);
        }
    else
        {
        // Cancel ongoing IP address discovery     
        }
    }

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::RefreshNATBindingL
// From CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
TBool CNATTraversalControllerExample::RefreshNATBindingL(
    RSocket& aSocket,
    const MSIPNATBindingObserver* /*aBindingObserver*/)
    {
    TBool maintainPersistentConnection = EFalse;
    
    // If it was detected that the terminal is behind a NAT, 
    // some kind of NAT binding keepalive should be started.
    // Some suggestions about the keepalive mechanisms can be found from
    // http://www.ietf.org/internet-drafts/draft-ietf-sip-outbound-04.txt    
    // aBindingObserver can be used for notifying flow failures 
    // specified in the above draft.

    if (Protocol(aSocket) == KProtocolInetTcp)
        {
        // If it was found that the terminal is behind a NAT
        // and the protocol of aSocket is TCP, 
        // then maintainPersistentConnection should be set to ETrue
        // to keep the TCP connection open for NAT binding keepalive.
        }

	return maintainPersistentConnection;
    }

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::RefreshNATBindingL
// From CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
TBool CNATTraversalControllerExample::RefreshNATBindingL(
    CSecureSocket& /*aSecureSocket*/,
    const MSIPNATBindingObserver* /*aBindingObserver*/)
    {
    TBool maintainPersistentConnection = EFalse;

    // If it was detected that the terminal is behind a NAT, 
    // some kind of NAT binding keepalive should be started and
    // maintainPersistentConnection should be set to ETrue
    // to keep the TLS connection open for NAT binding keepalive.

	return maintainPersistentConnection;
    }

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::UpdateNextHop
// From CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
void CNATTraversalControllerExample::UpdateNextHop( 
    const TInetAddr& /*aNextHopAddr*/,
    const MSIPNATBindingObserver* /*aBindingObserver*/ )
    {
    // For UDP based NAT binding keepalives the destination
    // for the NAT binding keepalives may change.
    // The new address must be taken into use.
    }

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::SocketIdle
// From CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
void CNATTraversalControllerExample::SocketIdle(
    TBool aIdle,
    RSocket& /*aSocket*/)
    {
    if (aIdle)
        {
        // SIP stack is not currently using aSocket for sending.
        // NAT bindings created with aSocket can be refreshed. 
        // See for example:
        // http://www.ietf.org/internet-drafts/draft-ietf-sip-outbound-04.txt
        }
    else
        {
        // SIP stack needs aSocket for sending.
        // It may not be used for refreshing NAT bindings 
        // and any outstanding request for aSocket must be cancelled 
        }    
    }
    
// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::SocketIdle
// From CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
void CNATTraversalControllerExample::SocketIdle(
    TBool aIdle,
    CSecureSocket& /*aSecureSocket*/)
    {
    if (aIdle)
        {
        // SIP stack is not currently using aSecureSocket for sending.
        // NAT bindings created with aSecureSocket can be refreshed. 
        // See for example:
        // http://www.ietf.org/internet-drafts/draft-ietf-sip-outbound-04.txt
        }
    else
        {
        // SIP stack needs aSecureSocket for sending.
        // It may not be used for refreshing NAT bindings 
        // and any outstanding request for aSocket must be cancelled 
        }
    }
            
// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::DataReceived
// From CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
void CNATTraversalControllerExample::DataReceivedL(
    const TDesC8& /*aData*/,
    const RSocket& /*aSocket*/,
    TBool& /*aHandled*/)
    {
    // SIP stack has received aData from aSocket.
    // This data has not been recognized as a SIP request nor SIP response.
    // This data can be for example a STUN response, 
    // if STUN is used for public IP address discovery or NAT bidning keepalive.
    }    

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::FreeResources
// From CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
void CNATTraversalControllerExample::FreeResources(
    TUint32 /*aIapId*/)
    {
	// SIP stack is closing RConnection that is related to aIapId.
	// All the resources reserved using this RConnection must be freed
	// and the related sockets must not be used anymore. 
    }

// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::FreeResources
// From CSIPNATTraversalController
// -----------------------------------------------------------------------------
//
void CNATTraversalControllerExample::FreeResources(
    MSIPNATBindingObserver& /*aBindingObserver*/)
    {
	// SIP stack is deleting aBindingObserver.
	// All the resources reserved using this aBindingObserver must be freed.
    }
    
// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::RequestCompleted
// From MNATTraversalPendingRequestContainer
// -----------------------------------------------------------------------------
//    
void CNATTraversalControllerExample::RequestCompleted(
    TUint32 aRequestId)
    {
    TNATTraversalPendingRequest tmp(aRequestId,0,*this);
    TInt index = iPendingRequests.FindInOrder(tmp,iRequestOrder);
    if (index >= 0)
        {
        TNATTraversalPendingRequest request = iPendingRequests[index]; // copy
        iDeltaTimer->Remove(iPendingRequests[index].DeltaTimerEntry());
        iPendingRequests.Remove(index); // remove from array
        request.CompleteRequest(); // use copy for informing the observer
        }
    }
    
// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::NextRequestId
// -----------------------------------------------------------------------------
//
TUint32 CNATTraversalControllerExample::NextRequestId()
	{
    if (iRequestIdCounter == KMaxTUint32)
		{
		iRequestIdCounter = 1;
		}
	else
		{
		iRequestIdCounter++;
		}
	return iRequestIdCounter;
	}
    
// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::Protocol
// -----------------------------------------------------------------------------
//
TUint CNATTraversalControllerExample::Protocol(RSocket& aSocket) const
    {
    TProtocolDesc proto;
    aSocket.Info(proto);
    return proto.iProtocol;
    }
    
// -----------------------------------------------------------------------------
// CNATTraversalControllerExample::BindingExists
// -----------------------------------------------------------------------------
//   
TBool CNATTraversalControllerExample::BindingExists(
    const MSIPNATBindingObserver* /*aObserver*/) const
    {
	// Find NAT binding keepalive related to aObserver
	// If it exists, return ETrue.
	return ETrue;    
    }
