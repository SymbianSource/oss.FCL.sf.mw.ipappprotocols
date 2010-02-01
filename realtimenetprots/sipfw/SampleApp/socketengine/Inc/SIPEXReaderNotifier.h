
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



#ifndef __MSIPEXREADERNOTIFIER_H
#define __MSIPEXREADERNOTIFIER_H

// CLASS DECLARATIONS
/**
* The socket reader notifier inteface.
* Notifies about events in reader.
*/
class MSIPExReaderNotifier
	{
	public:

        /**
        * Called when the message has read from the socket.
        * @param aBuffer Contains the readed buffer
        */
        virtual void MessageReceived( TDesC8& aBuffer ) = 0;
        
        /**
        * Called if error occurs in reading from the socket.
        * @param aError The error code
        */        
        virtual void ErrorInReading( TInt aError ) = 0;
	};

#endif // __MSIPEXREADERNOTIFIER_H

// End of file
