// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CDeflate.cpp
// Part of     : deflatecomp
// deflate compressor.
// Version     : 1.0
//



#include <e32base.h>
#include <badesca.h>

#include "Deflate.h"
#include "SigCompDeflateContext.h"
#include "MessageWriter.h"

/** deflate stream header. btyppe == 01, last block bit set */
static const TInt KDeflateStreamHeaderBits = ((1<<1) + 1);

/** number of bits in deflate stream header */
static const TInt KDeflateStreamHeaderBitsLength = 3;


/*
* Following table is used for length/offset encoding.
* as defined in RFC 1951 (DEFLATE Compressed Data Format
* Specification version 1.3), chapter 3.2.5.
*/
const TInt16 KLengthCode[] =
    {
    257, 258, 259, 260, 261, 262, 263, 264, 265, 265, 266, 266, 267, 267,
    268, 268, 269, 269, 269, 269, 270, 270, 270, 270, 271, 271, 271, 271,
    272, 272, 272, 272, 273, 273, 273, 273, 273, 273, 273, 273, 274, 274,
    274, 274, 274, 274, 274, 274, 275, 275, 275, 275, 275, 275, 275, 275,
    276, 276, 276, 276, 276, 276, 276, 276, 277, 277, 277, 277, 277, 277,
    277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 278, 278, 278, 278,
    278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 279, 279,
    279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279,
    280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
    280, 280, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
    281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
    281, 281, 281, 281, 281, 281, 282, 282, 282, 282, 282, 282, 282, 282,
    282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
    282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 283, 283, 283, 283,
    283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
    283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
    284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284,
    284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284,
    284, 284, 284, 285
    };

/*
* Following table is used for length/offset encoding.
* as defined in RFC 1951 (DEFLATE Compressed Data Format
* Specification version 1.3), chapter 3.2.5.
*/
const TUint8 KDistanceCode[] =
    {
    0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,
    8,  8, 8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15,  0,  0, 16, 17, 18, 18, 19, 19, 20, 20, 20, 20, 21, 21,
    21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29
    }; 

/*
* Following table is used for length/offset encoding.
* as defined in RFC 1951 (DEFLATE Compressed Data Format
* Specification version 1.3), chapter 3.2.5.
*/
const TUint8 KLengthExtraBits[] = 
    {
    0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0
    };

/*
* Following table is used for length/offset encoding.
* as defined in RFC 1951 (DEFLATE Compressed Data Format
* Specification version 1.3), chapter 3.2.5.
*/
const TUint8 KDistanceExtraBits[] = 
    {
    0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13
    };

/*
* Following table is used for length/offset encoding.
* as defined in RFC 1951 (DEFLATE Compressed Data Format
* Specification version 1.3), chapter 3.2.5.
*/
const TUint16 KLengthBases[] =
    {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56,
    64, 80, 96, 112, 128, 160, 192, 224, 0
    };

/*
* Following table is used for length/offset encoding.
* as defined in RFC 1951 (DEFLATE Compressed Data Format
* Specification version 1.3), chapter 3.2.5.
*/
const TUint16 KDistanceBases[] =
    {
    0,     1,     2,     3,     4,     6,     8,    12,    16,    24,
    32,    48,    64,    96,   128,   192,   256,   384,   512,   768,
    1024,  1536,  2048,  3072,  4096,  6144,  8192, 12288, 16384, 24576
    };

