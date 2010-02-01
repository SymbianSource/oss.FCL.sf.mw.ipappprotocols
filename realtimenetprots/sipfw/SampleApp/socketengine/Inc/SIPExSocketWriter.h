
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



#ifndef __CSIPEXSOCKETWRITER_H__
#define __CSIPEXSOCKETWRITER_H__

#include <e32base.h>

// FORWARD DECLARATIONS
class MSIPExWriterNotifier;
class RSocket;

/**
* CSIPExSocketWriter
* Writes data to the connected socket.
*/
class CSIPExSocketWriter : public CActive
    {
    public:

        /**
        * NewL
        * Static constructor
        * @param aSocket The reference to the socket
        * @param aNotifier The reference to the writer notifier
        */
        static CSIPExSocketWriter* NewL( RSocket& aSocket, MSIPExWriterNotifier& aNotifier );

        /**
        * NewLC
        * Static constructor. On return the instance is left to the CleanupStack.
        * @param aSocket The reference to the socket
        * @param aNotifier The reference to the writer notifier
        */
        static CSIPExSocketWriter* NewLC( RSocket& aSocket, MSIPExWriterNotifier& aNotifier );

        /**
        * ~CSIPExSocketWriter
        * Destructor
        */
        ~CSIPExSocketWriter();

    public:
 
        /**
        * Write
        * Asynchronous function.
        * Starts the asynchronous writing.
        */
        void Write( const TDesC8& aBuffer );

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
        * CSIPExSocketWriter
        * Constructor
        * @param aSocket The reference to the socket
        * @param aNotifier The reference to the writer notifier
        */
        CSIPExSocketWriter( RSocket& aSocket, MSIPExWriterNotifier& aNotifier );

        /**
        * ConstructL
        * Symbian 2nd phase constructor that might leave
        */
        void ConstructL();

    private:
        // Member variables
        MSIPExWriterNotifier&   iNotifier;

        // Ref: Data socket
        RSocket&    iSocket;

        // The write buffer
        HBufC8*     iBuffer;
    };

#endif // __CSOCKETWRITER_H__

// End of file
