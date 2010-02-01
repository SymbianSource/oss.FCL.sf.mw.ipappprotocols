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
* Name        : SigCompDeflate.h
* Part of     : CDeflateComp
* Interface   : 
* compression context
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef SIGCOMPCOMPDEFLATECONTEXT_H
#define SIGCOMPCOMPDEFLATECONTEXT_H

#include "SigCompCompressionContext.h"

class TStateItem;

/*
* deflate compression context saved in compartment beetwen
* CompressMessageL() calls.
*/

class CSigCompDeflateContext: public CSigCompCompressionContext
    {
    public:
        CSigCompDeflateContext();

        virtual ~CSigCompDeflateContext();

        /** 
         * save the window size for future use.
         *
         * @param aWindowSize window size
         */
        void SetWindowSize(TInt aWindowSize);

        /** 
         * return previously saved window size
         *
         * @returns window size
         */
        TInt WindowSize();

        /** 
         * save the current position for future use.
         *
         * @param aCurrentPosition current position
         */
        void SetCurrentPosition(TInt aCurrentPosition);

        /** 
         * return previously saved 'current position'
         *
         * @returns saved 'current position'
         */
        TInt CurrentPosition();

        /** 
         * save the window for future use. ownership is transfered
         *
         * @param aWindow window pointer
         */
        void SetWindow(CArrayFixFlat<TUint8>* aWindow);

        /** 
         * return saved window pointer.
         *
         * @returns window size
         */
        CArrayFixFlat<TUint8>* Window();

        /**
        * set dictionary state item
        *
        * @params aStateItem to be set
        * @params aStateItemLength length of state item
        */
        void SetStateItem(TStateItem* aStateItem, TInt aStateItemLength);

        /**
        * get dictionary state item
        *
        * @returns state item, may be null.
        */
        TStateItem* StateItem();

        /**
        * get dictionary state item length
        *
        * @returns state item length
        */
        TInt StateItemLength();

        /** 
         * set bytecode confirmation
         *
         * @param aBytecodeStateConfirmed is bytecode state confirmed
         *
         */
		void SetBytecodeStateConfirmed(TBool aBytecodeStateConfirmed);

        /** 
         * get bytecode confirmation
         *
         * @returns is bytecode state confirmed
         *
         */
		TBool BytecodeStateConfirmed() const;

        /** 
         * set bytecode state hash
         *
         * @param aHash bytecode state hash
         *
         */
		void SetBytecodeHash(const TUint8* aHash);

        /** 
         * get bytecode state hash
         *
         * @returns bytecode state hash
         *
         */
		const TUint8* BytecodeHash() const;

        /** 
         * set circular buffer confirmation
         *
         * @param aStateConfirmed is circular buffer state confirmed
         *
         */
        void SetCBStateConfirmed(TBool aStateConfirmed);

        /** 
         * get circular buffer confirmation
         *
         * @returns is circular buffer state confirmed
         *
         */
		TBool CBStateConfirmed() const;

        /** 
         * set circular buffer state hash
         *
         * @param aHash circular buffer state hash
         *
         */
        void SetCBStateHash(const TUint8* aHash);

        /** 
         * get circular buffer state hash
         *
         * @returns circular buffer state hash
         *
         */
		const TUint8* CBStateHash() const;

        /** 
         * set circular buffer state length
         *
         * @param aLength circular buffer state length
         *
         */
        void SetCBStateLength(TInt aLength);

        /** 
         * get circular buffer state length
         *
         * @returns circular buffer state length
         *
         */
        TInt CBStateLength() const;

        /**
         * Set iUploadState flag
         *
         * @param aUploadState value to be set
         *
         */
        void SetUploadState(TBool aUploadState);

        /**
         * get iUploadState
         *
         * @returns iUploadstate field
         *
         */
        TBool UploadState();

        /**
        * returns ETrue if user state was already uploaded to the other side,
        * EFalse if it was not.
        *
        * @returns iStateUploaded.
        */
        TBool StateUploaded();

        /**
        * set uploaded state flag
        *
        * @params aState state to be set
        */
        void SetStateUploaded(TBool aState);

        /** 
         * set remote state memory size
         *
         * @param aSms remote state memory size
         *
         */
		void SetSms(TInt aSms);

        /** 
         * get remote state memory size
         *
         * @returns remote state memory size
         *
         */
		TInt Sms() const;

        /** 
         * set decompression state memory size
         *
         * @param aDms remote decompression memory size
         *
         */
		void SetDms(TInt aDms);

        /** 
         * get remote decompression memory size
         *
         * @returns remote decompression memory size
         *
         */
		TInt Dms() const;


        /** 
         * set local udvm memory size
         *
         * @param aUdvmSize local udvm memory size
         *
         */
        void SetUdvmSize(TInt aUdvmSize);

        /** 
         * get local udvm memory size
         *
         * @returns local udvm memory size
         *
         */
   	    TInt UdvmSize() const;

        /** 
         * set local circular buffer size
         *
         * @param aCBSize local circular buffer size
         *
         */
        void SetCBSize(TInt aCBSize);

        /** 
         * get local circular buffer size
         *
         * @returns local circular buffer size
         *
         */
   	    TInt CBSize() const;

    private:

        /** should the state given by user be uploaded? */
        TBool iUploadState;

        /** dictionary state item */
        TStateItem* iStateItem;

        /** length of string part of dictionary state item */
        TInt iStateItemLength;


        /** current position within window, points to currently encoded char */
        TInt iCurrentPosition;

        /** size of window */
        TInt iWindowSize;

        /** compression window, dynamic dictionary*/
        CArrayFixFlat<TUint8>* iWindow;
        
        /** hash id, used to determine whenever we lost sync or not*/
        TUint8 iCBStateHash[6];
        TInt iCBStateLength;

        /** hash id, used to determine whenever we lost sync or not*/
        TUint8 iBytecodeStateHash[6];

        /** circular buffer state confirmed */
        TBool iCBStateConfirmed;

        /** bytecode state confirmed */
        TBool iBytecodeStateConfirmed;

        /** remote state memory size */
		TInt iStateMemorySize;

        /** remote decompression memory size */
		TInt iDecompressionMemorySize;

        /** local udvm memory size */
        TInt iUdvmSize;

        /** local circular buffer size */
        TInt iCBSize;

    };

#endif
