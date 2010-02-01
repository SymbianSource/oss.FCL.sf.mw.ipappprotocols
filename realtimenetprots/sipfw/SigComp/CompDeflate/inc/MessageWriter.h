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
* Name        : MessageWriter.h
* Part of     : compressor
* Interface   : 
* SigComp message writer
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef MESSAGEWRITER_H
#define MESSAGEWRITER_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 * @brief Class for writing to sigcomp message.
 * 
 * @class CMessageWriter messagewriter.h "messagewriter.h"
 *
 * This class is used for writing to sigcomp message with 
 * optional record marker encoding as
 * specified in RFC 3320, chapter 4.2.2
 */

class CMessageWriter : public CBase
    {
    public:

        /**
         * Constructs object.
         *
         * @param aBuffer buffer to write into.
         * @param aStreamBasedProtocol if ETrue, a message will be 
         *                             record marker encoded
         */

        CMessageWriter(CBufBase* aBuffer, TBool aStreamBasedProtocol);

        /**
         * Write one byte to buffer.
         *
         * @param aByte byte to write
         */

        void WriteByteL(TInt aByte);

        /**
         * Write block of bytes to buffer.
         *
         * @param aBlock block of bytes to write; Length() 
         *               specifies number of bytes to write
         */

        void WriteBlockL(const TDesC8& aBlock);

        /**
         * Flush cached data to buffer and optionally generate 
         * stream delimiter.
         */

        void FlushAndTerminateL();

    private:    // Data

        CBufBase* iBuffer;
        TBool iStreamBasedProtocol;
        TInt iCount;
    };

#endif
            
// End of File
