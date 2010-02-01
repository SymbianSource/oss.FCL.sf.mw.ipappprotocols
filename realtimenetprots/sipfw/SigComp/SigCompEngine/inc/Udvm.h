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
* Name        : Udvm.h
* Part of     : SigComp / UDVM
* Interface   : 
* UDVM core
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef UDVM_H
#define UDVM_H

//  INCLUDES
#include <e32base.h>
#include <hash.h>

class CUdvmMemory;
class CMessageReader;

//consts
static const TUint KMaxUdvmMemorySize = 65536;
static const TUint KMaxUdvmMemoryMask = 0xffff;
static const TUint KSigCompHeaderMask = 0xF8;
static const TInt KSigCompHeaderTBit = 2;
static const TInt KSigCompHeaderTMask = (1 << KSigCompHeaderTBit);
static const TInt KSigCompHeaderLenMask = 0x03;
static const TInt KReturnedFeedbackFieldBit = 7;
static const TInt KReturnedFeedbackFieldMask =
                                             (1 << KReturnedFeedbackFieldBit);
static const TInt KReturnedFeedbackLengthMask = 0x7f;
static const TUint KMax8BitValueMask = 0xff;
static const TUint KMax16BitValueMask = 0xffff;
static const TUint KMaxStateOperations = 4;
static const TInt KMaxOutputSize = 65536;

// CLASS DECLARATION

/* UDVM memory and registers map

    0             7 8            15
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |       UDVM_memory_size        |  0 - 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |        cycles_per_bit         |  2 - 3
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |        SigComp_version        |  4 - 5
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |    partial_state_ID_length    |  6 - 7
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         state_length          |  8 - 9
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                               |
   :           reserved            :  10 - 31
   |                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                               |
   :   zeroed scratch-pad memory   :  32 - 63
   |                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

              registers

   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |        byte_copy_left         |  64 - 65
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |        byte_copy_right        |  66 - 67
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |        input_bit_order        |  68 - 69
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |        stack_location         |  70 - 71
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/


/**
*  Class for holding state create/free operation requestes
*  
*
*  @lib sigcomp
*/

class TStateOperationRequest
    {

    public:
        TUint iOperation;

        /** state create parameters */
        TUint16 iStateLength;
        TUint16 iStateAddress;
        TUint16 iStateInstruction;
        TUint16 iMinimumAccessLength;
        TUint16 iStateRetentionPriority;
        CArrayFixFlat<TUint8>* iStateValue;

        /** state free parameters */
        TUint16 iPartialIdentifierStart;
        TUint16 iPartialIdentifierLength;
    };

/**
*  Universal decompression virtual machine core class.
*  
*
*  @lib sigcomp
*/

class CSigCompCompartment;
class CStateMgr;
class TStateItem;

