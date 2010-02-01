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
// Name        : Udvm.cpp
// Part of     : SigComp / UDVM
// UDVM core
// Version     : 1.0
//




// INCLUDE FILES
#include "Udvm.h"
#include "UdvmMemory.h"
#include "sigcompcompartment.h"
#include "StateMgr.h"
#include "Crc.h"
#include "MessageReader.h"

// ============================ MEMBER FUNCTIONS ==============================

void CUdvm::ConstructL(CStateMgr* aStateMgr,
                       TUint aMemSize,
                       TUint aCyclesPerBit)
    {
    iExecuteOpcode[EOpcode_DECOMPRESSION_FAILURE] =
                   &CUdvm::ExecuteOpcodeDecompressionFailureL;
    iExecuteOpcode[EOpcode_AND] = &CUdvm::ExecuteOpcodeAndL;
    iExecuteOpcode[EOpcode_OR] = &CUdvm::ExecuteOpcodeOrL;
    iExecuteOpcode[EOpcode_NOT] = &CUdvm::ExecuteOpcodeNotL;
    iExecuteOpcode[EOpcode_LSHIFT] = &CUdvm::ExecuteOpcodeLShiftL;
    iExecuteOpcode[EOpcode_RSHIFT] = &CUdvm::ExecuteOpcodeRShiftL;
    iExecuteOpcode[EOpcode_ADD] = &CUdvm::ExecuteOpcodeAddL;
    iExecuteOpcode[EOpcode_SUBTRACT] = &CUdvm::ExecuteOpcodeSubtractL;
    iExecuteOpcode[EOpcode_MULTIPLY] = &CUdvm::ExecuteOpcodeMultiplyL;
    iExecuteOpcode[EOpcode_DIVIDE] = &CUdvm::ExecuteOpcodeDivideL;
    iExecuteOpcode[EOpcode_REMAINDER] = &CUdvm::ExecuteOpcodeRemainderL;
    iExecuteOpcode[EOpcode_SORT_ASCENDING] = &CUdvm::ExecuteOpcodeAscendingL;
    iExecuteOpcode[EOpcode_SORT_DESCENDING] = &CUdvm::ExecuteOpcodeDescendingL;
    iExecuteOpcode[EOpcode_SHA_1] = &CUdvm::ExecuteOpcodeSha1L;
    iExecuteOpcode[EOpcode_LOAD] = &CUdvm::ExecuteOpcodeLoadL;
    iExecuteOpcode[EOpcode_MULTILOAD] = &CUdvm::ExecuteOpcodeMultiloadL;
    iExecuteOpcode[EOpcode_PUSH] = &CUdvm::ExecuteOpcodePushL;
    iExecuteOpcode[EOpcode_POP] = &CUdvm::ExecuteOpcodePopL;
    iExecuteOpcode[EOpcode_COPY] = &CUdvm::ExecuteOpcodeCopyL;
    iExecuteOpcode[EOpcode_COPY_LITERAL] = &CUdvm::ExecuteOpcodeCopyLiteralL;
    iExecuteOpcode[EOpcode_COPY_OFFSET] = &CUdvm::ExecuteOpcodeCopyOffsetL;
    iExecuteOpcode[EOpcode_MEMSET] = &CUdvm::ExecuteOpcodeMemsetL;
    iExecuteOpcode[EOpcode_JUMP] = &CUdvm::ExecuteOpcodeJumpL;
    iExecuteOpcode[EOpcode_COMPARE] = &CUdvm::ExecuteOpcodeCompareL;
    iExecuteOpcode[EOpcode_CALL] = &CUdvm::ExecuteOpcodeCallL;
    iExecuteOpcode[EOpcode_RETURN] = &CUdvm::ExecuteOpcodeReturnL;
    iExecuteOpcode[EOpcode_SWITCH] = &CUdvm::ExecuteOpcodeSwitchL;
    iExecuteOpcode[EOpcode_CRC] = &CUdvm::ExecuteOpcodeCrcL;
    iExecuteOpcode[EOpcode_INPUT_BYTES] = &CUdvm::ExecuteOpcodeInputBytesL;
    iExecuteOpcode[EOpcode_INPUT_BITS] = &CUdvm::ExecuteOpcodeInputBitsL;
    iExecuteOpcode[EOpcode_INPUT_HUFFMAN] = &CUdvm::ExecuteOpcodeInputHuffmanL;
    iExecuteOpcode[EOpcode_STATE_ACCESS] = &CUdvm::ExecuteOpcodeStateAccessL;
    iExecuteOpcode[EOpcode_STATE_CREATE] = &CUdvm::ExecuteOpcodeStateCreateL;
    iExecuteOpcode[EOpcode_STATE_FREE] = &CUdvm::ExecuteOpcodeStateFreeL;
    iExecuteOpcode[EOpcode_OUTPUT] = &CUdvm::ExecuteOpcodeOutputL;
    iExecuteOpcode[EOpcode_END_MESSAGE] = &CUdvm::ExecuteOpcodeEndMessageL;

    iStateMgr = aStateMgr;
    iUdvmMemory = CUdvmMemory::NewL(aMemSize);

    iCyclesPerBit = aCyclesPerBit;

    iSHA1 = CSHA1::NewL();
    }

CUdvm::CUdvm() : iReturnedFeedback(NULL, 0),
                 iRequestedFeedback(NULL, 0),
                 iReturnedParameters(NULL, 0)
    {
    }

CUdvm* CUdvm::NewLC(CStateMgr* aStateMgr,
                    TUint aMemSize,
                    TUint aCyclesPerBit)
    {
    CUdvm* self= new (ELeave) CUdvm();
    CleanupStack::PushL(self);
    self->ConstructL(aStateMgr, aMemSize, aCyclesPerBit);
    return self;
    }

