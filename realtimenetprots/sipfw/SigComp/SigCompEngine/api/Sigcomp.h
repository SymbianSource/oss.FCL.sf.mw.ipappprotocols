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
* Name        : SigComp.h
* Part of     : SigComp
* Interface   : 
* main sigcomp class, sigcomp public API.
* Version     : 1.0
*
*/



#ifndef CSIGCOMP_H
#define CSIGCOMP_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSigCompCompartment;
class TSigCompStateItem;
class CUdvm;
class CStateMgr;
class CSigCompCompressor;
class TStateItem;

// CONSTANTS
const TInt KSigCompVersion = 0x0001;

// CLASS DECLARATION
/**
* @publishedPartner
* @released
* 
* @class CSigComp sigcomp.h "sigcomp.h"
*
* This class is used for compressing and decompressing text-based protocols
* using static or dynamic compression. Different algorithms can be used in
* order to compress the signalling messages. 
*
* The user of the class must call AllowL() or Deny() immediately after
* verification
* of the decompressed message. If the user fails to call AllowL()/Deny()
* the UDVM memory will be freed next time the user will use SigComp services.
* It is the user's responsibility to map the decompressed
* message under appropriate compartment.
*
* The user is supposed to use only one instance of the class while using
* the compression of the particular text-based protocol e.g. SIP/SDP. The
* compression algorithm and static dictionary to be used are chosen while
* creating the instance of the class The static dictionary will be shared
* between different compartments. All compartments created will use the
* compression algorithm defined during the instantion of the class.
*
* If the precondintions are not met the function will leave with KErrArgument
* system wide error code.
*/
class CSigComp : public CBase
    {
    public: 

        /** Error codes*/
        enum TError 
            {

            /** Decompression failure occurred*/
            EDecompressionFailure = -3000,
            /** The SigComp message was incomplete*/
            EIncompleteMessage,
            /**Compression failure occured*/
            ECompressionFailure

            };


        /** 
          * The number of "UDVM cycles" available to decompress each
          * bit in a SigComp message.
          */
        enum TCyclesPerBit
            {

            /** 16 cycles */
            E16 = 16,        
            /** 32 cycles */
            E32 = 32,
            /** 64 cycles */
            E64 = 64,
            /** 128 cycles */
            E128 = 128

            };


        /**
          * The number of bytes offered
          * to a particular compartment for the creation of state;
          * also defines the amount of memory available to decompress
          * SigComp message.
          */
        enum TMemorySize
            {
            /** 0 bytes */
            E0 = 0,
            /** 2048 bytes */
            E2048 = 2048,
            /** 4096 bytes */
            E4096 = 4096,
            /** 8192 bytes */
            E8192 = 8192,
            /** 16384 bytes */
            E16384 = 16384,
            /** 32768 bytes */
            E32768 = 32768,
            /** 65536 bytes */
            E65536 = 65536,
            /** 131072 bytes */
            E131072 = 131072

            };
    



    public:

        /**
         * Destructor
         */

        IMPORT_C ~CSigComp();

        /**
         * Constructs an object; leaves on failure. If the algorithm is
         * not supported, the function leaves with KErrNotFound
         * error code.
         *
         * @pre aDecompressionMemSize != TMemorySize::E0
         *
         * @param aDecompressionMemSize The amount of memory in bytes
         *                available decompress one
         *                SigComp message.
         * @param aCyclesPerBit The amount of "UDVM cycles" available to
         *                      decompress each bit in SigComp message
         * @param aStateMemorySize The default amount of memory offered for
         *                         each compartment for saving the states.
         * @param aAlgorithm       an algorithm to be used for the compression
         *                         e.g. DEFLATE
         *
         * @return new instance
         */

        IMPORT_C static CSigComp* NewL(TMemorySize aDecompressionMemSize,
                       TCyclesPerBit aCyclesPerBit,
                       TMemorySize aStateMemorySize,
                       const TDesC8& aAlgorithm);

        /**
         * Constructs an object; leaves on failure. If the algorithm is not
         * supported, the function leaves with KErrNotFound error code.
         * If the state item value is not found, the function leaves with
         * KErrNotFound error code.
         *
         * @pre aDecompressionMemSize != TMemorySize::E0
         *
         * @param aDecompressionMemSize The amount of memory in bytes available
         *                              to decompress one SigComp message.
         * @param aCyclesPerBit The amount of "UDVM cycles" available to
         *                      decompress each bit in SigComp message
         * @param aStateMemorySize The default amount of memory offered for
         *                         each compartment for saving the states.
         * @param aAlgorithm       an algorithm to be used for the compression
         *                         e.g. DEFLATE
         * @param aStateItem       the local state to be used with every
         *                         compartment e.g. static dictionary
         * @param aUploadState     If set to ETrue, the state item will be
         *                         uploaded to the remote endpoint in the first
         *                         SigComp message sent within the compartment.
         *                         
         * @return new instance
         */


        IMPORT_C static CSigComp* NewL(TMemorySize aDecompressionMemSize,
                       TCyclesPerBit aCyclesPerBit,
                       TMemorySize aStateMemorySize,
                       const TDesC8& aAlgorithm,
                       const TSigCompStateItem& aStateItem,
                       TBool aUploadState = EFalse);


                                       

        /**
         * Constructs an object and adds the pointer to the cleanup stack;
         * leaves on failure.
         * If the algorithm is not
         * supported, the function leaves with KErrNotFound error code.
         *
         * @pre aDecompressionMemSize != TMemorySize::E0
         *
         * @param aDecompressionMemSize The amount of memory in bytes available
         *                              to decompress one SigComp message.
         * @param aCyclesPerBit The amount of "UDVM cycles" available to
         *                      decompress each bit in SigComp message
         * @param aStateMemorySize The default amount of memory offered for
         *                         each compartment for saving the states.
         * @param aAlgorithm       an algorithm to be used for the compression
         *                         e.g. DEFLATE
         *
         * @return new instance
         */

        IMPORT_C static CSigComp* NewLC(TMemorySize aDecompressionMemSize,
                        TCyclesPerBit aCyclesPerBit,
                        TMemorySize aStateMemorySize,
                        const TDesC8& aAlgorithm);



        /**
         * Constructs an object and adds the pointer to the cleanupstack;
         * leaves on failure. 
         * If the algorithm is not supported, the function leaves with
         * KErrNotFound error code.
         * If the state item value is not found, the function leaves
         * with KErrNotFound error code.
         *
         * @pre aDecompressionMemSize != TMemorySize::E0
         *
         * @param aDecompressionMemSize The amount of memory in bytes available
         *                              to decompress one SigComp message.
         * @param aCyclesPerBit The amount of "UDVM cycles" available to
         *                      decompress each bit in SigComp message
         * @param aStateMemorySize The default amount of memory offered for
         *                         each compartment for saving the states.
         * @param aAlgorithm       an algorithm to be used for the compression
         *                         e.g. DEFLATE
         * @param aStateItem       the local state to be used with every
         *                         compartment e.g. static dictionary
         * @param aUploadState     If set to ETrue, the state item will be
         *                         uploaded to the remote endpoint in the
         *                         first SigComp message sent within the
         *                         compartment.
         *                         
         * @return new instance
         */


        IMPORT_C static CSigComp* NewLC(TMemorySize aDecompressionMemSize,
                            TCyclesPerBit aCyclesPerBit,
                            TMemorySize aStateMemorySize,
                            const TDesC8& aAlgorithm,
                            const TSigCompStateItem& aStateItem,
                            TBool aUploadState = EFalse);

        /**
         * Checks if the first byte contains bit pattern indicating the start
         * of the SigComp message. Function examines only the first byte.
         *
         * @param aMessage a message
         * @return ETrue if the first byte indicates the start of the SigComp
         *          message, EFalse otherwise
         */

        IMPORT_C TBool IsSigCompMsg(const TDesC8& aMessage) const;


        /**
         * Checks if the message contains the delimeter indicating SigComp
         * message end. This function must be used only with stream-based
         * transports, otherwise it will always return EFalse.
         *
         * @pre IsSigCompMsg()==ETrue
         *
         * @param aMessage a message
         * @return ETrue is the SigComp message is complete, EFalse otherwise
         */

        IMPORT_C TBool IsCompleteL(const TDesC8& aMessage) const;

                    
        /**
         * Decompress the message; leaves on failure. 
         *
         * SigComp specific leave codes:
         * EDecompressionFailure - the decompressor failed to decompress the
         * message.
         * EIncompleteMessage - the SigComp message was incomplete
         * The function expects the client to provide complete SigComp message.
         *
         * @pre IsSigCompMsg()==ETrue
         * 
         * @param aMessage a message to decompress. The message must start with
         *                 SigComp specific delimeter and end with SigComp
         *                 specific delimeter
         *
         * @param aBytesConsumed on return will contain the number of bytes
         *                       consumed by the decompression from the
         *                       original message
         * @param aStreamBasedProtocol if set to ETrue, the message was received
         *                       over stream-based transport protocol
         * @returns decompressed message, the ownership is transferred
         */

        IMPORT_C CBufBase* DecompressL(const TDesC8& aMessage,
                                       TUint& aBytesConsumed,
                                       TBool aStreamBasedProtocol = EFalse);

        /**
         * Allows the state of the decompressed message to be saved; 
         * leaves on failure.
         * 
         * @param aSigCompCompartment the compartment under which the
         * state will be saved.
         */

        IMPORT_C void AllowL(CSigCompCompartment& aSigCompCompartment);

        /**
         * Denies the state of the message to be saved.
         */

        IMPORT_C void Deny();


        /**
         * Get decompression memory size parameter.
         *
         * @return decompression memory size
         */

        TMemorySize DecompressionMemorySize() const;


        /**
         * Get cycles per bit parameter.
         *
         * @return cycles per bit
         */

        TCyclesPerBit CyclesPerBit() const;


        /**
         * Get state memory size parameter.
         *
         * @return state memory size
         */

        TMemorySize StateMemorySize() const;


        /**
         * Get state manager instance.
         *
         * @return state manager
         */

        const CStateMgr* StateMgr() const;

        /**
         * Get compressor instance.
         *
         * @return compressor
         */

        const CSigCompCompressor* Compressor() const;

        /**
         * Get state memory size in bytes
         *
         * @return state memory size in bytes
         */

        TUint StateMemoryBytes() const;

#if defined(SIGCOMP_DEBUG)
        /**
         * Get consumed cycles number
         *
         * @return consumed cycles number.
         *
         */
        IMPORT_C TUint CyclesConsumed() const;
#endif

    private:

        CSigComp();

        void ConstructL(TMemorySize aDecompressionMemSize,
                TCyclesPerBit aCyclesPerBit,
                TMemorySize aStateMemorySize,
                const TDesC8& aAlgorithm);

        void ConstructL(TMemorySize aDecompressionMemSize,
                TCyclesPerBit aCyclesPerBit,
                TMemorySize aStateMemorySize,
                const TDesC8& aAlgorithm,
                const TSigCompStateItem& aStateItem,
                TBool aUploadState = EFalse);

        void ConstructL(TMemorySize aDecompressionMemSize,
                        TCyclesPerBit aCyclesPerBit,
                        TMemorySize aStateMemorySize,
                        TBool aUploadState);

    private:
        /** decompression memory size, in bytes */
        TInt iMemSize;

        /** cycles per bit */
        TCyclesPerBit iCyclesPerBit;

        /** state memory size */
        TMemorySize iStateMemorySize;

        /**  should the state be uploaded */
        TBool iUploadState;

        /** state manager instance */
        CStateMgr* iStateMgr;

        /** UDVM instance */
        CUdvm* iUdvm;

        /** compressor instance */
        CSigCompCompressor* iCompressor;

    };

#endif // CSIGCOMP_H
