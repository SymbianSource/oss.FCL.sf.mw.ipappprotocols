
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



#ifndef __CSIPEXSOCKETREADER_H__
#define __CSIPEXSOCKETREADER_H__

#include <e32base.h>

// FORWARD DECLARATIONS
class MSIPExReaderNotifier;
class RSocket;

/**
* CSIPExSocketReader
* Reads data from the connected socket.
*/
class CSIPExSocketReader : public CActive
    {
    public:

        /**
        * NewL
        * Static constructor
        * @param aSocket The reference to the socket
        * @param aNotifier The reference to the reader notifier
        */
        static CSIPExSocketReader* NewL( RSocket& aSocket, MSIPExReaderNotifier& aNotifier );

        /**
        * NewLC
        * Static constructor. On return the instance is left to the CleanupStack.
        * @param aSocket The reference to the socket
        * @param aNotifier The reference to the reader notifier
        */
        static CSIPExSocketReader* NewLC( RSocket& aSocket, MSIPExReaderNotifier& aNotifier );

        /**
        * ~CSIPExSocketReader
        * Destructor
        */
        ~CSIPExSocketReader();

    public:

        /**
        * Read
        * Asynchronous function.
        * Starts the asynchronous reading.
        */
        void Read();

    protected: // from CActive

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

    private:
    
        /**
        * CSIPExSocketReader
        * Constructor
        * @param aSocket The reference to the socket
        * @param aNotifier The reference to the reader notifier
        */
        CSIPExSocketReader( RSocket& aSocket, MSIPExReaderNotifier& aNotifier );

        /**
        * ConstructL
        * Symbian 2nd phase constructor that might leave
        */
        void ConstructL();

    private:
        // Member variables

        // Ref: Reader notifier
        MSIPExReaderNotifier&   iNotifier;

        // Ref: Data socket
        RSocket& iSocket;

        // Buffers
        TBuf8<1>    iBuffer;
        TBuf8<256>   iMessage;
    };

#endif // __CSIPEXSOCKETREADER_H__

// end of file
