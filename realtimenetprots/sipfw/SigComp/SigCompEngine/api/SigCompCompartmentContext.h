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
* Name        : SigCompCompartmentContext.h
* Part of     : SigComp / compartment
* Interface   : 
* sigcomp compartment external interface
* Version     : 1.0
*
*/



#ifndef MSIGCOMPCOMPARTMENTCONTEXT_H
#define MSIGCOMPCOMPARTMENTCONTEXT_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSigCompCompressionContext;

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
*  Compartment interface.
*  
*  @lib sigcomp
*/
class MSigCompCompartmentContext
    {
    public:
        /**
         * Gets returned feedback from compartment
         * 
         * @return returned feedback
         */
        virtual const TDesC8& ReturnedFeedback() const = 0;

        /**
         * Gets requested feedback from compartment
         * 
         * @return requested feedback
         */
        virtual const TDesC8& RequestedFeedback() const = 0;

        /**
         * Set returned feedback in compartment
         * Internal use only
         * Can leave if system resourcess missing (no memory for egz.)
         *
         * @param aReturnedFeedback a returned feedback
         */
        virtual void SetReturnedFeedbackL(const TDesC8& aReturnedFeedback) = 0;

        /**
         * Set requested feedback in compartment
         * Internal use only
         * Can leave if system resourcess missing (no memory for egz.)
         *
         * @param aRequestedFeedback a requested feedback
         */
        virtual void SetRequestedFeedbackL(const TDesC8& aRequestedFeedback) = 0;

        /**
         * Gets returned parameters from compartment
         * 
         * @return returned parameters
         */
        virtual const TDesC8& ReturnedParameters() const = 0;

        /**
         * Gets compression context from compartment
         * 
         * @return compression context
         */
        virtual const CSigCompCompressionContext*
                                        CompressionContext() const = 0;

        /**
         * Sets compression context in compartment
         * 
         * @param aContext compression context to be saved in compartment.
         * 
         */
        virtual void SetCompressionContext(
                            CSigCompCompressionContext* aContext) = 0;

        /**
         * Gets type of compression (dynamic or not)
         * 
         * @return ETrue for dynamic compression, EFalse for static).
         */
        virtual TBool IsDynamicCompression() const = 0;
        
        /**
         * Gets cycles per bit
         * 
         * @return Cycles per bit
         */
        virtual CSigComp::TCyclesPerBit CyclesPerBit() const = 0;

        /**
         * Gets decompression memory size
         * 
         * @return Decompression memory size
         */
        virtual CSigComp::TMemorySize DecompressionMemorySize() const = 0;

        /**
         * Gets state memory size
         * 
         * @return State memory size
         */
        virtual CSigComp::TMemorySize StateMemorySize() const = 0;
    };

#endif // MSIGCOMPCOMPARTMENTCONTEXT_H
            
// End of File
