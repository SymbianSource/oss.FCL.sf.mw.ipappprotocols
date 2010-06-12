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

#ifndef CMSRPCONNECTION_H
#define CMSRPCONNECTION_H

// INCLUDES

#include "MMSRPConnector.h"
#include "MMSRPConnection.h"
#include "MMSRPReader.h"
#include "MMSRPReaderObserver.h"
#include "MMSRPParser.h"
#include "MMSRPWriter.h"
#include "MMSRPConnectionObserver.h"
#include "MMSRPTimeOutObserver.h"
#include "CMSRPTimeOutTimer.h"

// FORWARD DECLARATIONS
class CMSRPMessageHandler;
class MMSRPConnection;
class MMSRPConnectionObserver;
class MMSRPReaderObserver;
class MMSRPWriterObserver;
class MMSRPReader;
class MMSRPConnectionManager;

// CLASS DECLARATION

/**
*  MSRP Connection Manager
*/
class CMSRPConnection : public CBase, public MMSRPConnection, public MMSRPConnectorObserver, 
							 public MMSRPReaderObserver, public MMSRPParserObserver, 
							 public MMSRPWriterErrorObserver, public MMSRPTimeOutObserver
    {
	public:

	    // Constructors and destructor
	    
	    static MMSRPConnection* NewL( TInetAddr& aRemoteAddr, MMSRPConnectionManager& aConnMngr);
	    
        virtual ~CMSRPConnection();

    public: // Functions from base classes

		// From MMSRPConnection
        virtual TBool CheckConnection(TInetAddr& aRemoteAddr, TBool aOnlyListen);
           
        virtual TInt ConnectL(MMSRPConnectionObserver& aSubsession);
        
        virtual TInt ListenL(MMSRPConnectionObserver& aSubsession);

        virtual void ReleaseConnection(MMSRPConnectionObserver& aSubsession);
                                    
        virtual void SendL( MMSRPWriterObserver& aMsg /*, MMSRPConnectionObserver& aSubsession*/  );
                                                             
        virtual void ConnectionEstablishedL( TInt aNewState, RSocket* aSocket, TInt aStatus );

		virtual TInt getConnectionState();
        
        //from MMSRPConnectorObserver 
        
        //virtual void ConnectionEstablishedL( TInt aNewState, RSocket* aDataSocket, TInt aStatus );
        
        //from MMSRPReaderObserver

        virtual void ReadStatusL(RMsrpBuf& aBuf, TInt aStatus);
        
        //from MMSRPParserObserver
        virtual void ParseStatusL (CMSRPMessageHandler* aMsg, TInt aStatus);
                                        
        //from MMSRPWriterErrorObserver
        virtual void WriteSocketError(TInt aError);
        
        //from timeout
        void TimerExpiredL();
        

	private: // constructor

        CMSRPConnection(TInetAddr& aRemoteAddr, MMSRPConnectionManager& aConnMngr );

        void ConstructL( );
        
        void Remove(MMSRPConnectionObserver* aSubsession);
        
        void NotifyAllL(TInt aNewState, TInt aStatus );
        
        void CloseConnection( );


	private: // data
		
		TInetAddr iHostAddress;
		
		MMSRPConnectionManager& iConnMngr;
		
		TInt iConnectionState;
		
		RPointerArray<MMSRPConnectionObserver> iSubsessions;
		
		MMSRPConnector* iConnector; 
		
		MMSRPReader* iReader;
		
		MMSRPParser* iParser;
		
		MMSRPWriter* iWriter;
		
		CMSRPBufPool iBufPool;
										
		RSocket* iSocket;
		
		CMSRPTimeOutTimer*  iListenTimer;

    };

#endif      // CMSRPCONNECTION_H

// End of File
