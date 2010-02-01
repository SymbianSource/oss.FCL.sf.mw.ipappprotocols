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
* Name        : SigCompCompressionContext.h
* Part of     : SigComp / compressor
* Interface   : 
* compression context
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef SIGCOMPCOMPRESSIONCONTEXT_H
#define SIGCOMPCOMPRESSIONCONTEXT_H

#include <e32base.h>

/**
*  Abstract class for holding compressor specific data in compartment.
*/

class CSigCompCompressionContext: public CBase
    {
    public:

        /**
        * Sets compressor context. Ownership is transferred.
        * 
        * @param aContext private context to be saved
        */
        IMPORT_C void SetContext(CBufBase* aContext);

       /**
        * Gets saved context
        * 
        * @return saved context
        */
        IMPORT_C const CBufBase* Context() const;

        IMPORT_C virtual ~CSigCompCompressionContext();

    protected:
        IMPORT_C CSigCompCompressionContext();

    private:
        /** compressor private data saved in this context */
        CBufBase* iContext;
    };

#endif
 