/**
* huffman tree table. Literal tree is used for encoding both literals
* and offset/length pairs.
* Tables comes from RFC 1951 (DEFLATE Compressed Data Format
* Specification version 1.3), chapter 3.2.5.
*/
const CDeflate::TTreeNode KLiteralTree[] = 
    {
    { 12,  8}, {140,  8}, { 76,  8}, {204,  8}, { 44,  8},
    {172,  8}, {108,  8}, {236,  8}, { 28,  8}, {156,  8},
    { 92,  8}, {220,  8}, { 60,  8}, {188,  8}, {124,  8},
    {252,  8}, {  2,  8}, {130,  8}, { 66,  8}, {194,  8},
    { 34,  8}, {162,  8}, { 98,  8}, {226,  8}, { 18,  8},
    {146,  8}, { 82,  8}, {210,  8}, { 50,  8}, {178,  8},
    {114,  8}, {242,  8}, { 10,  8}, {138,  8}, { 74,  8},
    {202,  8}, { 42,  8}, {170,  8}, {106,  8}, {234,  8},
    { 26,  8}, {154,  8}, { 90,  8}, {218,  8}, { 58,  8},
    {186,  8}, {122,  8}, {250,  8}, {  6,  8}, {134,  8},
    { 70,  8}, {198,  8}, { 38,  8}, {166,  8}, {102,  8},
    {230,  8}, { 22,  8}, {150,  8}, { 86,  8}, {214,  8},
    { 54,  8}, {182,  8}, {118,  8}, {246,  8}, { 14,  8},
    {142,  8}, { 78,  8}, {206,  8}, { 46,  8}, {174,  8},
    {110,  8}, {238,  8}, { 30,  8}, {158,  8}, { 94,  8},
    {222,  8}, { 62,  8}, {190,  8}, {126,  8}, {254,  8},
    {  1,  8}, {129,  8}, { 65,  8}, {193,  8}, { 33,  8},
    {161,  8}, { 97,  8}, {225,  8}, { 17,  8}, {145,  8},
    { 81,  8}, {209,  8}, { 49,  8}, {177,  8}, {113,  8},
    {241,  8}, {  9,  8}, {137,  8}, { 73,  8}, {201,  8},
    { 41,  8}, {169,  8}, {105,  8}, {233,  8}, { 25,  8},
    {153,  8}, { 89,  8}, {217,  8}, { 57,  8}, {185,  8},
    {121,  8}, {249,  8}, {  5,  8}, {133,  8}, { 69,  8},
    {197,  8}, { 37,  8}, {165,  8}, {101,  8}, {229,  8},
    { 21,  8}, {149,  8}, { 85,  8}, {213,  8}, { 53,  8},
    {181,  8}, {117,  8}, {245,  8}, { 13,  8}, {141,  8},
    { 77,  8}, {205,  8}, { 45,  8}, {173,  8}, {109,  8},
    {237,  8}, { 29,  8}, {157,  8}, { 93,  8}, {221,  8},
    { 61,  8}, {189,  8}, {125,  8}, {253,  8}, { 19,  9},
    {275,  9}, {147,  9}, {403,  9}, { 83,  9}, {339,  9},
    {211,  9}, {467,  9}, { 51,  9}, {307,  9}, {179,  9},
    {435,  9}, {115,  9}, {371,  9}, {243,  9}, {499,  9},
    { 11,  9}, {267,  9}, {139,  9}, {395,  9}, { 75,  9},
    {331,  9}, {203,  9}, {459,  9}, { 43,  9}, {299,  9},
    {171,  9}, {427,  9}, {107,  9}, {363,  9}, {235,  9},
    {491,  9}, { 27,  9}, {283,  9}, {155,  9}, {411,  9},
    { 91,  9}, {347,  9}, {219,  9}, {475,  9}, { 59,  9},
    {315,  9}, {187,  9}, {443,  9}, {123,  9}, {379,  9},
    {251,  9}, {507,  9}, {  7,  9}, {263,  9}, {135,  9},
    {391,  9}, { 71,  9}, {327,  9}, {199,  9}, {455,  9},
    { 39,  9}, {295,  9}, {167,  9}, {423,  9}, {103,  9},
    {359,  9}, {231,  9}, {487,  9}, { 23,  9}, {279,  9},
    {151,  9}, {407,  9}, { 87,  9}, {343,  9}, {215,  9},
    {471,  9}, { 55,  9}, {311,  9}, {183,  9}, {439,  9},
    {119,  9}, {375,  9}, {247,  9}, {503,  9}, { 15,  9},
    {271,  9}, {143,  9}, {399,  9}, { 79,  9}, {335,  9},
    {207,  9}, {463,  9}, { 47,  9}, {303,  9}, {175,  9},
    {431,  9}, {111,  9}, {367,  9}, {239,  9}, {495,  9},
    { 31,  9}, {287,  9}, {159,  9}, {415,  9}, { 95,  9},
    {351,  9}, {223,  9}, {479,  9}, { 63,  9}, {319,  9},
    {191,  9}, {447,  9}, {127,  9}, {383,  9}, {255,  9},
    {511,  9}, {  0,  7}, { 64,  7}, { 32,  7}, { 96,  7},
    { 16,  7}, { 80,  7}, { 48,  7}, {112,  7}, {  8,  7},
    { 72,  7}, { 40,  7}, {104,  7}, { 24,  7}, { 88,  7},
    { 56,  7}, {120,  7}, {  4,  7}, { 68,  7}, { 36,  7},
    {100,  7}, { 20,  7}, { 84,  7}, { 52,  7}, {116,  7},
    {  3,  8}, {131,  8}, { 67,  8}, {195,  8}, { 35,  8},
    {163,  8}, { 99,  8}, {227,  8}
    };

