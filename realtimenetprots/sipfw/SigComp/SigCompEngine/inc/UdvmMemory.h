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
* Name        : UdvmMemory.h
* Part of     : SigComp / UDVM
* Interface   : 
* UDVM memory/memory manipulation
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef UDVMMEMORY_H
#define UDVMMEMORY_H

/**
*  Universal decompression virtual machine memory
*
*  @lib sigcomp
*/

class CUdvmMemory : public CBase
    {
    public:  // Constructors and destructor

        /**
         * @param aMemSize UDVM memory size in bytes
         */

        static CUdvmMemory* NewL(TUint aMemSize);

        static CUdvmMemory* NewLC(TUint aMemSize);

        /**
         * Destructor.
         */

        virtual ~CUdvmMemory();


    public: // New functions

        /**
         * Write byte to UDVM memory.
         * Leave if out of memory bounds.
         *
         * @pre 0<=aDest<65536
         *
         * @param aDest destination address in UDVM memory
         * @param aVal value, 8-bit significant
         *
         */

        void WriteMem8L(TUint aDest, TUint aVal);


        /**
         * Write 16-bit word to UDVM memory.
         * Leave if out of memory bounds.
         *
         * @pre 0<=aDest<65536
         *
         * @param aDest destination address in UDVM memory
         * @param aVal value, 16-bit significant
         *
         */

        void WriteMem16L(TUint aDest, TUint aVal);


        /**
         * Read byte from UDVM memory.
         * Leave if out of memory bounds.
         *
         * @pre 0<=aSrc<65536
         *
         * @param aSrc source address in UDVM memory
         *
         * @returns 8-bit value
         */

        TUint8 ReadMem8L(TUint aSrc) const;


        /**
         * Read 16-bit word from UDVM memory.
         * Leave if out of memory bounds.
         *
         * @pre 0<=aSrc<65536
         *
         * @param aSrc source address in UDVM memory
         *
         * @returns 16-bit value
         */

        TUint16 ReadMem16L(TUint aSrc) const;


        /**
         * Set block of UDVM memory to value.
         * Leave if out of memory bounds.
         *
         * @pre 0<=aDest<65536
         *
         * @param aDest destination address in UDVM memory
         * @param aVal value
         * @param aSize size of block
         *
         */

        void SetMemL(TUint aDest, TUint8 aVal, TUint aSize);


        /**
         * Copy block of data to UDVM memory.
         * Leave if out of memory bounds.
         *
         * @pre 0<=aDest<65536
         *
         * @param aDest destination address in UDVM memory
         * @param aSrc source address
         * @param aSize size of block
         *
         */

        void CopyToMemL(TUint aDest, const TUint8* aSrc, TUint aSize);


        /**
         * Copy block of data from UDVM memory.
         * Leave if out of memory bounds.
         *
         * @pre 0<=aSrc<65536
         *
         * @param aDest destination address
         * @param aSrc source address in UDVM memory
         * @param aSize size of block
         *
         */

        void CopyFromMemL(TUint8* aDest, TUint aSrc, TUint aSize) const;

        /**
         * Initialize UDVM memory.
         * Leave if UDVM memory too small for message.
         *
         * @param aMsgSize size of message
         *
         */
        void InitMemoryL(TUint aMsgSize, TUint aCyclesPerBit);

        /**
         * Get pointer to UDVM memory.
         *
         * @returns UDVM memory
         */
        TUint8* MemoryPtr();

        /**
         * Get size of UDVM memory
         *
         * @returns UDVM memory size
         */
        TUint MemorySize() const;

        /**
         * Get size of free UDVM memory
         *
         * @returns UDVM free memory size
         */
        TUint FreeMemorySize() const;


        /**
         * Check if access to UDVM memory is allowed.
         *
         * @param aAddr address of block in UDVM memory
         * @param aSize size of block that will be accessed.
         */
        void CheckMemAccessL(TUint aAddr, TUint aSize) const;

    protected:

        /**
        * @param aMemSize UDVM memory size
        * @param aMsgSize sizeof incoming message
        */

        void ConstructL(TUint aMemSize);


    private:
        CUdvmMemory();

    private:    // Data
        /** size of UDVM memory (up to 65536 bytes) */
        TUint iMemSize;

        /** size of decompression memory */
        TUint iFreeMemSize;

        /** total decompression memory (UDVM and buffered message) */
        CArrayFixFlat<TUint8>* iMem;
    };

#endif
            
// End of File
