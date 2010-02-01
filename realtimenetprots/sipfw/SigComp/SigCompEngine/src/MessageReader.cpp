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
// Name        : MessageReader.cpp
// Part of     : SigComp / dispatcher
// SigComp message reader
// Version     : 1.0
//




// INCLUDE FILES
#include "MessageReader.h"
#include "Sigcomp.h"


// ============================ MEMBER FUNCTIONS ==============================

CMessageReader::CMessageReader(const TDesC8& aMessage,
                               TBool aStreamBasedProtocol) :
    iMessage(aMessage),
    iStreamBasedProtocol(aStreamBasedProtocol)
    {
    }

TInt CMessageReader::ReadByte(TUint& aByte)
    {

    if (iStreamBasedProtocol)
        {
        if (iQuoteBytes > 0)
            {
            if (iPos < iMessage.Length())
                {
                aByte = iMessage[iPos];
                iPos++;
                iQuoteBytes--;
                return KErrNone;
                }
            else
                {
                return KErrTooBig;
                }
            }
        else
            {
            if (iPos < iMessage.Length())
                {
                aByte = iMessage[iPos];
                iPos++;

                if (aByte == 0xff)
                    {
                    if (iPos < iMessage.Length())
                        {
                        aByte = iMessage[iPos];
                        iPos++;

                        if (aByte <= 0x7f)
                            {
                            iQuoteBytes = aByte;
                            aByte = 0xff;
                            return KErrNone;
                            }
                        else if (aByte == 0xff)
                            {
                            return KErrEof;
                            }
                        else    // aByte: 0x80-0xfe
                            {
                            return CSigComp::EDecompressionFailure;
                            }
                        }
                    else
                        {
                        return KErrTooBig;
                        }
                    }
                else
                    {
                    return KErrNone;
                    }
                }
            else
                {
                return KErrTooBig;
                }
            }
        }
    else
        {
        if (iPos < iMessage.Length())
            {
            aByte = iMessage[iPos];
            iPos++;
            return KErrNone;
            }
        else
            {
            return KErrTooBig;
            }
        }
    }

TInt CMessageReader::ReadBlock(TDes8& aBlock)
    {
    return ReadBlock(aBlock, aBlock.MaxLength());
    }

TInt CMessageReader::ReadBlock(TDes8& aBlock, TInt aLength)
    {

    if (iStreamBasedProtocol)
        {
        aBlock.SetLength(0);

        TInt i;
        TUint byte;
        for (i = 0; i < aLength; i++)
            {
            TInt retval = ReadByte(byte);
            if (retval == KErrNone)
                {
                aBlock.Append(byte);
                }
            else
                {
                return retval;
                }
            }

        return KErrNone;
        }
    else
        {

        TInt len = aLength;

        if ((iPos + len) > iMessage.Length())
            {
            len = iMessage.Length() - iPos;
            }

        aBlock.Copy(iMessage.Mid(iPos, len));
        iPos += len;

        if (len == aLength)
            {
            return KErrNone;
            }
        else
            {
            return KErrTooBig;
            }
        }
    }

TInt CMessageReader::Avail(TInt aLength)
    {

    if (iStreamBasedProtocol)
        {
        TInt quote = iQuoteBytes;
        TInt pos = iPos;

        TInt i;
        TUint byte;

        for (i = 0; i < aLength; i++)
            {
            TInt retval = ReadByte(byte);
            if (retval != KErrNone)
                {
                iQuoteBytes = quote;
                iPos = pos;
                return retval;
                }
            }

        iQuoteBytes = quote;
        iPos = pos;

        return KErrNone;
        }
    else
        {
        if ((iPos + aLength) <= iMessage.Length())
            {
            return KErrNone;
            }
        else
            {
            return KErrTooBig;
            }
        }
    }

TInt CMessageReader::SkipDelimiters(TUint aDelToSkip)
    {
    if (iStreamBasedProtocol)
        {
        TUint i;
        for (i = 0; i < aDelToSkip; i++)
            {
            if (iPos < iMessage.Length())
                {
                if (iMessage[iPos] == 0xff)
                    {
                    if ((iPos+1) < iMessage.Length())
                        {
                        if (iMessage[iPos+1] == 0xff)
                            {
                            iPos += 2;
                            }
                        else
                            {
                            return KErrNone;
                            }
                        }
                    else
                        {
                        return KErrTooBig;
                        }
                    }
                else
                    {
                    return KErrNone;
                    }
                }
            else
                {
                return KErrNone;
                }
            }
        return KErrNone;
        }
    else
        {
        return KErrNone;
        }
    }

TInt CMessageReader::Pos()
    {
    return iPos;
    }