/**
* huffman tree table. Distance tree is used for encoding distance.
* Tables comes from RFC 1951 (DEFLATE Compressed Data Format
* Specification version 1.3), chapter 3.2.5.
*/
const CDeflate::TTreeNode KDistanceTree[] =
    {
    { 0, 5}, {16, 5}, { 8, 5}, {24, 5}, { 4, 5},
    {20, 5}, {12, 5}, {28, 5}, { 2, 5}, {18, 5},
    {10, 5}, {26, 5}, { 6, 5}, {22, 5}, {14, 5},
    {30, 5}, { 1, 5}, {17, 5}, { 9, 5}, {25, 5},
    { 5, 5}, {21, 5}, {13, 5}, {29, 5}, { 3, 5},
    {19, 5}, {11, 5}, {27, 5}, { 7, 5}, {23, 5}
    };

CDeflate* CDeflate::NewL(TBool aDynamic, CSigCompDeflateContext* aContext)
    {
    CDeflate* self= NewLC(aDynamic, aContext);
    CleanupStack::Pop();
    return self;
    }

CDeflate* CDeflate::NewLC(TBool aDynamic, CSigCompDeflateContext* aContext)
    {
    CDeflate* self= new (ELeave) CDeflate();
    CleanupStack::PushL(self);
    self->ConstructL(aDynamic, aContext);
    return self;
    }

CDeflate::~CDeflate()
    {
    delete iHashTable;
    delete iChainNodes;
    }

void CDeflate::CompressL(const TDesC8& aMessage,
                         CMessageWriter* aMessageWriter)
    {
    iMessageWriter = aMessageWriter;
    CArrayFixFlat<TUint8>* window = iDeflateContext->Window();

    SendBitsL(KDeflateStreamHeaderBits,
              KDeflateStreamHeaderBitsLength);

    const TUint8* msgPtr = aMessage.Ptr();
    TInt msgSize = aMessage.Length();
    TInt processed = 0;
    while (processed < msgSize)
        {
        processed += FillWindow(msgPtr + processed, msgSize - processed);

        while (iLookAhead)
            {
            TInt position;
            TInt length;
            TInt currentPosition;
            if (FindBestMatch(position, length))
                {
                //lazy matching:
                if ( length < KNiceLazyMatchLength )
                    {
                    currentPosition = iDeflateContext->CurrentPosition();
                    currentPosition++;
                    iDeflateContext->SetCurrentPosition(currentPosition);
                    TInt lazyPosition;
                    TInt lazyLength;
                    if (FindBestMatch(lazyPosition, lazyLength))
                        {
                        if (lazyLength > length)
                            {
                            InsertHash(iDeflateContext->CurrentPosition() - 1);
                            EncodeLiteralL(
                               window->At(iDeflateContext->CurrentPosition() - 1));
                            iLookAhead--;
                            length = lazyLength;
                            position = lazyPosition;
                            }
                        else
                            {
                            currentPosition = iDeflateContext->CurrentPosition();
                            currentPosition--;
                            iDeflateContext->SetCurrentPosition(currentPosition);
                            }
                        }
                    else
                        {
                        currentPosition = iDeflateContext->CurrentPosition();
                        currentPosition--;
                        iDeflateContext->SetCurrentPosition(currentPosition);
                        }
                    }
                EncodeDistanceLengthPairL(iDeflateContext->CurrentPosition() -
                                          position, length);
                iLookAhead -= length;
                while (length--)
                    {
                    InsertHash(iDeflateContext->CurrentPosition());
                    currentPosition = iDeflateContext->CurrentPosition();
                    currentPosition++;
                    iDeflateContext->SetCurrentPosition(currentPosition);
                    }
                }
            else
                {
                InsertHash(iDeflateContext->CurrentPosition());
                EncodeLiteralL(window->At(iDeflateContext->CurrentPosition()));
                currentPosition = iDeflateContext->CurrentPosition();
                currentPosition++;
                iDeflateContext->SetCurrentPosition(currentPosition);
                iLookAhead--;
                }
            }
        }

    EncodeLiteralL(256);
    FlushBitsL();
    }

