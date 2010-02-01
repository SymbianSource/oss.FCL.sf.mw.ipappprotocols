/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : MessageReader.h
* Part of     : SigComp / dispatcher
* Interface   : 
* SigComp message reader
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 * @brief Class for reading from sigcomp message.
 * 
 * @class CMessageReader messagereader.h "messagereader.h"
 *
 * This class is used for reading from sigcomp message with optional 
 * record marker decoding as specified in RFC 3320, chapter 4.2.2
 */

class CMessageReader : public CBase
    {
    public:

        /**
         * Constructs object.
         *
         * @param aMessage message to be read
         * @param aStreamBasedProtocol if ETrue, a message will be record
         *                             marker decoded.
         */

        CMessageReader(const TDesC8& aMessage, TBool aStreamBasedProtocol);

        /**
         * Read one byte from message.
         *
         * @returns If the reading is successful, the value of readed
         *          byte (0-255).
         *          If the reading fails, a negative value indicating 
         *          the cause of failure.
         *          KErrEof if delimiter is reached, or KErrTooBig if 
         *          end of buffer.
         */

        TInt ReadByte(TUint& aByte);

        /**
         * Read block of bytes from message.
         *
         * @param aBlock contains data read from the buffer; on exit, 
         *               its Length() specifies the length of readed
         *               (successful or failed) data.
         * @param aLentgh specifies the amount of data to be read.
         *
         * @returns If the reading is successful, a KErrNone.
         *          If the reading fails, a negative value indicating
         *          the cause of failure.
         *          KErrEof if delimiter is reached, or KErrTooBig if
         *          end of buffer.
         */

        TInt ReadBlock(TDes8& aBlock, TInt aLength);

        /**
         * Read block of bytes from message.
         *
         * @param aBlock contains data read from the buffer; on call, 
         *               its MaxLength()
         *               specifies the amount of data to be read; on exit,
         *               its Length() specifies the length of readed 
         *               (successful or failed) data.
         *
         * @returns If the reading is successful, a KErrNone.
         *          If the reading fails, a negative value indicating the 
         *          cause of failure.
         *          KErrEof if delimiter is reached, or KErrTooBig if end 
         *          of buffer.
         */

        TInt ReadBlock(TDes8& aBlock);

        /**
         * Check if specified number of bytes is available in buffer.
         *
         * @param aLength number of bytes to check
         *
         * @returns If the specified number of bytes is available, a KErrNone.
         *          If not, a negative value indicating the cause of failure.
         *          KErrEof if delimiter is reached, or KErrTooBig if end of 
         *          buffer.
         */

        TInt Avail(TInt aLength);

        /**
         * Get current position in message buffer.
         *
         * @returns position in message buffer.
         */

        TInt Pos();

        /**
         * If stream-based protocol, skip message delimiters.
         *
         * @param aDelToSkip number of delimiters to skip, default maximumm
         *                   possible.
         *
         * @returns KErrNone if seccesfful, error code if failure.
         */

        TInt SkipDelimiters(TUint aDelToSkip=KMaxTUint32);

    private:    // Data

        TPtrC8 iMessage;
        TBool iStreamBasedProtocol;
        TInt iPos;
        TInt iQuoteBytes;

    };

#endif
            
// End of File