CUdvm* CUdvm::NewL(CStateMgr* aStateMgr, TUint aMemSize, TUint aCyclesPerBit)
    {
    CUdvm* self= NewLC(aStateMgr, aMemSize, aCyclesPerBit);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CUdvm::~CUdvm()
    {

    DenyStateOperations();

    delete iUdvmMemory;
    delete iSHA1;
    }


// ---------------------------------------------------------------------------

/*
     0   1   2   3   4   5   6   7       0   1   2   3   4   5   6   7
   +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
   | 1   1   1   1   1 | T |  len  |   | 1   1   1   1   1 | T |   0   |
   +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
   |                               |   |                               |
   :    returned feedback item     :   :    returned feedback item     :
   |                               |   |                               |
   +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
   |                               |   |           code_len            |
   :   partial state identifier    :   +---+---+---+---+---+---+---+---+
   |                               |   |   code_len    |  destination  |
   +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
   |                               |   |                               |
   :   remaining SigComp message   :   :    uploaded UDVM bytecode     :
   |                               |   |                               |
   +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
                                       |                               |
                                       :   remaining SigComp message   :
                                       |                               |
                                       +---+---+---+---+---+---+---+---+

     0   1   2   3   4   5   6   7       0   1   2   3   4   5   6   7
   +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
   | 0 |  returned_feedback_field  |   | 1 | returned_feedback_length  |
   +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
                                       |                               |
                                       :    returned_feedback_field    :
                                       |                               |
                                       +---+---+---+---+---+---+---+---+

   Encoding:   Length of partial state identifier

   01          6 bytes
   10          9 bytes
   11          12 bytes

   Encoding:   Destination address:

   0000        reserved
   0001        2  *  64  =  128
   0010        3  *  64  =  196
   0011        4  *  64  =  256
     :                :
   1111        16 *  64  =  1024

   Note that the encoding 0000 is reserved for future SigComp versions,
   and causes a decompression failure in Version 0x01.
*/

void CUdvm::SetReturnedFeedbackL(TBool aT, CMessageReader* aMsgReader)
    {

    if (aT)
        {
        // returned feedback item

        TUint rff;
        if (aMsgReader->ReadByte(rff) != KErrNone)
            {
            User::Leave(CSigComp::EIncompleteMessage);
            }

        if (rff & KReturnedFeedbackFieldMask)
            {
            // long form

            // use decompression memory after udvm memory to store returned
            // feedback
            TUint8* udvmMem = iUdvmMemory->MemoryPtr();
            TUint udvmFreeMemSize = iUdvmMemory->FreeMemorySize();
            iReturnedFeedback.Set(&udvmMem[udvmFreeMemSize],
                                  0, rff & KReturnedFeedbackLengthMask);
            if (aMsgReader->ReadBlock(iReturnedFeedback) != KErrNone)
                {
                User::Leave(CSigComp::EIncompleteMessage);
                }
            }
        else
            {
            // short form

            // use decompression memory after udvm memory to store returned
            // feedback
            TUint8* udvmMem = iUdvmMemory->MemoryPtr();
            TUint udvmFreeMemSize = iUdvmMemory->FreeMemorySize();
            iReturnedFeedback.Set(&udvmMem[udvmFreeMemSize], 0, 1);
            iReturnedFeedback.Append(rff);
            }
        }
    else
        {
        iReturnedFeedback.Set(NULL, 0, 0);
        }
    }

void CUdvm::UploadStateInMessageL(TUint aLen, CMessageReader* aMsgReader)
    {

    // RFC3320:
    // The len field encodes the number of transmitted bytes as follows:
    //
    //   Encoding:   Length of partial state identifier
    //   01          6 bytes
    //   10          9 bytes
    //   11          12 bytes
    TUint id_len = (aLen + 1) * 3;

    TBuf8<12> psi;
    if (aMsgReader->ReadBlock(psi, id_len) != KErrNone)
        {
        User::Leave(CSigComp::EIncompleteMessage);
        }

    // upload state item
    UploadStateL(id_len, psi.Ptr());
    }

void CUdvm::UploadBytecodeInMessageL(CMessageReader* aMsgReader)
    {

    TUint tmp;

    // get first byte of 16-bit big-endian value
    if (aMsgReader->ReadByte(tmp) != KErrNone)
        {
        User::Leave(CSigComp::EIncompleteMessage);
        }

    // code length is most significant 12 bits
    TUint code_len = tmp << 4;

    // get second byte of 16-bit big-endian value
    if (aMsgReader->ReadByte(tmp) != KErrNone)
        {
        User::Leave(CSigComp::EIncompleteMessage);
        }

    code_len |= tmp >> 4;

    // destination is less significant 4 bits
    TUint destination = tmp & 0x000f;

    // The destination field is encoded as follows:
    //
    //                     Encoding:   Destination address:
    //                     0000        reserved
    //                     0001        2  *  64  =  128
    //                     0010        3  *  64  =  196
    //                     0011        4  *  64  =  256
    //                       :                :
    //                     1111        16 *  64  =  1024
    //
    //   Note that the encoding 0000 is reserved for future SigComp versions,
    //   and causes a decompression failure in Version 0x01.

    if (destination == 0)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    // destination decoding - see above
    destination = (destination + 1) * 64;

    // upload code_len bytes of bytecode at destination
    UploadCodeL(destination, code_len, aMsgReader);
    }

// ----------------------------------------------------------------------------
// CUdvm::DecompressL
// dispatch message and run bytecode
// ----------------------------------------------------------------------------
//

CBufBase* CUdvm::DecompressL(const TDesC8& aMessage, TUint& aBytesConsumed,
                             TBool aStreamBasedProtocol)
    {

    CMessageReader* msgReader = new (ELeave)CMessageReader(aMessage,
                                                         aStreamBasedProtocol);
    CleanupStack::PushL(msgReader);

    if (msgReader->SkipDelimiters() != KErrNone)
        {
        User::Leave(CSigComp::EIncompleteMessage);
        }

    TUint tmp;
    if (msgReader->ReadByte(tmp) != KErrNone)
        {
        User::Leave(CSigComp::EIncompleteMessage);
        }

    //is sigcomp message?
    if ((tmp & KSigCompHeaderMask) != KSigCompHeaderMask)
        {
        User::Leave(CSigComp::EIncompleteMessage);
        }

    // get T bit from header
    TBool t = tmp & KSigCompHeaderTMask;
    // partial state identifier length
    TUint len = tmp & KSigCompHeaderLenMask;

    // initialize udvm memory
    if (aStreamBasedProtocol)
        {
        iUdvmMemory->InitMemoryL(iUdvmMemory->MemorySize()/2, iCyclesPerBit);
        }
    else
        {
        iUdvmMemory->InitMemoryL(aMessage.Length(), iCyclesPerBit);
        }
    // T bit set, returned feedback present
    SetReturnedFeedbackL(t, msgReader);

    if (len)
        {
        // partial state identifier
        UploadStateInMessageL(len, msgReader);
        }
    else
        {
        // bytecode
        UploadBytecodeInMessageL(msgReader);
        }

    // available cycles, as defined in RFC3220 chapter 8.6
    iAvailableCycles = (8 * aMessage.Length() + 1000) * iCyclesPerBit;

    // prepare output buffer
    CBufFlat* outputBuffer = CBufFlat::NewL(128);
    CleanupStack::PushL(outputBuffer);
    outputBuffer->SetReserveL(128);
    iOutputBuffer = outputBuffer; // used for parameter passing to Opcodes

    // execute code
    ExecuteCodeL(msgReader);

    iOutputBuffer = NULL;
    CleanupStack::Pop(outputBuffer);

    // skip end delimiter
    if (aStreamBasedProtocol)
        {
        msgReader->SkipDelimiters(1);
        }

    // get message bytes consumed
    aBytesConsumed = msgReader->Pos();

    outputBuffer->Compress();

    CleanupStack::PopAndDestroy(msgReader);

    return outputBuffer;
    }

// ----------------------------------------------------------------------------
// CUdvm::ByteCopyingFragmentL
//
// ----------------------------------------------------------------------------
//

inline TUint CUdvm::ByteCopyingFragmentL(TUint& aAddress,
                                         TUint& aLength,
                                         TUint aBcl,
                                         TUint aBcr) const
    {

    TUint len;

    if (((aAddress + 1) <= aBcr) && ((aAddress + aLength) > aBcr))
        {
        len = aBcr - aAddress;
        }
    else
        {
        len = aLength;
        }

    if ((aAddress + len) > iUdvmMemory->FreeMemorySize())
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    aLength -= len;
    aAddress = aBcl;

    return len;
    }

// ----------------------------------------------------------------------------
// CUdvm::UploadCodeL
// upload bytecode, initialize UDVM memory and registers
// ----------------------------------------------------------------------------
//

void CUdvm::UploadCodeL(TUint aDest, TUint aLen, CMessageReader* aMsgReader)
    {

    // init memory
    iUdvmMemory->WriteMem16L(EMem_partial_state_ID_length, 0x0000);
    iUdvmMemory->WriteMem16L(EMem_state_length, 0x0000);

    // upload code
    iUdvmMemory->CheckMemAccessL(aDest & KMaxUdvmMemoryMask, aLen);
    TPtr8 codePtr(iUdvmMemory->MemoryPtr() +
                  (aDest & KMaxUdvmMemoryMask), aLen);
    if (aMsgReader->ReadBlock(codePtr) != KErrNone)
        {
        User::Leave(CSigComp::EIncompleteMessage);
                }

    iCodeStart = aDest;
    }


// ----------------------------------------------------------------------------
// CUdvm::UploadStateL
// upload state item, initialize UDVM memory and registers
// ----------------------------------------------------------------------------
//

void CUdvm::UploadStateL(TUint aStateLen, const TUint8* aState)
    {

    TStateItem* si = iStateMgr->FindStateItem(TPtrC8(aState, aStateLen));

    if (si == NULL)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    TestStateAccessLenL(si, aStateLen);

    // init memory
    iUdvmMemory->WriteMem16L(EMem_partial_state_ID_length, aStateLen);
    iUdvmMemory->WriteMem16L(EMem_state_length, si->iStateLength);

    // upload state value
    iUdvmMemory->CopyToMemL(si->iStateAddress & KMaxUdvmMemoryMask,
                            si->iStateValue,
                            si->iStateLength);

    iCodeStart = si->iStateInstruction;
    }


// ----------------------------------------------------------------------------
// CUdvm::TestStateAccessLenL
// test state access lenght
// ----------------------------------------------------------------------------
//

void CUdvm::TestStateAccessLenL(const TStateItem* aStateItem,
                                TUint aAccessLen) const
    {

    if (aAccessLen < aStateItem->iMinimumAccessLength)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }
    }


