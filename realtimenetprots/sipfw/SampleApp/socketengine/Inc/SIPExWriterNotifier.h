
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



#ifndef __MSIPEXWRITERNOTIFIER_H
#define __MSIPEXWRITERNOTIFIER_H

// CLASS DECLARATIONS
/**
* Interface for notifying about socket writer events.
*/
class MSIPExWriterNotifier
	{
	public:
		
        /**
        * Called when the message has been written to the socket.
        */
        virtual void WriteDone() = 0;

        /**
        * Called when the error has occured in writing.
        */
        virtual void ErrorInWriting( TInt aError ) = 0;
	};
#endif // __MSIPEXWRITERNOTIFIER_H

// End of file