class CUdvm : public CBase
    {

    public:  // enumerators

        /** memory and registers map
        *   as defined in RFC3220 chapter 7.2.
        **/
        enum TMemMap
            {
            EMem_UDVM_memory_size = 0,
            EMem_cycles_per_bit = 2,
            EMem_SigComp_version = 4,
            EMem_partial_state_ID_length = 6,
            EMem_state_length = 8,
            EMem_reserved = 10,
            EMem_scratch_pad = 32,
            EMem_registers = 64,
            //register block definition starts here
            EReg_byte_copy_left = 64,
            EReg_byte_copy_right = 66,
            EReg_input_bit_order = 68,
            EReg_stack_location = 70,
            EMem_user_area = 72
            };

        /** input_bit_order flags */
        enum TIBOFlags
            {
            EIBOFlag_P = 1,
            EIBOFlag_H = 2,
            EIBOFlag_F = 4
            };

        /** requested feedback flags */
        enum TRFFlags
            {
            ERFFlag_I = 1,
            ERFFlag_S = 2,
            ERFFlag_Q = 4
            };

        /** UDVM bytecodes */
        enum TOpcode
            {
            EOpcode_DECOMPRESSION_FAILURE = 0,
            EOpcode_AND = 1,
            EOpcode_OR = 2,
            EOpcode_NOT = 3,
            EOpcode_LSHIFT = 4,
            EOpcode_RSHIFT = 5,
            EOpcode_ADD = 6,
            EOpcode_SUBTRACT = 7,
            EOpcode_MULTIPLY = 8,
            EOpcode_DIVIDE = 9,
            EOpcode_REMAINDER = 10,
            EOpcode_SORT_ASCENDING = 11,
            EOpcode_SORT_DESCENDING = 12,
            EOpcode_SHA_1 = 13,
            EOpcode_LOAD = 14,
            EOpcode_MULTILOAD = 15,
            EOpcode_PUSH = 16,
            EOpcode_POP = 17,
            EOpcode_COPY = 18,
            EOpcode_COPY_LITERAL = 19,
            EOpcode_COPY_OFFSET = 20,
            EOpcode_MEMSET = 21,
            EOpcode_JUMP = 22,
            EOpcode_COMPARE = 23,
            EOpcode_CALL = 24,
            EOpcode_RETURN = 25,
            EOpcode_SWITCH = 26,
            EOpcode_CRC = 27,
            EOpcode_INPUT_BYTES = 28,
            EOpcode_INPUT_BITS = 29,
            EOpcode_INPUT_HUFFMAN = 30,
            EOpcode_STATE_ACCESS = 31,
            EOpcode_STATE_CREATE = 32,
            EOpcode_STATE_FREE = 33,
            EOpcode_OUTPUT = 34,
            EOpcode_END_MESSAGE = 35,
            EOpcodesNumber = 36
            };


    public:  // Constructors and destructor

        /**
         * @param aStateMgr state manager
         * @param aMemSize UDVM memory size in bytes
         * @param aCyclesPerBit the amount of "UDVM cycles" available
         *                      to decompress each bit in SigComp message
         */

        static CUdvm* NewL(CStateMgr* aStateMgr,
                           TUint aMemSize,
                           TUint aCyclesPerBit);

        static CUdvm* NewLC(CStateMgr* aStateMgr,
                            TUint aMemSize,
                            TUint aCyclesPerBit);

        /**
         * Destructor.
         */

        virtual ~CUdvm();


    public: // New functions

        /**
         * Decompress message.
         *
         * @param aMessage message to decompression
         * @param aBytesConsumed on return will contain the number of bytes
         *                       consumed by the decompression from the
         *                       original message 
         *
         * @returns decompressed message, the ownership is transferred
         */

        CBufBase* DecompressL(const TDesC8& aMessage, TUint& aBytesConsumed,
                              TBool aStreamBasedProtocol);


        /**
         * Execute UDVM code.
         *
         * @param aMsgReader message reader.
         *
         */

        void ExecuteCodeL(CMessageReader* aMsgReader);

        /**
         * Allow for state operations.
         *
         * @param aCompartment compartment
         *
         * @returns number of completed state operations
         */

        TInt AllowStateOperationsL(CSigCompCompartment* aCompartment);

        /**
         * Deny state operations.
         *
         */

        void DenyStateOperations();

        /**
         * Resolve "byte copying". Find continuous block in bcl/bcr range from
         * given address. Leave if out of memory bounds.
         *
         * @param aAddress address of area
         * @param aLength length of area
         * @param aBcl UDVM bcl register value
         * @param aBcr UDVM bcr register value
         *
         * @returns number of bytes to copy continuous block
         *
         * @post aAddress and aLength is updated to next block address and
         *       length
         */

        TUint ByteCopyingFragmentL(TUint& aAddress,
                                   TUint& aLength, 
                                   TUint aBcl, TUint aBcr) const;


        /**
         * Compare MinmumStateAccessLength with AccessLength
         * Leave if AccessLength too small.
         *
         * @param aStateItem state item
         * @param aAccessLen candidate access length
         *
         */

        void TestStateAccessLenL(const TStateItem* aStateItem,
                                 TUint aAccessLen) const;


        /**
         * Get a portion of bits from input stream.
         *
         * @param aSize number of bits to get
         * @param aP P flag of input_bit_order UDVM register 
         * @param aF F flag of input_bit_order UDVM register
         *
         * @returns value
         */

        TUint InputMessageBitsL(TUint aSize, TUint aP, TUint aF);


        /**
         * Decode literal bytecode operand.
         *
         * @param aAddress address in UDVM memory
         *
         * @returns value
         *
         * @post aAddress is updated
         */

        TUint DecodeLiteralOperandL(TUint& aAddress) const;


        /**
         * Decode reference bytecode operand.
         *
         * @param aAddress address in UDVM memory
         *
         * @returns address of value in UDVM memory
         *
         * @post aAddress is updated
         */

        TUint DecodeReferenceOperandL(TUint& aAddress) const;


        /**
         * Decode multitype bytecode operand.
         *
         * @param aAddress address in UDVM memory
         *
         * @returns operand value
         *
         * @post aAddress is updated
         */

        TUint DecodeMultitypeOperandL(TUint& aAddress) const;


        /**
         * Decode address bytecode operand.
         *
         * @param aPc current value of UDVM PC "register"
         * @param aAddress address in UDVM memory
         *
         * @returns operand value
         *
         * @post aAddress is updated
         */

        TUint DecodeAddressOperandL(TUint aPC, TUint& aAddress) const;


        /**
         * Upload bytecode, initialize UDVM memory and registers.
         *
         * @param aDest destination address in UDVM memory
         * @param aLen code length
         * @param aMsgReader message reader.
         *
         */

        void UploadCodeL(TUint aDest, TUint aLen, CMessageReader* aMsgReader);


        /**
         * Upload state, initialize UDVM memory and registers.
         *
         * @param aStateLen partial state identifier length
         * @param aState partial state identifier
         *
         */

        void UploadStateL(TUint aStateLen, const TUint8* aState);


        /**
         * Set returned feedback
         *
         * @param aT T flag.
         * @param aMsgReader message reader.
         *
         */

        void SetReturnedFeedbackL(TBool aT, CMessageReader* aMsgReader);

        /**
         * Upload state in message
         *
         * @param aLen length
         * @param aMsgReader message reader.
         *
         */

        void UploadStateInMessageL(TUint aLen, CMessageReader* aMsgReader);

        /**
         * Upload bytecode in message
         *
         * @param aMsgReader message reader.
         *
         */
        void UploadBytecodeInMessageL(CMessageReader* aMsgReader);

#if defined(SIGCOMP_DEBUG)
        /**
         * Get consumed cycles number
         *
         * @returns consumed cycles number.
         *
         */
        TUint CyclesConsumed() const;
#endif

    private:
        CUdvm();

        /**
         * execute opcode "Decompression failure".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeDecompressionFailureL(TUint& aPc2);

        /**
         * execute opcode "AND
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeAndL(TUint& aPc2);

        /**
         * execute opcode "OR
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeOrL(TUint& aPc2);

        /**
         * execute opcode "NOT
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeNotL(TUint& aPc2);

        /**
         * execute opcode "LSHIFT
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeLShiftL(TUint& aPc2);

        /**
         * execute opcode "RSHIFT
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeRShiftL(TUint& aPc2);

        /**
         * execute opcode "ADD
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeAddL(TUint& aPc2);

        /**
         * execute opcode "SUBTRACT"
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeSubtractL(TUint& aPc2);

        /**
         * execute opcode "MULTIPLY"
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeMultiplyL(TUint& aPc2);

        /**
         * execute opcode "DIVIDE".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeDivideL(TUint& aPc2);

        /**
         * execute opcode "REMAINDER".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeRemainderL(TUint& aPc2);

        /**
         * execute opcode "ascending".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeAscendingL(TUint& aPc2);

        /**
         * execute opcode "DESCENDING".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeDescendingL(TUint& aPc2);

        /**
         * execute opcode "SHA1".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeSha1L(TUint& aPc2);

        /**
         * execute opcode "LOAD".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeLoadL(TUint& aPc2);

        /**
         * execute opcode "MULTILOAD".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeMultiloadL(TUint& aPc2);

        /**
         * execute opcode "PUSH".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodePushL(TUint& aPc2);

        /**
         * execute opcode "POP".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodePopL(TUint& aPc2);

        /**
         * execute opcode "COPY".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeCopyL(TUint& aPc2);

        /**
         * execute opcode "COPYLITERAL".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeCopyLiteralL(TUint& aPc2);

        /**
         * execute opcode "COPY OFFSET".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeCopyOffsetL(TUint& aPc2);

        /**
         * execute opcode "MEMSET".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeMemsetL(TUint& aPc2);

        /**
         * execute opcode "COMPARE".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeCompareL(TUint& pc2);

        /**
         * execute opcode "CALL".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeCallL(TUint& pc2);

        /**
         * execute opcode "RETURN".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeReturnL(TUint& pc2);

        /**
         * execute opcode "SWITCH".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeSwitchL(TUint& pc2);

        /**
         * execute opcode "CRC".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeCrcL(TUint& aPc2);

        /**
         * execute opcode "INPUT BYTES".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeInputBytesL(TUint& aPc2);

        /**
         * execute opcode "INPUT BITS".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeInputBitsL(TUint& aPc2);

        /**
         * execute opcode "INPUT HUFFMAN".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeInputHuffmanL(TUint& aPc2);

        /**
         * execute opcode "STATE ACCESS".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeStateAccessL(TUint& aPc2);

        /**
         * execute opcode "STATE CREATE".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeStateCreateL(TUint& aPc2);

        /**
         * execute opcode "STATE FREE".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeStateFreeL(TUint& aPc2);

        /**
         * execute opcode "ENDMESSAGE".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeEndMessageL(TUint& aPc2);

        /**
         * execute opcode "OUTPUT".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeOutputL(TUint& aPc2);

        /**
         * execute opcode "JUMP".
         *
         * @param aPc2 program counter
         *
         * @returns ETrue if opcode causes termination of UDVM program
         *
         */
        TBool ExecuteOpcodeJumpL(TUint& aPc2);

    protected:
        void ConstructL(CStateMgr* aStateMgr,
                        TUint aMemSize,
                        TUint aCyclesPerBit);


    private:    // Data

        /** start instruction */
        TUint iCodeStart;

        /** cycles per bit */
        TUint iCyclesPerBit;

        /** cycles available for decompression */
        TUint iAvailableCycles;

        /** output (decompressed) message buffer */
        CBufFlat* iOutputBuffer; // Not owned

        /** UDVM memory */
        CUdvmMemory* iUdvmMemory;

        /** input message reader */
        CMessageReader* iInputMessageReader;

        /** last message input byte value */
        TUint iLastInputByte;

        /** used bits from last input byte */
        TUint iLastUsedInputBit;

        /** input bit order */
        TUint iLastIBO;

        /** SHA-1 instance */
        CSHA1* iSHA1;

        /** state manager */
        CStateMgr* iStateMgr;

        /** state operation requests */
        TStateOperationRequest iStateOperationRequests[8];

        /**  current number of state create requests*/
        TUint iStateCreateRequestsNumber;

        /** current number of state free requests */
        TUint iStateFreeRequestsNumber;

        /** returned feedback */
        TPtr8 iReturnedFeedback;

        /** requested feedback */
        TPtr8 iRequestedFeedback;

        /** returned parameters */
        TPtr8 iReturnedParameters;

        /** current used cycles */
        TUint iCycles;

        /** UDVM instruction lookup table */
        TBool (CUdvm:: * iExecuteOpcode[EOpcodesNumber])(TUint& aPc2);

    };

#endif
            
// End of File