// ----------------------------------------------------------------------------
// CUdvm::InputMessageBitsL
// input message bits
// ----------------------------------------------------------------------------
//

TUint CUdvm::InputMessageBitsL(TUint aSize, TUint aP, TUint aF)
    {
    // aP and aF are flags from input_bit_order UDVM register

    // iLastUsedInputBit is number of used bits from readed byte,
    // zero if need to read next byte

    //output value
    TUint val = 0;

    //it may be needed later, depending on aF flag.
    TUint l = aSize;

    //to store the return value of ReadByte function
    TInt ret = 0;

    // get aSize bits, aF aP flags indicates from which side bits are taken
    // (MSB or LSB order)
    // as described in RFC 3220 chapter 8.2.
    while (aSize)
        {
        if (iLastUsedInputBit == 0)
            {
            ret = iInputMessageReader->ReadByte(iLastInputByte);

            if(ret != KErrNone)
            	User::Leave( ret );
            }

        if (aP)
            {
            if (aF)
                {
                // put LSB of input byte as MSB of output 16-bit word
                val >>= 1;
                val |= ((iLastInputByte & 1) << 15);
                }
            else
                {
                // put LSB of input byte as LSB of output 16-bit word
                val <<= 1;
                val |= (iLastInputByte & 1);
                }

            iLastInputByte >>= 1;
            }
        else
            {
            if (aF)
                {
                // put MSB of input byte as MSB of output 16-bit word
                val >>= 1;
                val |= ((iLastInputByte & 0x80) << 8);
                }
            else
                {
                // put MSB of input byte as LSB of output 16-bit word
                val <<= 1;
                val |= ((iLastInputByte & 0x80) >> 7);
                }

            iLastInputByte <<= 1;
            }

        //next bit
        iLastUsedInputBit++;

        if (iLastUsedInputBit > 7)
            {
            iLastUsedInputBit = 0;
            }

        aSize--;
        }

    //if MSB we have to move it to the right.
    if (aF)
        {
        val >>= (16 - l);
        }

    return val;
    }


// ----------------------------------------------------------------------------
// CUdvm::DecodeLiteralOperandL
// decode literal bytecode operand
// ----------------------------------------------------------------------------
//

TUint CUdvm::DecodeLiteralOperandL(TUint& aAddress) const
    {

    TUint value = 0;
    TUint8 tmp = iUdvmMemory->ReadMem8L(aAddress++);

        // Bytecode:                       Operand value:      Range:

    if ((tmp & 0x80) == 0x00)
        {
        // 0nnnnnnn                        N                   0 - 127
        value = tmp & 0x7f;
        }
    else if ((tmp & 0xc0) == 0x80)
        {
        // 10nnnnnn nnnnnnnn               N                   0 - 16383
        value = ((tmp & 0x3f) << 8) + iUdvmMemory->ReadMem8L(aAddress++);
        }
    else if ((tmp & 0xff) == 0xc0)
        {
        // 11000000 nnnnnnnn nnnnnnnn      N                   0 - 65535
        value = (iUdvmMemory->ReadMem8L(aAddress) << 8)+
                 iUdvmMemory->ReadMem8L(aAddress + 1);
        aAddress += 2;
        }
    else
        {
        // argument error
        User::Leave(CSigComp::EDecompressionFailure);
        }

    return value;
    }


// ----------------------------------------------------------------------------
// CUdvm::DecodeReferenceOperandL
// decode reference bytecode operand
// ----------------------------------------------------------------------------
//

TUint CUdvm::DecodeReferenceOperandL(TUint& aAddress) const
    {

    TUint value = 0;
    TUint8 tmp = iUdvmMemory->ReadMem8L(aAddress++);

        // Bytecode:                       Operand value:      Range:

    if ((tmp & 0x80) == 0x00)
        {
        // 0nnnnnnn                        memory[2 * N]       0 - 65535
        value = (tmp & 0x7f) * 2;
        }
    else if ((tmp & 0xc0) == 0x80)
        {
        // 10nnnnnn nnnnnnnn               memory[2 * N]       0 - 65535
        value = (((tmp & 0x3f) << 8) + iUdvmMemory->ReadMem8L(aAddress++)) * 2;
        }
    else if ((tmp & 0xff) == 0xc0)
        {
        // 11000000 nnnnnnnn nnnnnnnn      memory[N]           0 - 65535
        value = (iUdvmMemory->ReadMem8L(aAddress) << 8)+
                 iUdvmMemory->ReadMem8L(aAddress + 1);
        aAddress += 2;
        }
    else
        {
        // argument error
        User::Leave(CSigComp::EDecompressionFailure);
        }

    return value;
    }


// ----------------------------------------------------------------------------
// CUdvm::DecodeMultitypeOperandL
// decode multitype bytecode operand
// ----------------------------------------------------------------------------
//

TUint CUdvm::DecodeMultitypeOperandL(TUint& aAddress) const
    {

    TUint value = 0;
    TUint8 tmp = iUdvmMemory->ReadMem8L(aAddress++);

        // Bytecode:                       Operand value:      Range:

    if ((tmp & 0xc0) == 0x00)
        {
        // 00nnnnnn                        N                   0 - 63
        value = tmp & 0x3f;
        }
    else if ((tmp & 0xc0) == 0x40)
        {
        // 01nnnnnn                        memory[2 * N]       0 - 65535
        value = iUdvmMemory->ReadMem16L((tmp & 0x3f) * 2);
        }
    else if ((tmp & 0xfe) == 0x86)
        {
        // 1000011n                        2 ^ (N + 6)        64 , 128
        value = 1 << ((tmp & 0x01) + 6);
        }
    else if ((tmp & 0xf8) == 0x88)
        {
        // 10001nnn                        2 ^ (N + 8)    256 , ... , 32768
        value = 1 << ((tmp & 0x07) + 8);
        }
    else if ((tmp & 0xe0) == 0xe0)
        {
        // 111nnnnn                        N + 65504       65504 - 65535
        value = (tmp & 0x1f) + 65504;
        }
    else if ((tmp & 0xf0) == 0x90)
        {
        // 1001nnnn nnnnnnnn               N + 61440       61440 - 65535
        value = ((tmp & 0x0f) << 8) + iUdvmMemory->ReadMem8L(aAddress++) +
                61440;
        }
    else if ((tmp & 0xe0) == 0xa0)
        {
        // 101nnnnn nnnnnnnn               N                   0 - 8191
        value = ((tmp & 0x1f) << 8) + iUdvmMemory->ReadMem8L(aAddress++);
        }
    else if ((tmp & 0xe0) == 0xc0)
        {
        // 110nnnnn nnnnnnnn               memory[N]           0 - 65535
        value = iUdvmMemory->ReadMem16L(((tmp & 0x1f) << 8)+
                                        iUdvmMemory->ReadMem8L(aAddress++));
        }
    else if ((tmp & 0xff) == 0x80)
        {
        // 10000000 nnnnnnnn nnnnnnnn      N                   0 - 65535
        value = (iUdvmMemory->ReadMem8L(aAddress) << 8)+
                 iUdvmMemory->ReadMem8L(aAddress + 1);
        aAddress += 2;
        }
    else if ((tmp & 0xff) == 0x81)
        {
        // 10000001 nnnnnnnn nnnnnnnn      memory[N]           0 - 65535
        value = iUdvmMemory->ReadMem16L((iUdvmMemory->ReadMem8L(aAddress) << 8)
                + iUdvmMemory->ReadMem8L(aAddress + 1));
        aAddress += 2;
        }
    else
        {
        // argument error
        User::Leave(CSigComp::EDecompressionFailure);
        }

    return value;
    }