void CDeflate::Reset(void)
    {
    iDeflateContext->SetCurrentPosition(0);
    iLookAhead = 0;
    ResetHashTable();
    iBitBuffer = 0;
    iBitsQueued = 0;
    }

void CDeflate::ResetHashTable(void)
    {

    TInt i;
    if (iHashTable)
        {
        for (i = 0;i < iHashSize;i++)
            {
            (*iHashTable)[i] = 0;
            }
        }

    if (iChainNodes)
        {
        for (i = 0;i < iDeflateContext->WindowSize();i++)
            {
            (*iChainNodes)[i] = 0;
            }
        }
    }

void CDeflate::SetDictionary(const TUint8* aPtr, TInt aSize)
    {
    if (aPtr)
        {
        ResetHashTable();
        Mem::Copy(&iDeflateContext->Window()->At(0), aPtr, aSize);

        for (TInt i = 0;i < aSize - 2;i++)
            {
            InsertHash(i);
            }
        iDeflateContext->SetCurrentPosition(aSize);
        }
    }

void CDeflate::ChangeWindowSizeL(TInt aWindowSize)
    {
    if (aWindowSize != iDeflateContext->WindowSize())
        {
        iDeflateContext->SetWindowSize(aWindowSize);

        CArrayFixFlat<TUint8>* window = new (ELeave)CArrayFixFlat<TUint8>(8);
        iDeflateContext->SetWindow(window);
        iDeflateContext->Window()->ResizeL(iDeflateContext->WindowSize() * 2);

        delete iChainNodes;
        iChainNodes = NULL;
        iChainNodes = new (ELeave)CArrayFixFlat<TInt16>(8);
        iChainNodes->ResizeL(iDeflateContext->WindowSize());
        iChainMask = iDeflateContext->WindowSize() - 1;

        ResetHashTable();
        iDeflateContext->SetCurrentPosition(0);
        iLookAhead = 0;
        }
    }

void CDeflate::SendBitsL(TUint aValue, TUint aLength)
    {
    iBitBuffer |= aValue << iBitsQueued;
    iBitsQueued += aLength;
    while (iBitsQueued >= 8)
        {
        PutByteL(static_cast<TUint8>(iBitBuffer & 0xff));
        iBitBuffer >>= 8;
        iBitsQueued -= 8;
        }
    }

void CDeflate::FlushBitsL()
    {
    if (iBitsQueued > 0)
        {
        PutByteL(static_cast<TUint8>(iBitBuffer));
        }
    iBitBuffer = 0;
    iBitsQueued = 0;
    } 

void CDeflate::PutByteL(TUint8 aByte)
    {
    iMessageWriter->WriteByteL(aByte);
    }

void CDeflate::EncodeLiteralL(TInt aCode)
    {
    SendBitsL(KLiteralTree[aCode].iBits, KLiteralTree[aCode].iLength);
    }

void CDeflate::EncodeDistanceLengthPairL(TUint aDist, TUint aLength)
    {
    aLength -= KMinMatch;
    aDist -= 1;

    TInt code = KLengthCode[aLength];
    EncodeLiteralL(code);
    TUint extra = KLengthExtraBits[code - (KMaxMatch - 1)];
    if (extra)
        {
        aLength -= KLengthBases[code - (KMaxMatch - 1)];
        SendBitsL(aLength, extra);
        }

    if (aDist < KEncodeDistance)
        {
        code = KDistanceCode[aDist];
        }
    else
        {
        code = KDistanceCode[(aDist>>7) + KEncodeDistance];
        }

    SendBitsL(KDistanceTree[code].iBits, KDistanceTree[code].iLength);

    extra = KDistanceExtraBits[code];
    if (extra)
        {
        aDist -= KDistanceBases[code];
        SendBitsL(aDist, extra);
        } 
    }

