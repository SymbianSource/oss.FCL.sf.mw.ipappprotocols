/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : CDeflate.h
* Part of     : deflatecomp / deflatecomp
* deflate compressor header file.
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CDEFLATE_H
#define CDEFLATE_H

class CSigCompDeflateContext;
class CMessageWriter;

const TInt KMaxBits = 15;
const TInt KLengthCodes = 29;
const TInt KLiterals = 256;
const TInt KLiteralCodes = (KLiterals + 1 + KLengthCodes);
const TInt KDistanceCodes = 30;
const TInt KMinMatch = 3;
const TInt KMaxMatch = 258;
const TUint KEncodeDistance = 256;

/** length of good match, search stops after one of this length
has been found
*/
const TInt KGoodMatchLength = 16;

/** length of nice match, chainlength will be reduced if one of
this length has been found
*/
const TInt KNiceMatchLength = 8;

/** Do not perform lazy matching if we found match of this length
*/
const TInt KNiceLazyMatchLength = 16;


/** maximum number of chains linked to one hash table entry */
const TInt KMaxChainLength = 32;

/*
number of bits for hash table. hash table size = 1<<KHashTableBits
the lower this number is, the slower compression is (because of
bigger possibility of conflicts in hashing) but lower the memory consumption
15 is probably the bigger reasonable number, 8 lower.
*/
const TInt KHashTableBits = 10;

/**
 * @brief Class for deflate compressor definition.
 *
 * From RFC 1951:
 * The compressor uses a chained hash table to find duplicated strings,
 * using a hash function that operates on 3-byte sequences.  At any
 * given point during compression, let XYZ be the next 3 input bytes to
 * be examined (not necessarily all different, of course).  First, the
 * compressor examines the hash chain for XYZ.  If the chain is empty,
 * the compressor simply writes out X as a literal byte and advances one
 * byte in the input.  If the hash chain is not empty, indicating that
 * the sequence XYZ (or, if we are unlucky, some other 3 bytes with the
 * same hash function value) has occurred recently, the compressor
 * compares all strings on the XYZ hash chain with the actual input data
 * sequence starting at the current point, and selects the longest
 * match.
 *
 * The compressor searches the hash chains starting with the most recent
 * strings, to favor small distances and thus take advantage of the
 * Huffman encoding. The hash chains are singly linked. There are no
 *  deletions from the hash chains; the algorithm simply discards matches
 *  that are too old.  To avoid a worst-case situation, very long hash
 *  chains are arbitrarily truncated at a certain length, determined by a
 *  run-time parameter.
 *
 *  To improve overall compression, the compressor optionally defers the
 *  selection of matches ("lazy matching"): after a match of length N has
 *  been found, the compressor searches for a longer match starting at
 *  the next input byte.  If it finds a longer match, it truncates the
 *  previous match to a length of one (thus producing a single literal
 *  byte) and then emits the longer match.  Otherwise, it emits the
 *  original match, and, as described above, advances N bytes before
 *  continuing.
 */
