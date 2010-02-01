/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CTransport.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CTRANSPORT_H__
#define __CTRANSPORT_H__

#include "CTransportBase.h"
#include "SipStackServerDefs.h"
#include "TResponseRoute.h"
#include "CSipConnectionMgr.h"


//SIP version 
_LIT8(KDefaultSipVersion, "SIP/2.0");
_LIT8(KDefaultProtocol, "SIP");
_LIT8(KDefaultVersion, "2.0");

class MTransactionFinder;
class MServerTaFactory;
class MTransportOwner;
class CSIPServerResolver;
class CNetworkInfo;
class COwnerSettingsList;
class CSIPMessage;
class CSIPRequest;
class CSIPResponse;
class CSIPViaHeader;
class CTransportErrorHandler;
class CSIPHostPort;
class CSIPNATTraversalController;
class CSocketContainer;
class MSIPNATBindingObserver;

class CTransport : public CTransportBase
	{
public:

	virtual ~CTransport();

public: // From CTransportBase	   
                     
	void Send( const TSIPTransportParams& aParams,
	           CSIPResponse& aResponse,
			   TTransactionId aId,
			   TRequestStatus &aStatus );
	void Send( const TSIPTransportParams& aParams,
	           CSIPResponse& aResponse,
			   const TInetAddr& aAddr,
			   TRequestStatus &aStatus );
	void Send( const TSIPTransportParams& aParams,
	           CSIPRequest& aRequest, 
	           TBool aForceUDP,
	           const TInetAddr& aAddr,
			   TUint aOrigTransport, 
			   TRequestStatus &aStatus,
			   TBool aPublicAddrResolved );
			  
	TBool HandleL( const TSIPTransportParams& aParams, 
	               CSIPMessage* aMessage, 
	               TTransactionId aId,
	               TBool aIsStrict );
	TBool HandleL( const TSIPTransportParams& aParams, 
	               CSIPMessage* aMessage, 
	               const TInetAddr& aAddr,
	               TUint aLocalPort,
	               TBool aIsStrict );
	               
	virtual TBool HandleMessage( const TSIPTransportParams& aParams,
	                             RStringF aProtocol,
							 	 const TInetAddr& aRemoteAddr,
							 	 TUint aLocalPort,
							 	 TBool aIsStrict ) = 0;
	
	virtual TBool Handle( TUint32 aTransportId );
	
	TBool Match( const TSIPTransportParams& aParams );
	
	void GetAddrFromResponseL( const TSIPTransportParams& aParams,
	                           CSIPMessage& aMessage, 
	                           TInetAddr& aAddr );
	TUint ProtocolType();
	TBool HasRemoteHost( const TSIPTransportParams& aParams,
	                     const TInetAddr& aRemoteHost,
	                     RStringF aProtocol );
	
	virtual void Disconnect( const TInetAddr& /*aRemoteHost*/ ) {}
	virtual void ICMPErrorL( CSipConnectionMgr::TICMPError /*aICMPError*/,
							 const TInetAddr& /*aSourceAddr*/ ) {}
	
	virtual TBool Remove( const TSIPTransportParams& aParams );
	virtual TBool Remove( TUint32 /*aTransportId*/ ) {return ETrue;}
	virtual TBool Remove( MSIPNATBindingObserver& aSIPNATBindingObserver );
	
	virtual TBool Shutdown( TUint32 aTransportId,
	                        MSIPTransportRemovalObserver* aRemovalObserver );
    
    virtual TBool AddUserL( const TSIPTransportParams& /*aParams*/, 
                            TUint /*aReceivePort*/ ) {return EFalse;}
                           
    TBool IsReservedTransport() const;
    
    virtual RSocket* AcquireUdpSocket( 
        MSocketUsagePermissionObserver* aObserver,
        const TInetAddr& aRemoteAddr,
        TBool& aPermissionToUse );
                                        
    virtual void UdpSocketReleased( MSocketUsagePermissionObserver* aObserver,
                                    RSocket& aSocket ); 
    
    virtual TInetAddr* PersistentRemoteAddr( 
                            MSIPNATBindingObserver* aBindingObserver );
    

public:

    static void ConvertToInetAddrL( const TDesC8& aAddrStr, TInetAddr& aAddr );

    static HBufC8* ConvertInetAddrToTextL( const TInetAddr& aAddr );
    
    static CSIPViaHeader* TopViaHeader( CSIPMessage* aMessage );
	
protected:
	
	// Uses cleanupstack for aData inside method, so caller should not 
	// push aData to cleanupstack before calling this method
	void RecvL( HBufC8* aData,
			    const TInetAddr& aRemoteAddr,
			    TBool aCompressed );
	void RecvL( CSIPMessage* aMessage,
			    const TInetAddr& aRemoteAddr,
			    TInt aParserError,
			    TBool aCompressed );
	CTransport( MTransactionFinder& aFinder, 
				MServerTaFactory& aTaFactory,
				MTransportOwner& aTransportOwner,
				CSIPServerResolver& aServerResolver,
				MSigCompController& aSigCompHandler,
				const TSIPTransportParams& aTransportParams,
				CNetworkInfo& aNetworkInfo,
				COwnerSettingsList& aOwnerSettingsList,
				CSIPNATTraversalController& aNATTraversal );
	virtual TBool CancelResponseSend( TTransactionId aId, TBool aCancelAlso2xxResponses );
	virtual void SendToNetL( const TSIPTransportParams& aParams,
	                         const TInetAddr& aAddress, 
	                         CSIPMessage& aMessage, 
	                         TBool aForceUDP,
				             TUint aOrigTransport, 
				             TRequestStatus &aStatus );
	virtual void SendToNetL( const TSIPTransportParams& aParams,
	                         const TInetAddr& aAddress, 
	                         TTransactionId aId, 
				             CSIPMessage& aMessage, 
				             TRequestStatus &aStatus );
	virtual void SendToNetL( const TSIPTransportParams& aParams,
	                         const TInetAddr& aAddress,
							 CSIPMessage& aMessage,
							 TRequestStatus &aStatus );
	virtual TUint SourcePort() = 0;
	virtual TUint Protocol() = 0;
	virtual CTransportErrorHandler& ErrorHandler() = 0;
	virtual CSocketContainer& GetSocketContainerL( const TInetAddr& aRemoteAddr ) = 0;
	virtual void InformSendingStatus( const TInetAddr& aRemoteAddr ) = 0; 
	virtual void SetPersistencyL( const TInetAddr& aRemoteAddr, 
	                              TBool aIsPersistent,
	                              MSIPNATBindingObserver* aBindingObserver );
	TBool IsPersistent() const;
	TBool IsDefault() const;
	void RemoveResponseRoute( const TInetAddr& aRemoteAddr );
	void RemoveResponseRoute( TTransactionId aId );
	HBufC8* LocalAddressL();
	void HandleMixedAddressFamilysL( CSIPMessage& aMessage, 
	                                 const TInetAddr& aRemoteAddr );	

    MSigCompController* SigCompressionHandler() {return iSigCompHandler;}
    
    TBool IsConnectedTransport();
    
    TBool NotifyFlowFailure();
    
protected: // Data
	
	MTransportOwner* iTransportOwner;
	RArray<TResponseRoute> iIDArray;
	
	// Binding observers are not owned
	RPointerArray<MSIPNATBindingObserver> iBindingObservers;
	
	CSIPServerResolver* iServerResolver;
	
    TSIPTransportParams iTransportParams;
    COwnerSettingsList& iSettingsList;  
    
    CSIPNATTraversalController& iNATTraversal;
	
private:

	void HandleClientRequestL( const TSIPTransportParams& aParams,
	                           CSIPRequest& aRequest,
	                           TBool aPublicAddrResolved );
	
	//Obtains address from the top Via-header of aMessage, and places it
	//into aAddr
	void HandleClientResponseL( const TSIPTransportParams& aParams,
	                            CSIPMessage* aMessage, 
	                            TInetAddr& aAddr );
	
	TBool HandleServerResponseL( CSIPMessage* aMessage,
								 const TInetAddr& aAddr );
	TInt HandleServerRequestL( CSIPMessage* aMessage,
							   const TInetAddr& aAddr );
	
	TBool MethodIsSameInCSecAndReqLine( CSIPRequest& aMessage );
	TBool AllMandatoryHeaders( CSIPMessage& aMessage );
	TBool SupportedSIPVersion( CSIPMessage& aMessage );
	TBool ContentTypeOk( CSIPMessage& aMessage );
	TBool OnlyOneViaHeader( CSIPMessage& aMessage );
	TBool CorrectProtocolInVia( CSIPMessage& aMessage );
	TInt CheckAndUpdateContentLengthL( CSIPMessage* aMessage,
									   const TInetAddr& aAddr );
	TBool IsAck( CSIPMessage& aMessage );
	void HandleSigCompAllowDenyL( const TInetAddr& aRemoteAddr, 
                                  TInt aParserError,
                                  TBool aCompressed );
	void UpdateListenersL( const TSIPTransportParams& aParams, 
	                       CSIPMessage* aMessage );
	void SetReceivedIfNeededL( CSIPViaHeader* aViaHeader, TInetAddr aAddr );

	void RouteL( CSIPMessage* aMessage, const TInetAddr& aRemoteAddr );
	
	void SendErrorResponseL( TUint aResponseCode, 
	                         TInt aStrIndex, 
	                         CSIPMessage& aMessage,
	                         const TInetAddr& aAddr );
	
	void UpdateContactHeadersL( const TSIPTransportParams& aParams,
	                            CSIPMessage* aMessage,
	                            TBool aUpdatePorts );         
	                            
	void ChangeHostAddressFamilyIfNeededL( CSIPHostPort& aHostPort, 
                                           const TInetAddr& aRemoteAddr );
	
	TBool HandleClientResponseRPortL( CSIPViaHeader& aViaHeader, TInetAddr& aAddr );
	
	void HandleResponseNATTraversalL( CSIPResponse& aResponse, 
	                                  const TInetAddr& aRemoteAddr,
	                                  MSIPNATBindingObserver* aBindingObserver );
	 
	TBool HasValue( CSIPViaHeader& aViaHeader, RStringF& aParam );
	
	void AddBindingObserverL( MSIPNATBindingObserver& aBindingObserver );
	
	TBool RemoveBindingObserver( MSIPNATBindingObserver& aBindingObserver );
	
private: // Data

	MTransactionFinder* iTaFinder;
	MServerTaFactory* iTaFactory;
	MSigCompController* iSigCompHandler;
	
	TTimerId iTimerId;
	TUint iTime;

	//For constructing P-Access-Network-Info header
	CNetworkInfo& iNetworkInfo;	
private: //For testing purposes
#ifdef CPPUNIT_TEST	
	friend class CValidTest;
	friend class CSipConnectionTest;
	friend class CTransportTlsTest;
#endif
	};
#endif // end of __CTRANSPORT_H__

// End of File
