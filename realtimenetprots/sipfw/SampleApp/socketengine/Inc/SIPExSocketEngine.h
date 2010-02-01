
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
*
*/



#ifndef __CSIPEXSOCKETENGINE_H__
#define __CSIPEXSOCKETENGINE_H__

// INCLUDES
#include    "SIPExTimeOutTimer.h"
#include    "SIPExSocketEngineObserver.h"
#include    "SIPEXReaderNotifier.h"
#include    "SIPExWriterNotifier.h"
#include    "SIPExTimeOutNotify.h"
#include    <in_sock.h>     // SInetAddr


// FORWARD DECLARATIONS
class MSIPExSocketEngineObserver;
class CSIPExSocketReader;
class CSIPExSocketWriter;
class CSIPExTimeOutTimer;

// CLASS DECLARATIONS
/**
* Implements socket connections.
*/
class CSIPExSocketEngine 
:   public CActive, 
    public MSIPExTimeOutNotify,
    public MSIPExReaderNotifier,
    public MSIPExWriterNotifier
    {
    public:

        /**
        * Static constructor
        * @param aObserver A reference to the socket engine observer.
        */
        IMPORT_C static CSIPExSocketEngine* NewL( MSIPExSocketEngineObserver& aObserver );

        /**
        * Static constructor. On return the instance is left to the CleanupStack
        * @param aObserver A reference to the socket engine observer.
        */
        IMPORT_C static CSIPExSocketEngine* NewLC( MSIPExSocketEngineObserver& aObserver );

        /**
        * Destructor
        */
        IMPORT_C ~CSIPExSocketEngine();

    public: // From base classes

        /**
        * Connecting the socket to the aIPAddress.
        * @param aIapId An iap id used for connection
        * @param aIPAddress An ip address where to connect.
        */
        IMPORT_C void ConnectL( const TUint32 aIapId, const TInetAddr& aIPAddress );
        
        /**
        * Writes to the socket.
        * @param aTxt A text to write to the socket.
        */
        IMPORT_C void Write( const TDesC8& aTxt );

        /**
        * Reads from the socket.
        */
        IMPORT_C void Read();

        /**
        * Closing the socket.
        */
        IMPORT_C void Stop();

        /**
        * Starts listening local game port
        * @param aIapId The IAP id of the active accesspoint. 
        * @return A local address
        */
        IMPORT_C TInetAddr& StartListeningL( const TUint32 aIapId );

    public: 

        /**
        * Socket engine state enumerations.
        */
        enum TSocketEngineState
            {
            ENotConnected,
            EConnecting,
            EConnected,
            EListening,
            ETimedOut
            };

    protected: // From base classes

        // From MSIPExTimeOutNotify
        /**
        * From MSIPExTimeOutNotify
        * Destructor
        */
        void TimerExpired();

        // From MSIPExReaderNotifier

        /**
        * From MSIPExReaderNotifier
        */
        void MessageReceived( TDesC8& aBuffer );

        /**
        * From MSIPExReaderNotifier
        */
        void ErrorInReading( TInt aError );

        // From MSIPExWriterNotifier

        /**
        * From MSIPExWriterNotifier
        */
        void WriteDone();
        
        /**
        * From MSIPExWriterNotifier
        */
        void ErrorInWriting( TInt aError );

    protected: // From base classes
        
        /**
        * From CActive
        * Cancels any outstanding request.
        */
        void DoCancel();
        
        /**
        * From CActive
        * Handles the completion of the active request.
        */
        void RunL();

    private: // New functions

        /**
        * Changes the new state to iState and notifies observer.
        * @param aNewState A new state of socket engine
        */
        void ChangeStateAndNotify( TSocketEngineState aNewState );

         /**
        * Activates PDP ctx of passed iap
        * @param aIapId The IAP id of the activated accesspoint.
        */
        void StartInterfaceL( const TUint32 aIapId );
        
        /**
        * Gets the local IP address with host resolver.
        * The address is set to the iAddress variable.
        */
        void ResolveLocalIPAddressL();

    private:

        /**
        * Destructor
        */
        CSIPExSocketEngine( MSIPExSocketEngineObserver& aObserver );

        /**
        * Destructor
        */
        void ConstructL();

    private:
        // Member variables

        // The reference to the socket engine observer
        MSIPExSocketEngineObserver&  iObserver;

        // Sockets
        RSocketServ     iSocketServer;
        RConnection     iConnection;
        RSocket         iDataSocket;
        RSocket         iListenSocket;        
        

        // 
        CSIPExSocketReader*  iReader;
        CSIPExSocketWriter*  iWriter;
        CSIPExTimeOutTimer*  iTimer;

        // Socket engine's state
        TSocketEngineState  iState;

        // The remote address
        TInetAddr   iAddress;
    };

#endif // __CSIPEXSOCKETENGINE_H__

// End of file