class CDeflate : public CBase
    {
    public:

        /**
         * @brief huffman tree node
         */
        class TTreeNode
            {
            public:
                /** actual value of this node */
                TUint16 iBits;
                /** number of bits this tree node uses */
                TUint8 iLength;
            };

    public:

        /**
        * This function creates a new CDeflate object.
        * The function may leave with KErrNoMemory if there is
        * insufficient memory available.
        * 
        * @post compressor is ready to compress.
        * @returns A created CDeflate object.
        */ 
        static CDeflate* NewL(TBool aDynamic,
                              CSigCompDeflateContext* aContext);

        /**
        * This function creates a new CDeflate and places a pointer
        * to it onto the cleanup stack.
        *  
        * @post compressor is ready to compress.
        * @returns Pointer to a created CDeflate object.
        * The function may leave with KErrNoMemory if there is
        * insufficient memory available.
        */ 
        static CDeflate* NewLC(TBool aDynamic,
                               CSigCompDeflateContext* aContext);

        /**
        * Destructor of the object. Destroy object and all its members.
        * 
        */ 
        ~CDeflate();

        /**
        * Compress given data into aOutputBuffer.
        * @param aMessage message to be compressed
        * @param aOutputBuffer compressed message will be placed into
        * this buffer
        */
        void CompressL(const TDesC8& aMessage, CMessageWriter* aMessageWriter);

        /**
        * Reset the compressor.
        */
        void Reset(void);

        /**
        * Reset hash table
        */
        void ResetHashTable(void);

        /**
        * Set the given dictionary
        * @param aPtr pointer to memory containing dictionary
        * @param aSize size of the dictionary, in bytes
        */
        void SetDictionary(const TUint8* aPtr, TInt aSize);

        /**
        * Change the window size.
        * @param aWindowSize new window size.
        */
        void ChangeWindowSizeL(TInt aWindowSize);
    
    private:
        void ConstructL(TBool aDynamic, CSigCompDeflateContext* aContext);

        CDeflate();

        /**
        * Send n bits to a queue buffer.
        * 
        * @param aValue bits value
        * @param aLength number of bits to send,
        */ 
        void SendBitsL(TUint aValue, TUint aLength);

        /**
        * Flush all queued bits
        */ 
        void FlushBitsL();

        /**
        * Put one byte into output buffer.
        * 
        * @param aValue byte to store in output buffer.
        */ 
        void PutByteL(TUint8 aValue);

        /**
        * Encode literal value, using Literal huffman tree
        * 
        * @param aValue literal to encode
        */ 
        void EncodeLiteralL(TInt aValue);

        /**
        * Encode pair of distance and length
        * 
        * @param aDist distance of a match being encoded
        * @param aLength length of the match.
        */ 
        void EncodeDistanceLengthPairL(TUint aDist, TUint aLength);

        /**
        * This method searches matching substrings in encoded message
        * using the hash table.
        * 
        * @param aMatchPos position within current window of found match.
        *                  (if match was found).
        * @param aLength length reference
        *                (if match was found).
        *
        * @post if match was found aOffset and aLength are filled with correct
        *       values
        * @returns ETrue if match was found, EFalse if not,
        */ 
        TBool FindBestMatch(TInt& aMatchPos, TInt& aLength);

        /**
        * Fill window with aBytes characters from given pointer
        * @param aPtr pointer where to fetch data from
        * @param aBytes number of bytes to fetch
        *
        * @returns number of bytes fetched from given address.
        */
        TInt FillWindow(const TUint8* aPtr, TInt aBytes);

        /**
        * Calculate hash from three given characters
        * @param aA first character for hash 
        * @param aB second character for hash 
        * @param aC third character for hash 
        */
        TInt CalcHash(TUint8 aA, TUint8 aB, TUint8 aC) const;

        /**
        * Insert hash calculated from 3 characters at given address
        * to the hash table.
        * @param aPosition position within the iWindow from where 3
        * characters for the hash will be fetched from
        */
        void InsertHash(TInt aPosition);

    private:
        /** output bit buffer, encoded bitstream*/
        TUint iBitBuffer;
        
        /** number of bits currently stored in bit buffer */
        TUint iBitsQueued;
        
        /** size of hash table */
        TInt iHashSize;
        
        /** mask used when calculating hash */
        TUint iHashMask;
        
        /** shift used when calculating hash */
        TUint iHashShift;
        
        /** shift used when calculating hash */
        TUint iHashShift2;
        
        /** the hash table,used to speed up searching of matching substrings */
        CArrayFixFlat<TInt16>* iHashTable;

        /** number of bytes queued */
        TInt iLookAhead;

        /** array of nodes, those are used as linked elements in hash table */
        CArrayFixFlat<TInt16>* iChainNodes;
        
        /** modulo mask for chain nodes, iWindowSize - 1 */
        TInt iChainMask;

        /** output buffer, encoded bitstream is stored there */
        CMessageWriter* iMessageWriter;

        /** determines whenever compression is dynamic or not */
        TBool iDynamic;
        
        /** deflate compression context */
        CSigCompDeflateContext* iDeflateContext;
    };

#endif