// ----------------------------------------------------------------------------
// CUdvm::DecodeAddressOperandL
// decode address bytecode operand
// ----------------------------------------------------------------------------
//

TUint CUdvm::DecodeAddressOperandL(TUint aPc, TUint& aAddress) const
    {
    return (DecodeMultitypeOperandL(aAddress) + aPc) & KMaxUdvmMemoryMask;
    }


// ----------------------------------------------------------------------------

TBool CUdvm::ExecuteOpcodeDecompressionFailureL(TUint& /*aPc2*/)
    {
    iCycles += 1;
    User::Leave(CSigComp::EDecompressionFailure);
    return EFalse; //not covered, never called because of above Leave().
    }

TBool CUdvm::ExecuteOpcodeAndL(TUint& aPc2)
    {
    // AND ($operand_1, %operand_2)

    TUint op1 = DecodeReferenceOperandL(aPc2);
    TUint op2 = DecodeMultitypeOperandL(aPc2);

    iUdvmMemory->WriteMem16L(op1, iUdvmMemory->ReadMem16L(op1) & op2);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeOrL(TUint& aPc2)
    {
    // OR ($operand_1, %operand_2)

    TUint op1 = DecodeReferenceOperandL(aPc2);
    TUint op2 = DecodeMultitypeOperandL(aPc2);

    iUdvmMemory->WriteMem16L(op1, iUdvmMemory->ReadMem16L(op1) | op2);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeNotL(TUint& aPc2)
    {
    // NOT ($operand_1)

    TUint op1 = DecodeReferenceOperandL(aPc2);

    iUdvmMemory->WriteMem16L(op1, iUdvmMemory->ReadMem16L(op1) ^
                                              KMax16BitValueMask);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeLShiftL(TUint& aPc2)
    {
    // LSHIFT ($operand_1, %operand_2)

    TUint op1 = DecodeReferenceOperandL(aPc2);
    TUint op2 = DecodeMultitypeOperandL(aPc2);

    iUdvmMemory->WriteMem16L(op1, (iUdvmMemory->ReadMem16L(op1) << op2) &
                                                           KMax16BitValueMask);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeRShiftL(TUint& aPc2)
    {
    // RSHIFT ($operand_1, %operand_2)

    TUint op1 = DecodeReferenceOperandL(aPc2);
    TUint op2 = DecodeMultitypeOperandL(aPc2);

    iUdvmMemory->WriteMem16L(op1, (iUdvmMemory->ReadMem16L(op1) >> op2) &
                                                           KMax16BitValueMask);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeAddL(TUint& aPc2)
    {
    // ADD ($operand_1, %operand_2)

    TUint op1 = DecodeReferenceOperandL(aPc2);
    TUint op2 = DecodeMultitypeOperandL(aPc2);

    iUdvmMemory->WriteMem16L(op1, (iUdvmMemory->ReadMem16L(op1) + op2) &
                                                           KMax16BitValueMask);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeSubtractL(TUint& aPc2)
    {
    // SUBTRACT ($operand_1, %operand_2)

    TUint op1 = DecodeReferenceOperandL(aPc2);
    TUint op2 = DecodeMultitypeOperandL(aPc2);

    iUdvmMemory->WriteMem16L(op1, (iUdvmMemory->ReadMem16L(op1) - op2) &
                                                           KMax16BitValueMask);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeMultiplyL(TUint& aPc2)
    {
    // MULTIPLY ($operand_1, %operand_2)

    TUint op1 = DecodeReferenceOperandL(aPc2);
    TUint op2 = DecodeMultitypeOperandL(aPc2);

    iUdvmMemory->WriteMem16L(op1, (iUdvmMemory->ReadMem16L(op1) * op2) &
                                                           KMax16BitValueMask);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeDivideL(TUint& aPc2)
    {
    // DIVIDE ($operand_1, %operand_2)

    TUint op1 = DecodeReferenceOperandL(aPc2);
    TUint op2 = DecodeMultitypeOperandL(aPc2);

    if (op2 == 0)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    iUdvmMemory->WriteMem16L(op1, (iUdvmMemory->ReadMem16L(op1) / op2) &
                                                           KMax16BitValueMask);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeRemainderL(TUint& aPc2)
    {
    // REMAINDER ($operand_1, %operand_2)

    TUint op1 = DecodeReferenceOperandL(aPc2);
    TUint op2 = DecodeMultitypeOperandL(aPc2);

    if (op2 == 0)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    iUdvmMemory->WriteMem16L(op1, (iUdvmMemory->ReadMem16L(op1) % op2) &
                                                           KMax16BitValueMask);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeAscendingL(TUint& aPc2)
    {
    // SORT-ASCENDING (%start, %n, %k)

    TUint start = DecodeMultitypeOperandL(aPc2);
    TUint n = DecodeMultitypeOperandL(aPc2);
    TUint k = DecodeMultitypeOperandL(aPc2);

    TBool sort;
    TUint16 a;
    TUint16 b;

    do
        {
        sort = EFalse;

        for (TUint i = 0; i < (k - 1); i++)
            {
            a = iUdvmMemory->ReadMem16L(start + 2 * i);
            b = iUdvmMemory->ReadMem16L(start + 2 * (i + 1));

            if (a > b)
                {
                iUdvmMemory->WriteMem16L(start + i * 2, b);
                iUdvmMemory->WriteMem16L(start + (i + 1) * 2, a);
                for (TUint j = 1; j < n; j++)
                    {
                    a = iUdvmMemory->ReadMem16L(start + j * k * 2 + i * 2);
                    iUdvmMemory->WriteMem16L(start + j * k * 2 + i * 2,
                    iUdvmMemory->ReadMem16L(start + j * k * 2 + (i + 1) * 2));
                    iUdvmMemory->WriteMem16L(start + j * k * 2 + (i + 1) * 2,
                                                                            a);
                    }
                sort = ETrue;
                }
            }
        }
    while (sort);

    //cost = 1 + k * (ceiling(log2(k)) + n)
    TInt i;
    for (i = 15; i >= 0 && !(k & (1 << i));)
        {
        i--;
        }

    if (k & (~(1 << i)))
        {
        i++;
        }

    iCycles += (1 + (k * (i + n)));
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeDescendingL(TUint& aPc2)
    {
    // SORT-DESCENDING (%start, %n, %k)

    TUint start = DecodeMultitypeOperandL(aPc2);
    TUint n = DecodeMultitypeOperandL(aPc2);
    TUint k = DecodeMultitypeOperandL(aPc2);

    TBool sort;
    TUint16 a;
    TUint16 b;

    do
        {
        sort = EFalse;

        for (TUint i = 0; i < (k - 1); i++)
            {
            a = iUdvmMemory->ReadMem16L(start + i * 2);
            b = iUdvmMemory->ReadMem16L(start + (i + 1) * 2);

            if (a < b)
                {
                iUdvmMemory->WriteMem16L(start + i * 2, b);
                iUdvmMemory->WriteMem16L(start + (i + 1) * 2, a);
                for (TUint j = 1; j < n; j++)
                    {
                    a = iUdvmMemory->ReadMem16L(start + j * k * 2 + i * 2);
                    iUdvmMemory->WriteMem16L(start + j * k * 2 + i * 2,
                    iUdvmMemory->ReadMem16L(start + j * k * 2 + (i + 1) * 2));
                    iUdvmMemory->WriteMem16L(start + j * k * 2 + (i + 1) * 2,
                                                                            a);
                    }
                sort = ETrue;
                }
            }
        }
    while (sort);

    // cost = 1 + k * (ceiling(log2(k)) + n)
    TInt i;
    for (i = 15; i >= 0 && !(k & (1 << i));)
        {
        i--;
        }

    if (k & (~(1 << i)))
        {
        i++;
        }
    iCycles += 1 + k * (i + n);
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeSha1L(TUint& aPc2)
    {
    // SHA-1 (%position, %length, %destination)
    // NOTE: byte copying

    TUint position = DecodeMultitypeOperandL(aPc2);
    TUint length = DecodeMultitypeOperandL(aPc2);
    TUint destination = DecodeMultitypeOperandL(aPc2);

    iCycles += 1 + length;

    TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
    TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

    iSHA1->Reset();

    while (length)
        {
        TUint nextpos = position;
        TUint len = ByteCopyingFragmentL(nextpos,
                                         length,
                                         bcl,
                                         bcr);

        if (len)
            {
            TPtrC8 data = TPtrC8(&iUdvmMemory->MemoryPtr()[position], len);
            iSHA1->Hash(data);
            }

        position = nextpos;
        }

    TPtrC8 hash = iSHA1->Hash(KNullDesC8);
    const TUint8* SHA1result = hash.Ptr();

    TUint respos = 0;
    length = 20;

    while (length)
        {
        TUint newdest = destination;
        TUint len = ByteCopyingFragmentL(newdest,
                                         length,
                                         bcl,
                                         bcr);

        for (TUint i = 0; i < len; i++)
            {
            iUdvmMemory->MemoryPtr()[destination + i] = SHA1result[respos + i];
            }

        destination = newdest;
        respos += len;
        }
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeLoadL(TUint& aPc2)
    {
    // LOAD (%address, %value)

    TUint address = DecodeMultitypeOperandL(aPc2);
    TUint value = DecodeMultitypeOperandL(aPc2);

    iUdvmMemory->WriteMem16L(address, value);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeMultiloadL(TUint& aPc2)
    {
    // MULTILOAD (%address, #n, %value_0, ..., %value_n-1)

    TUint pc = aPc2 - 1;

    TUint address = DecodeMultitypeOperandL(aPc2);
    TUint n = DecodeLiteralOperandL(aPc2);

    for (TUint i = 0; i < n; i++)
        {
        TUint value = DecodeMultitypeOperandL(aPc2);

        // check overlaping instruction location
        if (!((address >= aPc2) || ((address + n * 2) <= pc)))
            {
            User::Leave(CSigComp::EDecompressionFailure);
            }

        iUdvmMemory->WriteMem16L(address + i * 2, value);
        }
    iCycles += 1 + n;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodePushL(TUint& aPc2)
    {
    // PUSH (%value)

    TUint value = DecodeMultitypeOperandL(aPc2);

    TUint sl = iUdvmMemory->ReadMem16L(EReg_stack_location);
    TUint sf = iUdvmMemory->ReadMem16L(sl);

    iUdvmMemory->WriteMem16L((sl + 2 + sf * 2) & KMaxUdvmMemoryMask, value);
    iUdvmMemory->WriteMem16L(sl, (sf + 1) & KMax16BitValueMask);
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodePopL(TUint& aPc2)
    {
    // POP (%address)

    TUint address = DecodeMultitypeOperandL(aPc2);

    TUint sl = iUdvmMemory->ReadMem16L(EReg_stack_location);
    TUint sf = (iUdvmMemory->ReadMem16L(sl) - 1) & KMaxUdvmMemoryMask;

    iUdvmMemory->WriteMem16L(sl, sf);
    iUdvmMemory->WriteMem16L(address,
                        iUdvmMemory->ReadMem16L((sl + 2 + sf * 2) &
                                                     KMaxUdvmMemoryMask));
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeCopyL(TUint& aPc2)
    {
    // COPY (%position, %length, %destination)
    // NOTE: byte copying

    TUint position = DecodeMultitypeOperandL(aPc2);
    TUint length = DecodeMultitypeOperandL(aPc2);
    TUint destination = DecodeMultitypeOperandL(aPc2);

    iCycles += 1 + length;

    TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
    TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

    while (length)
        {
        iUdvmMemory->WriteMem8L(destination, iUdvmMemory->ReadMem8L(position));

        destination++;
        destination &= KMaxUdvmMemoryMask;
        position++;
        position &= KMaxUdvmMemoryMask;

        length--;

        if (destination == bcr)
            {
            destination = bcl;
            }

        if (position == bcr)
            {
            position = bcl;
            }
        }
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeCopyLiteralL(TUint& aPc2)
    {
    // COPY-LITERAL (%position, %length, $destination)
    // NOTE: byte copying

    TUint position = DecodeMultitypeOperandL(aPc2);
    TUint length = DecodeMultitypeOperandL(aPc2);
    TUint destination = DecodeReferenceOperandL(aPc2);

    iCycles += 1 + length;

    if (length)
        {
        TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
        TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

        TUint dest = iUdvmMemory->ReadMem16L(destination);

        while (length)
            {
            iUdvmMemory->WriteMem8L(dest, iUdvmMemory->ReadMem8L(position));

            dest++;
            dest &= KMaxUdvmMemoryMask;
            position++;
            position &= KMaxUdvmMemoryMask;

            length--;

            if (dest == bcr)
                {
                dest = bcl;
                }

            if (position == bcr)
                {
                position = bcl;
                }
            }
        iUdvmMemory->WriteMem16L(destination, dest);
        }
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeCopyOffsetL(TUint& aPc2)
    {
    // COPY-OFFSET (%offset, %length, $destination)
    // NOTE: byte copying

    TUint offset = DecodeMultitypeOperandL(aPc2);
    TUint length = DecodeMultitypeOperandL(aPc2);
    TUint destination = DecodeReferenceOperandL(aPc2);

    iCycles += 1 + length;    // not 1+length+offset !!!

    if (length)
        {
        TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
        TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

        TUint dest = iUdvmMemory->ReadMem16L(destination);

        TUint position = dest;
        while (offset)
            {
            if (position == bcl)
                {
                position = (bcr - 1) & KMaxUdvmMemoryMask;
                }
            else
                {
                position--;
                }
            offset--;
            }

        while (length)
            {
            iUdvmMemory->WriteMem8L(dest, iUdvmMemory->ReadMem8L(position));

            dest++;
            dest &= KMaxUdvmMemoryMask;
            position++;
            position &= KMaxUdvmMemoryMask;

            length--;

            if (dest == bcr)
                {
                dest = bcl;
                }

            if (position == bcr)
                {
                position = bcl;
                }
            }

        iUdvmMemory->WriteMem16L(destination, dest);
        }
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeMemsetL(TUint& aPc2)
    {
    // MEMSET (%address, %length, %start_value, %offset)
    // NOTE: byte copying

    TUint address = DecodeMultitypeOperandL(aPc2);
    TUint length = DecodeMultitypeOperandL(aPc2);
    TUint value = DecodeMultitypeOperandL(aPc2);
    TUint offset = DecodeMultitypeOperandL(aPc2);

    iCycles += 1 + length;

    if (length)
        {
        TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
        TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

        while (length)
            {
            iUdvmMemory->WriteMem8L(address, value);
            value = (value + offset) & KMax8BitValueMask;

            address++;
            address &= KMaxUdvmMemoryMask;
            length--;

            if (address == bcr)
                {
                address = bcl;
                }
            }
        }
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeCompareL(TUint& aPc2)
    {
    /*
    COMPARE (%value_1, %value_2, @address_1,
             @address_2, @address_3)
    */

    TUint pc = aPc2 - 1;

    TUint value1 = DecodeMultitypeOperandL(aPc2);
    TUint value2 = DecodeMultitypeOperandL(aPc2);
    TUint address1 = DecodeAddressOperandL(pc, aPc2);
    TUint address2 = DecodeAddressOperandL(pc, aPc2);
    TUint address3 = DecodeAddressOperandL(pc, aPc2);

    if (value1 < value2)
        {
        aPc2 = address1;
        }
    else if (value1 == value2)
        {
        aPc2 = address2;
        }
    else // if (value1 > value2)
        {
        aPc2 = address3;
        }
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeCallL(TUint& aPc2)
    {
    // CALL (@address)

    TUint pc = aPc2 - 1;

    TUint address = DecodeAddressOperandL(pc, aPc2);

    TUint sl = iUdvmMemory->ReadMem16L(EReg_stack_location);
    TUint sf = iUdvmMemory->ReadMem16L(sl);

    iUdvmMemory->WriteMem16L((sl + 2 + sf * 2) & KMaxUdvmMemoryMask, aPc2);
    iUdvmMemory->WriteMem16L(sl, (sf + 1) & KMax16BitValueMask);

    aPc2 = address;
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeReturnL(TUint& aPc2)
    {
    // RETURN

    TUint sl = iUdvmMemory->ReadMem16L(EReg_stack_location);
    TUint sf = (iUdvmMemory->ReadMem16L(sl) - 1) & KMaxUdvmMemoryMask;

    iUdvmMemory->WriteMem16L(sl, sf);
    aPc2 = iUdvmMemory->ReadMem16L((sl + 2 + sf * 2) & KMaxUdvmMemoryMask);

    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeSwitchL(TUint& aPc2)
    {
    // SWITCH (#n, %j, @address_0, @address_1, ... , @address_n-1)

    TUint pc = aPc2 - 1;

    TUint n = DecodeLiteralOperandL(aPc2);
    TUint j = DecodeMultitypeOperandL(aPc2);

    if (j >= n)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    TUint address;
    do
        {
        address = DecodeAddressOperandL(pc, aPc2);
        }
    while (j--);

    aPc2 = address;

    iCycles += 1 + n;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeCrcL(TUint& aPc2)
    {
    // CRC (%value, %position, %length, @address)
    // NOTE: byte copying

    TUint pc = aPc2 - 1;

    TUint value = DecodeMultitypeOperandL(aPc2);
    TUint position = DecodeMultitypeOperandL(aPc2);
    TUint length = DecodeMultitypeOperandL(aPc2);
    TUint address = DecodeAddressOperandL(pc, aPc2);

    iCycles += 1 + length;

    TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
    TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

    TUint16 crc = Crc::KFcsInit;

    while (length)
        {
        TUint nextpos = position;
        TUint len = ByteCopyingFragmentL(nextpos,
                                         length,
                                         bcl,
                                         bcr);

        if (len)
            {
            crc = Crc::Calc(&iUdvmMemory->MemoryPtr()[position], len, crc);
            }

        position = nextpos;
        }

    if (crc != value)
        {
        aPc2 = address;
        }
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeInputBytesL(TUint& aPc2)
    {
    // INPUT-BYTES (%length, %destination, @address)
    // NOTE: byte copying

    TUint pc = aPc2 - 1;

    TUint length = DecodeMultitypeOperandL(aPc2);
    TUint destination = DecodeMultitypeOperandL(aPc2);
    TUint address = DecodeAddressOperandL(pc, aPc2);

    iCycles += 1 + length;

    if (iLastUsedInputBit)
        {
        iLastUsedInputBit = 0;
        }

    if (iInputMessageReader->Avail(length) != KErrNone)
        {
        aPc2 = address;
        }
    else
        {
        TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
        TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

        while (length)
            {
            TUint nextdest = destination;
            TUint len = ByteCopyingFragmentL(nextdest,
                                             length,
                                             bcl,
                                             bcr);

            if (len)
                {
                TPtr8 blockPtr(iUdvmMemory->MemoryPtr() + destination, len);
                iInputMessageReader->ReadBlock(blockPtr);
                }

            destination = nextdest;
            }
        }
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeInputBitsL(TUint& aPc2)
    {
    // INPUT-BITS (%length, %destination, @address)

    TUint pc = aPc2 - 1;

    TUint length = DecodeMultitypeOperandL(aPc2);
    TUint destination = DecodeMultitypeOperandL(aPc2);
    TUint address = DecodeAddressOperandL(pc, aPc2);

    iCycles += 1;

    if (length > 16)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    TUint ibo = iUdvmMemory->ReadMem16L(EReg_input_bit_order);

    if (ibo & (~(EIBOFlag_P | EIBOFlag_H | EIBOFlag_F)))
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    if ((iLastIBO ^ ibo) & EIBOFlag_P)
        {
        // P bit changed
        iLastUsedInputBit = 0;
        }

    // bytes to read
    TUint btr = (length - ((8 - iLastUsedInputBit) & 7) + 7) >> 3;

    if (iInputMessageReader->Avail(btr) != KErrNone)
        {
        aPc2 = address;
        }
    else
        {
        iUdvmMemory->WriteMem16L(destination,
                                 InputMessageBitsL(length,
                                                   ibo&EIBOFlag_P,
                                                   ibo&EIBOFlag_F));
        }
    iLastIBO = ibo;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeInputHuffmanL(TUint& aPc2)
    {
    // INPUT-HUFFMAN (%destination, @address, #n,
    // bits_1, %lower_bound_1,%upper_bound_1, %uncompressed_1, ...,
    // bits_n, %lower_bound_n,%upper_bound_n, %uncompressed_n)

    TUint pc = aPc2 - 1;

    TUint destination = DecodeMultitypeOperandL(aPc2);
    TUint address = DecodeAddressOperandL(pc, aPc2);
    TUint n = DecodeLiteralOperandL(aPc2);

    iCycles += 1 + n;

    TUint ibo = iUdvmMemory->ReadMem16L(EReg_input_bit_order);

    if (ibo & (~(EIBOFlag_P | EIBOFlag_H | EIBOFlag_F)))
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    if ((iLastIBO ^ ibo) & EIBOFlag_P)
        {
        // P bit changed
        iLastUsedInputBit = 0;
        }

    TUint p = ibo & EIBOFlag_P;
    TUint h = ibo & EIBOFlag_H;

    TUint val = 0;
    TUint bits = 0;
    TUint len;
    TUint lower;
    TUint upper;
    TUint uncompressed;

    TBool quit = EFalse;
    while (n && !quit)
        {
        len = DecodeMultitypeOperandL(aPc2);
        lower = DecodeMultitypeOperandL(aPc2);
        upper = DecodeMultitypeOperandL(aPc2);
        uncompressed = DecodeMultitypeOperandL(aPc2);

        bits += len;
        if (bits > 16)
            {
            User::Leave(CSigComp::EDecompressionFailure);
            }

        // bytes to read
        TUint btr = (len - ((8 - iLastUsedInputBit) & 7) + 7) >> 3;

        if (iInputMessageReader->Avail(btr) != KErrNone)
            {
            aPc2 = address;
            n = 1;
            quit = ETrue;
            }

        if (!quit)
            {
            val = (val << len) | InputMessageBitsL(len, p, h);

            if ((val >= lower) && (val <= upper))
                {
                iUdvmMemory->WriteMem16L(destination,
                            (val + uncompressed - lower) & KMax16BitValueMask);
                quit = ETrue;
                }
            if (!quit)
                {
                n--;
                }
            }
        }

    if (n == 0)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    n--;

    while (n)
        {
        len = DecodeMultitypeOperandL(aPc2);
        lower = DecodeMultitypeOperandL(aPc2);
        upper = DecodeMultitypeOperandL(aPc2);
        uncompressed = DecodeMultitypeOperandL(aPc2);
        n--;
        }

    iLastIBO = ibo;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeStateAccessL(TUint& aPc2)
    {
    /*
        STATE-ACCESS (%partial_identifier_start,
                      %partial_identifier_length,
                      state_begin,
                      %state_length,
                      %state_address, %state_instruction)
        NOTE: byte copying
    */

    TUint partial_identifier_start = DecodeMultitypeOperandL(aPc2);
    TUint partial_identifier_length = DecodeMultitypeOperandL(aPc2);
    TUint state_begin = DecodeMultitypeOperandL(aPc2);
    TUint state_length = DecodeMultitypeOperandL(aPc2);
    TUint state_address = DecodeMultitypeOperandL(aPc2);
    TUint state_instruction = DecodeMultitypeOperandL(aPc2);

    if ((partial_identifier_length < KStateIdentifierMinLength) ||
       (partial_identifier_length > KStateIdentifierMaxLength))
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    __ASSERT_ALWAYS(partial_identifier_start < iUdvmMemory->MemorySize(),
                    User::Leave(CSigComp::EDecompressionFailure));


    __ASSERT_ALWAYS(partial_identifier_start+partial_identifier_length <=
                    iUdvmMemory->MemorySize(),
                    User::Leave(CSigComp::EDecompressionFailure));

    TStateItem* si = iStateMgr->FindStateItem(
                    TPtrC8(&iUdvmMemory->MemoryPtr()[partial_identifier_start],
                    partial_identifier_length));

    if (si == NULL)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    TestStateAccessLenL(si, partial_identifier_length);

    if (state_length == 0)
        {
        state_length = si->iStateLength;
        }

    if (state_address == 0)
        {
        state_address = si->iStateAddress;
        }

    if (state_instruction == 0)
        {
        state_instruction = si->iStateInstruction;
        }

    if ((state_begin + state_length) > si->iStateLength)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    iCycles += 1 + state_length;

    TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
    TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

    while (state_length)
        {
        TUint nextdest = state_address;
        TUint len = ByteCopyingFragmentL(nextdest,
                                         state_length,
                                         bcl,
                                         bcr);

        if (len)
            {
            iUdvmMemory->CopyToMemL(state_address,
                                    &si->iStateValue[state_begin],
                                    len);
            state_begin += len;
            }

        state_address = nextdest;
        }

    if (state_instruction)
        {
        aPc2 = state_instruction;
        }
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeStateCreateL(TUint& aPc2)
    {
    /*
        STATE-CREATE (%state_length,
                      %state_address,
                      %state_instruction,
                      %minimum_access_length,
                      %state_retention_priority)
    */

    if (iStateCreateRequestsNumber == KMaxStateOperations)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    TUint state_length = DecodeMultitypeOperandL(aPc2);
    TUint state_address = DecodeMultitypeOperandL(aPc2);
    TUint state_instruction = DecodeMultitypeOperandL(aPc2);
    TUint minimum_access_length = DecodeMultitypeOperandL(aPc2);
    TUint state_retention_priority = DecodeMultitypeOperandL(aPc2);

    if ((minimum_access_length < KStateIdentifierMinLength) ||
       (minimum_access_length > KStateIdentifierMaxLength) ||
       (state_retention_priority == KMaxUdvmMemoryMask))
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    TUint i = iStateCreateRequestsNumber+iStateFreeRequestsNumber;
    iStateOperationRequests[i].iOperation = EOpcode_STATE_CREATE;
    iStateOperationRequests[i].iStateLength =
                               static_cast<TUint16>(state_length);
    iStateOperationRequests[i].iStateAddress =
                               static_cast<TUint16>(state_address);
    iStateOperationRequests[i].iStateInstruction =
                               static_cast<TUint16>(state_instruction);
    iStateOperationRequests[i].iMinimumAccessLength =
                               static_cast<TUint16>(minimum_access_length);
    iStateOperationRequests[i].iStateRetentionPriority =
                               static_cast<TUint16>(state_retention_priority);
    iStateOperationRequests[i].iStateValue = new (ELeave)
                                             CArrayFixFlat<TUint8>(8);
    iStateCreateRequestsNumber++;
    iStateOperationRequests[i].iStateValue->ResizeL(state_length);

    iCycles += 1 + state_length;

    TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
    TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

    TUint state_begin = 0;
    while (state_length)
        {
        TUint nextdest = state_address;
        TUint len = ByteCopyingFragmentL(nextdest,
                                         state_length,
                                         bcl,
                                         bcr);

        if (len)
            {
            iUdvmMemory->CopyFromMemL(
                      &iStateOperationRequests[i].iStateValue->At(state_begin),
                      state_address,
                      len);
            state_begin += len;
            }
        state_address = nextdest;
        }
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeEndMessageL(TUint& aPc2)
    {
    /*
        END-MESSAGE (%requested_feedback_location,
                     %returned_parameters_location,
                     %state_length, %state_address,
                     %state_instruction,
                     %minimum_access_length,
                    %state_retention_priority)
        NOTE: byte copying
    */

    TUint requested_feedback_location = DecodeMultitypeOperandL(aPc2);
    TUint returned_parameters_location = DecodeMultitypeOperandL(aPc2);
    TUint state_length = DecodeMultitypeOperandL(aPc2);
    TUint state_address = DecodeMultitypeOperandL(aPc2);
    TUint state_instruction = DecodeMultitypeOperandL(aPc2);
    TUint minimum_access_length = DecodeMultitypeOperandL(aPc2);
    TUint state_retention_priority = DecodeMultitypeOperandL(aPc2);

    iCycles += 1 + state_length;

    if ((minimum_access_length >= KStateIdentifierMinLength) &&
       (minimum_access_length <= KStateIdentifierMaxLength) &&
       (state_retention_priority != 0xffff))
        {
        if (iStateCreateRequestsNumber == KMaxStateOperations)
            {
            User::Leave(CSigComp::EDecompressionFailure);
            }

        TUint i = iStateCreateRequestsNumber+iStateFreeRequestsNumber;
        iStateOperationRequests[i].iOperation = EOpcode_STATE_CREATE;
        iStateOperationRequests[i].iStateLength =
                                       static_cast<TUint16>(state_length);
        iStateOperationRequests[i].iStateAddress =
                                       static_cast<TUint16>(state_address);
        iStateOperationRequests[i].iStateInstruction =
                                       static_cast<TUint16>(state_instruction);
        iStateOperationRequests[i].iMinimumAccessLength =
                                   static_cast<TUint16>(minimum_access_length);
        iStateOperationRequests[i].iStateRetentionPriority =
                                static_cast<TUint16>(state_retention_priority);
        iStateOperationRequests[i].iStateValue = new (ELeave)
                                                 CArrayFixFlat<TUint8>(8);
        iStateCreateRequestsNumber++;
        iStateOperationRequests[i].iStateValue->ResizeL(state_length);

        TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
        TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

        TUint state_begin = 0;
        while (state_length)
            {
            TUint nextdest = state_address;
            TUint len = ByteCopyingFragmentL(nextdest,
                                             state_length,
                                             bcl,
                                             bcr);

            if (len)
                {
                iUdvmMemory->CopyFromMemL(
                      &iStateOperationRequests[i].iStateValue->At(state_begin),
                      state_address,
                      len);
                state_begin += len;
                }

            state_address = nextdest;
            }
        }

    if (requested_feedback_location)
        {
        TUint rfl = requested_feedback_location;

        TUint rf = iUdvmMemory->ReadMem8L(rfl);
        rfl++;

        if (rf & ERFFlag_Q)
            {
            // returned feedback item
            TUint rff = iUdvmMemory->ReadMem8L(rfl);
            rfl++;

            if (rff & 0x80)
                {
                // long form
                rfl += rff & 0x7f;
                // check memory out of range
                iUdvmMemory->ReadMem8L(rfl - 1);
                }
            }

        iRequestedFeedback.Set(&iUdvmMemory->MemoryPtr()
                                  [requested_feedback_location],
                               rfl - requested_feedback_location,
                               rfl - requested_feedback_location);
        }
    else
        {
        iRequestedFeedback.Set(NULL, 0, 0);
        }

    if (returned_parameters_location)
        {
        TUint rpl = returned_parameters_location;

        iUdvmMemory->ReadMem8L(rpl);    // cpb dms sms
        rpl++;

        iUdvmMemory->ReadMem8L(rpl);    // version
        rpl++;

        TBool quit = EFalse;
        TUint psi;
        for (; !quit; )
            {
            psi = iUdvmMemory->ReadMem8L(rpl);
            rpl++;

            if ((psi < KStateIdentifierMinLength) ||
                (psi > KStateIdentifierMaxLength))
                {
                quit = ETrue;
                }
            else
                {
                rpl += psi;
                }
            }

        iReturnedParameters.Set(&iUdvmMemory->MemoryPtr()
                                   [returned_parameters_location],
                                rpl - returned_parameters_location,
                                rpl - returned_parameters_location);
        }
    else
        {
        iReturnedParameters.Set(NULL, 0, 0);
        }
    return ETrue;
    }

TBool CUdvm::ExecuteOpcodeStateFreeL(TUint& aPc2)
    {
    /*
        STATE-FREE (%partial_identifier_start,
                    %partial_identifier_length)
    */

    if (iStateFreeRequestsNumber == KMaxStateOperations)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    TUint partial_identifier_start = DecodeMultitypeOperandL(aPc2);
    TUint partial_identifier_length = DecodeMultitypeOperandL(aPc2);

    if ((partial_identifier_length < KStateIdentifierMinLength) ||
        (partial_identifier_length > KStateIdentifierMaxLength))
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    TUint i = iStateCreateRequestsNumber+iStateFreeRequestsNumber;
    iStateOperationRequests[i].iOperation = EOpcode_STATE_FREE;
    iStateOperationRequests[i].iPartialIdentifierStart =
                               static_cast<TUint16>(partial_identifier_start);
    iStateOperationRequests[i].iPartialIdentifierLength =
                               static_cast<TUint16>(partial_identifier_length);
    iStateFreeRequestsNumber++;
    iCycles += 1;
    return EFalse;
    }

TBool CUdvm::ExecuteOpcodeOutputL(TUint& aPc2)
    {
    // OUTPUT (%output_start, %output_length)
    // NOTE: byte copying

    TUint start = DecodeMultitypeOperandL(aPc2);
    TUint length = DecodeMultitypeOperandL(aPc2);

    iCycles += 1 + length;

    TUint bcl = iUdvmMemory->ReadMem16L(EReg_byte_copy_left);
    TUint bcr = iUdvmMemory->ReadMem16L(EReg_byte_copy_right);

    while (length)
        {
        TUint nextstart = start;
        TUint len = ByteCopyingFragmentL(nextstart, length, bcl, bcr);

        if (len)
            {
            iOutputBuffer->InsertL(iOutputBuffer->Size(),
                                   &iUdvmMemory->MemoryPtr()[start],
                                   len);
            }
        start = nextstart;
        }

    // RFC 3320, chapter 9.4.8:
    // Decompression failure occurs if the cumulative number of bytes
    // provided to the dispatcher exceeds 65536 bytes.
    if (iOutputBuffer->Size() > KMaxOutputSize)
        {
        User::Leave(CSigComp::EDecompressionFailure);
                }

    return EFalse;
    }


TBool CUdvm::ExecuteOpcodeJumpL(TUint& aPc2)
    {
    // JUMP (@address)
    TUint pc = aPc2 - 1;
    aPc2 = DecodeAddressOperandL(pc, aPc2);
    iCycles += 1;
    return EFalse;
    }


// ----------------------------------------------------------------------------
// CUdvm::ExecuteCodeL
// execute UDVM code
// ----------------------------------------------------------------------------
//

void CUdvm::ExecuteCodeL(CMessageReader* aMsgReader)
    {
    iInputMessageReader = aMsgReader;
    iLastUsedInputBit = 0;

    TPtr8 retf(iReturnedFeedback);
    DenyStateOperations();
    iReturnedFeedback.Set(retf);

    iOutputBuffer->Reset();

    iCycles = 0;

    TUint pc = iCodeStart;
    TBool terminated = EFalse;

    while (!terminated)
        {
        pc &= KMaxUdvmMemoryMask;
        TUint8 opcode = iUdvmMemory->ReadMem8L(pc);
        pc++;

        if (opcode >= EOpcodesNumber)
            {
            // unknown opcode
            User::Leave(CSigComp::EDecompressionFailure);
            }

        terminated = (this->*iExecuteOpcode[opcode])(pc);

        if (iCycles > iAvailableCycles)
            {
            User::Leave(CSigComp::EDecompressionFailure);
            }
        }
    }


// ----------------------------------------------------------------------------
// CUdvm::AllowStateOperationsL
// allow for state operations
// ----------------------------------------------------------------------------
//

TInt CUdvm::AllowStateOperationsL(CSigCompCompartment* aCompartment)
    {
    TInt sso = 0;

    if (iStateMgr && aCompartment)
        {
        TInt soNum = iStateCreateRequestsNumber + iStateFreeRequestsNumber;

        for (TInt i = 0; i < soNum; i++)
            {
            if (iStateOperationRequests[i].iOperation == EOpcode_STATE_CREATE)
                {
                if (iStateMgr->CreateStateL(aCompartment,
                   iStateOperationRequests[i].iStateLength,
                   iStateOperationRequests[i].iStateAddress,
                   iStateOperationRequests[i].iStateInstruction,
                   iStateOperationRequests[i].iMinimumAccessLength,
                   iStateOperationRequests[i].iStateLength ?
                   &iStateOperationRequests[i].iStateValue->At(0) : NULL,
                   iStateOperationRequests[i].iStateRetentionPriority) != NULL)
                    {
                    sso++;
                    }
                }

            if (iStateOperationRequests[i].iOperation == EOpcode_STATE_FREE)
                {
                if (iStateMgr->FreeStateL(aCompartment,
                                         TPtrC8(
                          &iUdvmMemory->MemoryPtr()
                          [iStateOperationRequests[i].iPartialIdentifierStart],
                          iStateOperationRequests[i].iPartialIdentifierLength))
                          == KErrNone)
                    {
                    sso++;
                    }
                }
            }

        if (sso == soNum)
            {
            iStateMgr->SetReturnedFeedbackL(*aCompartment,
                                            iReturnedFeedback);
            iStateMgr->SetRequestedFeedbackL(*aCompartment,
                                             iRequestedFeedback);
            iStateMgr->SetReturnedParametersL(*aCompartment,
                                              iReturnedParameters);
            }
        }

    return sso;
    }


// ----------------------------------------------------------------------------
// CUdvm::DenyStateOperations
// deny state operations
// ----------------------------------------------------------------------------
//

void CUdvm::DenyStateOperations()
    {

    TUint soNum = iStateCreateRequestsNumber + iStateFreeRequestsNumber;

    for (TUint i = 0; i < soNum; i++)
        {
        if (iStateOperationRequests[i].iOperation == EOpcode_STATE_CREATE)
            {

            delete iStateOperationRequests[i].iStateValue;
            iStateOperationRequests[i].iStateValue = NULL;
            }
        }

    iStateCreateRequestsNumber = 0;
    iStateFreeRequestsNumber = 0;

    iReturnedFeedback.Set(NULL, 0, 0);
    iRequestedFeedback.Set(NULL, 0, 0);
    iReturnedParameters.Set(NULL, 0, 0);
    }


// ----------------------------------------------------------------------------
// CUdvm::CyclesConsumed
// get consumed cycles number
// ----------------------------------------------------------------------------
//

#if defined(SIGCOMP_DEBUG)

TUint CUdvm::CyclesConsumed() const
    {
    return iCycles;
    }

#endif

//  End of File
