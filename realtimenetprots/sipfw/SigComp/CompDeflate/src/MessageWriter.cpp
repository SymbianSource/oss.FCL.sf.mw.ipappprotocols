// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : MessageWriter.cpp
// Part of     : compressor
// Interface   : 
// SigComp message writer
// Version     : 1.0
//




#include <e32base.h>
#include <badesca.h>

#include "MessageWriter.h"

/** Message delimeter, appended whe streaming protocol is enabled */
static const TUint16 KMessageDelimeter = 0xffff;

CMessageWriter::CMessageWriter(CBufBase* aBuffer, TBool aStreamBasedProtocol)
                :iBuffer(aBuffer), iStreamBasedProtocol(aStreamBasedProtocol),
                iCount(0)
    {
    }

void CMessageWriter::WriteByteL(TInt aByte)
    {
    TUint8 tmp;

    if(iStreamBasedProtocol)
        {
        if(aByte == 0xFF)
            {
            iCount++;
            }
        else
            {
            if(iCount)
                {
                iCount--;

                tmp = 0xFF;
                iBuffer->InsertL(iBuffer->Size(), &tmp, 1);
                tmp = static_cast<TUint8>(iCount);
                iBuffer->InsertL(iBuffer->Size(), &tmp, 1);

                for(TInt i=0; i<(iCount); i++)
                    {
                    tmp = 0xFF;
                    iBuffer->InsertL(iBuffer->Size(), &tmp, 1);
                    }

                iCount = 0;
                tmp = static_cast<TUint8>(aByte);
                iBuffer->InsertL(iBuffer->Size(), &tmp, 1);
                }
            else
                {
                tmp = static_cast<TUint8>(aByte);
                iBuffer->InsertL(iBuffer->Size(), &tmp, 1);
                }
            }
        }
    else
        {
        tmp = static_cast<TUint8>(aByte);
        iBuffer->InsertL(iBuffer->Size(), &tmp, 1);
        }
    }

void CMessageWriter::WriteBlockL(const TDesC8& aBlock)
    {
    for(TInt i=0; i<aBlock.Length(); i++)
        {
        WriteByteL(aBlock[i]);
        }
    }

void CMessageWriter::FlushAndTerminateL()
    {
    if(iStreamBasedProtocol)
        {
        if(iCount)
            {
            TUint8 tmp;

// this code is not covered, because there is no way for deflate compressor
// to output FF at the end of compressed message, the EOS character encoding
// flushes all FFs in WriteByteL()
// I left the code there, as it may be usefull for other compressors.

            iCount--;

            tmp = 0xFF;
            iBuffer->InsertL(iBuffer->Size(), &tmp, 1);
            tmp = static_cast<TUint8>(iCount);
            iBuffer->InsertL(iBuffer->Size(), &tmp, 1);

            for(TInt i=0; i<(iCount); i++)
                {
                tmp = 0xFF;
                iBuffer->InsertL(iBuffer->Size(), &tmp, 1);
                }
            iCount = 0;
            }
        iBuffer->InsertL(iBuffer->Size(), &KMessageDelimeter, 2);
        }
    }