TBool CDeflate::FindBestMatch(TInt& aMatchPos, TInt& aLength)
    {
    aMatchPos = 0;
    if (iLookAhead > KMinMatch)
        {
        TInt position = iDeflateContext->CurrentPosition();
        TInt16 currentPos = (*iHashTable)[CalcHash(
                                 iDeflateContext->Window()->At(position),
                                 iDeflateContext->Window()->At(position + 1),
                                 iDeflateContext->Window()->At(position + 2))];
        TInt chainLength = KMaxChainLength;
        aLength = KMinMatch - 1;

        while (currentPos && chainLength > 0)
            {
            TInt len = 0;
						TUint8 *m = &iDeflateContext->Window()->At(position);
						TUint8 *c = &iDeflateContext->Window()->At(currentPos);
            while ((len < Min(KMaxMatch, iLookAhead)) &&
                  ((position + len) < (2 * iDeflateContext->WindowSize())) &&
                  (*m++ == *c++))
                {
                len++;
                }

            if ((len > aLength) &&
                ((position - currentPos) < iDeflateContext->WindowSize()))
                {
                aMatchPos = currentPos;
                aLength = len;
                if ( aLength >= KGoodMatchLength )
                    break;
                }

            if ( aLength >= KNiceMatchLength )
                chainLength >>= 2;

            currentPos = (*iChainNodes)[currentPos & iChainMask];
            chainLength--;
            }
        }
    return (aMatchPos > 0) ? ETrue : EFalse;
    }

TInt CDeflate::FillWindow(const TUint8* aPtr, TInt aBytes)
    {
    TInt position = iDeflateContext->CurrentPosition();
    TInt processed = Min(aBytes, 2 * iDeflateContext->WindowSize() -
                         (position + iLookAhead));

    if (position >= (2 * iDeflateContext->WindowSize() - KMaxMatch))
        {
        Mem::Copy(&iDeflateContext->Window()->At(0),
                &iDeflateContext->Window()->At(iDeflateContext->WindowSize()),
                 iDeflateContext->WindowSize());
        position -= iDeflateContext->WindowSize();
        iDeflateContext->SetCurrentPosition(position);

        TInt i;
        for (i=0; i<iHashSize; i++)
            {
            TInt16 t = (*iHashTable)[i];
            t = static_cast<TInt16>((t > iDeflateContext->WindowSize()) ?
                                    (t - iDeflateContext->WindowSize()) : 0);
            (*iHashTable)[i] = t;
            }

        for (i=0; i<iDeflateContext->WindowSize(); i++)
            {
            TInt16 t = (*iChainNodes)[i];
            t = static_cast<TInt16>((t > iDeflateContext->WindowSize()) ?
                                    (t - iDeflateContext->WindowSize()) : 0);
            (*iChainNodes)[i] = t;
            }

        processed = Min(processed + iDeflateContext->WindowSize(), aBytes);
        }

    Mem::Copy(&iDeflateContext->Window()->At(position + iLookAhead),
              aPtr, processed);
    iLookAhead += processed;
    return processed;
    }

void CDeflate::InsertHash(TInt aPosition)
    {
    if ((aPosition + 2) < 2 * (iDeflateContext->WindowSize()))
        {
        TUint hash = CalcHash(iDeflateContext->Window()->At(aPosition),
                              iDeflateContext->Window()->At(aPosition + 1),
                              iDeflateContext->Window()->At(aPosition + 2));
        (*iChainNodes)[aPosition & iChainMask] = (*iHashTable)[hash];
        (*iHashTable)[hash] = static_cast<TInt16>(aPosition);
        }
    }

TInt CDeflate::CalcHash(TUint8 aA, TUint8 aB, TUint8 aC) const
    {
    return (((aA << (iHashShift2)) ^ (aB << iHashShift) ^ aC) & iHashMask);
    }

void CDeflate::ConstructL(TBool aDynamic, CSigCompDeflateContext* aContext)
    {
    iDynamic = aDynamic;
    iDeflateContext = aContext;

    iHashSize = 1 << KHashTableBits;
    iHashMask = iHashSize - 1;
    iHashShift = ((KHashTableBits + KMinMatch - 1) / KMinMatch);
    iHashShift2 = iHashShift << 1;
    iHashTable = new (ELeave)CArrayFixFlat<TInt16>(8);
    iHashTable->ResizeL(iHashSize);

    if (iDeflateContext->Window() != NULL)
        {
        iChainNodes = new (ELeave)CArrayFixFlat<TInt16>(8);
        iChainNodes->ResizeL(iDeflateContext->WindowSize());
        iChainMask = iDeflateContext->WindowSize() - 1;

        ResetHashTable();
        iLookAhead = 0;

        if (iDynamic)
            {
            TInt position = aContext->CurrentPosition();

            if (position > 2)
                {
                for (TInt i = 0;i < position - 2;i++)
                    {
                    InsertHash(i);
                    }
                }
            }
        else
            {
            iDeflateContext->SetCurrentPosition(0);
            }
        }
    else
        {
        iDeflateContext->SetWindowSize(0);
        }
    }

CDeflate::CDeflate()
    {
    }
