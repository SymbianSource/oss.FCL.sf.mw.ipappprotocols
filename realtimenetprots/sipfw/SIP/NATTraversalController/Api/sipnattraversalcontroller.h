/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipnattraversalcontroller.h
* Part of       : SIP NAT Traversal Controller
* Version       : SIP/5.0 
*
*/



#ifndef CSIPNATTRAVERSALCONTROLLER_H
#define CSIPNATTRAVERSALCONTROLLER_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <es_sock.h>
#include <in_sock.h>
#include <securesocket.h>

// FORWARD DECLARATIONS
class MSIPNATBindingObserver;
class MSIPNATTraversalRequestObserver;

// CONSTANTS
/** Interface UID of this ECOM interface */
const TUid KSIPNATTraversalControllerIFUid = { 0x102747d0 };

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* Interface for SIP NAT Traversal Controller ECOM plug-in(s).
*/
class CSIPNATTraversalController : public CBase
    {
    public: // Constructors and destructor
    
        /**
        * Destructor.
        */
        virtual inline ~CSIPNATTraversalController();

    public: // New functions

        /**
        * Resolves the public IP address and port related to 
        * the given IAP and UDP socket. 
        * This address will be used in the Contact-header 
        * of an outgoing SIP request.
        * @param aIapId IAP id
        * @param aConnection a opened and started connection
        * @param aLocalAddr a local address for the IAP
        * @param aDomain domain that can be used in service resolution
        *        in for example DNS queries for STUN service.
        * @param aUdpSocket handle to a UDP socket bind to the same local port 
        *        as the socket that will be used for sending SIP requests 
        *        and should be used for the public address discovery.
        * @param aNextHopAddr next hop IP address and port for 
        *        possibly needed NAT binding keepalives
        * @param aBindingObserver identifies the user for the 
        *        resources possibly reserved by this call. 
        *        If not NULL, can be later used for resource cleanup and
        *        will be informed if the created NAT binding fails.
        *        The ownership is not transferred.      
        * @param aRequestObserver observer to be informed when 
        *        the asynchronous request completes
        * @return a request ID
        */
        virtual TUint32 ResolvePublicAddrL(
            TUint32 aIapId,
            RConnection& aConnection,
            const TInetAddr& aLocalAddr,
            const TDesC8& aDomain,
            RSocket& aUdpSocket,
            const TInetAddr& aNextHopAddr,
            MSIPNATBindingObserver* aBindingObserver,
            MSIPNATTraversalRequestObserver& aRequestObserver ) = 0;
        
        /**
        * Cancels a particular outstanding request
        * @param aRequestId a request ID to be canceled
        */
        virtual void Cancel( TUint32 aRequestId ) = 0;        

        /**
        * Creates a NAT binding keepalive if the public address resolved
        * with ResolvePublicAddrL differs from the local IP address. 
        * If a keepalive already exists for aBindingObserver,
        * nothing should be done.
        * @param aSocket The socket that should be used 
        *        for NAT binding keepalives.
        * @param aBindingObserver identifies the user for the 
        *        resources possibly reserved by this call. 
        *        If not NULL, can be later used for resource cleanup and
        *        should be informed if the created NAT binding fails.
        *        The ownership is not transferred.
        * @return ETrue, if aSocket uses TCP and the TCP
        *         connection should be kept persistent 
        *         in order to allow the keepalive. 
        *         Otherwise EFalse. 
        */
        virtual TBool RefreshNATBindingL( 
            RSocket& aSocket,
            const MSIPNATBindingObserver* aBindingObserver ) = 0;
            
        /**
        * Creates a NAT binding keepalive if the public address resolved
        * with ResolvePublicAddrL differs from the local IP address. 
        * If a keepalive already exists for aBindingObserver,
        * nothing should be done.
        * @param aSecureSocket The socket that should be used 
        *        for NAT binding keepalives.
        * @param aBindingObserver identifies the user for the 
        *        resources possibly reserved by this call. 
        *        If not NULL, can be later used for resource cleanup and
        *        should be informed if the created NAT binding fails.
        *        The ownership is not transferred.
        * @return ETrue, if the TLS connection should be persistent.
        *         in order to allow the keepalive.
        *         Otherwise EFalse. 
        */
        virtual TBool RefreshNATBindingL(
            CSecureSocket& aSecureSocket,
            const MSIPNATBindingObserver* aBindingObserver ) = 0;

        /**
        * In case of a UDP based NAT binding keepalive,
        * the destination for the keepalive messages may change.
        * The new address must be taken into use if the
        * related aBindingObserver was found.
        * @param aNextHopAddr ETrue if idle otherwise EFalse
        * @param aBindingObserver identifies the NAT binding keepalive
        *        The ownership is not transferred.
        */
        virtual void UpdateNextHop( 
            const TInetAddr& aNextHopAddr,
            const MSIPNATBindingObserver* aBindingObserver ) = 0;

        /**
        * Informs that the socket is idle or not idle and 
        * can or cannot be used for sending. 
        * @param aIdle ETrue if idle otherwise EFalse
        * @param aSocket handle to the socket
        */    
        virtual void SocketIdle( 
            TBool aIdle,
            RSocket& aSocket ) = 0;
            
        /**
        * Informs that the socket is idle or not idle and 
        * can or cannot be used for sending. 
        * @param aIdle ETrue if idle otherwise EFalse
        * @param aSecureSocket handle to the TLS socket
        */    
        virtual void SocketIdle( 
            TBool aIdle,
            CSecureSocket& aSecureSocket ) = 0;
            
        /**
        * Data was received from a socket 
        * @param aData data received from the socket
        * @param aUdpSocket handle to the UDP socket
        * @param aHandle on return ETrue if the data was succesfully handled,
        *        otherwise EFalse.
        */    
        virtual void DataReceivedL( 
            const TDesC8& aData,
            const RSocket& aUdpSocket,
            TBool& aHandled ) = 0;    

        /**
        * Frees all the resources reserved related to an IAP.
        * @param aIapId IAP id
        */
        virtual void FreeResources( 
            TUint32 aIapId ) = 0;

        /**
        * Frees all the resources reserved for the observer.
        * @param aBindingObserver a observer
        */
        virtual void FreeResources( 
            MSIPNATBindingObserver& aBindingObserver ) = 0;

    public: // Data

        /// Unique key for implementations of this interface.
        TUid iInstanceKey;

    protected: // Constructors:

        inline CSIPNATTraversalController();
    };
    
#include "sipnattraversalcontroller.inl"

#endif // CSIPNATTRAVERSALCONTROLLER_H
