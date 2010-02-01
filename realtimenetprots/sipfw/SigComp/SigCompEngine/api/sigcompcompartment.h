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
* Name        : SigCompCompartment.h
* Part of     : SigComp / compartment
* Interface   : 
* sigcomp compartment class.
* Version     : 1.0
*
*/



#ifndef CSIGCOMPCOMPARTMENT_H
#define CSIGCOMPCOMPARTMENT_H

//  INCLUDES
#include <e32base.h>
#include <sigcomp.h>
#include <sigcompcompartmentcontext.h>

// FORWARD DECLARATIONS
class CSigCompCompartmentStatesHolder;

// CLASS DECLARATION 
/**
* @publishedPartner
* @released
* 
* @class CSigCompCompartment sigcompcompartment.h "sigcompcompartment.h"
*
* This class is used for compressing text-based protocol messages 
* while communicating with a particular peer endpoint.
*
*/
class CSigCompCompartment : public CBase, public MSigCompCompartmentContext
    {
    public:
        /**
         * Destructor
         */

        IMPORT_C ~CSigCompCompartment();


        /**
         * Constructs an object; leaves on failure
         *
         * @param aSigComp the instance of SigComp under which the
         *           compartments are created. The compartment
         *           will use state memory size defined while
         *           creating SigComp instance.
         * @param aDynamicCompression if set to ETrue, the compartment
         *                  will use dynamic compression.
         *       
         * @return new instance
         * @leave KErrNotSupported if the dynamic compression was 
         * 		  required but the algorithm does not support it.
         */

        IMPORT_C static CSigCompCompartment* NewL(const CSigComp& aSigComp,
                        TBool aDynamicCompression = EFalse);



        /**
         * Constructs an object; leaves on failure. 
         *
         * @param aSigComp the instance of SigComp under which the compartments
         *                 are created.
         *
         * @param aStateMemorySize number of bytes offered for a compartment
         *                         for the state creation. This overrides state
         *                         memory size defined while creating SigComp
         *                         instance.
         * @param aDynamicCompression if set to ETrue, the compartment will use
         *                            dynamic compression.
         *
         * @return new instance
         * @leave KErrNotSupported if the compression algorithm does not 
         *						   support dynamic compression.
         */

        IMPORT_C static CSigCompCompartment* NewL(const CSigComp& aSigComp,
                        CSigComp::TMemorySize aStateMemorySize,
                          TBool aDynamicCompression = EFalse);



        /**
         * Constructs an object and adds the pointer to the cleanup stack; 
         * leaves on failure. 
         *
         * @param aSigComp the instance of SigComp under which the compartments
         *                 are created. The compartment will use state memory
         *                 size defined while creating SigComp instance.
         * @param aDynamicCompression if set to ETrue, the compartment will
         *                            use dynamic compression.
         *       
         * @return new instance
         * @leave KErrNotSupported if the compression algorithm does not 
         *						   support dynamic compression.
         */

        IMPORT_C static CSigCompCompartment* NewLC(const CSigComp& aSigComp,
                               TBool aDynamicCompression = EFalse);


        /**
         * Constructs an object and adds the pointer to the cleanup stack; 
         * leaves on failure.
         *
         * @param aSigComp the instance of SigComp under which the compartments
         *                 are created.
         *
         * @param aStateMemorySize number of bytes offered for a compartment
         *                         for the state creation. This overrides state
         *                         memory size defined while creating SigComp
         *                         instance.
         * @param aDynamicCompression if set to ETrue, the compartment will use
         *                            dynamic compression.
         *
         * @return new instance
         * @leave KErrNotSupported if the compression algorithm does not
         *						   support dynamic compression.
         */

        IMPORT_C static CSigCompCompartment* NewLC(const CSigComp& aSigComp,
                               CSigComp::TMemorySize aStateMemorySize,
                               TBool aDynamicCompression = EFalse);


        /**
         * Compress the message using the algorithm provided while
         * creating the instance of the CSigComp class.
         * 
         * @param aMessage a message to compress.
         * 
         * @return Compressed message, ownership is transfered to the client.
         * @leave ECompressionError if the compression fails.
         */

        IMPORT_C CBufBase* CompressL(const TDesC8& aMessage,
                                     TBool aForStreamBasedProtocol = EFalse);


        /**
         * Get state memory size
         * Internal use only
         *
         * @return state memory size
         */

        CSigComp::TMemorySize StateMemorySize() const;


        /**
         * Set returned feedback in compartment
         * Internal use only
         * Can leave if system resourcess missing (no memory for egz.)
         *
         * @param aReturnedFeedback a returned feedback
         */

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#if defined(SIGCOMP_DEBUG)
        IMPORT_C
#endif
#endif
        void SetReturnedFeedbackL(const TDesC8& aReturnedFeedback);


        /**
         * Set requested feedback in compartment
         * Internal use only
         * Can leave if system resourcess missing (no memory for egz.)
         *
         * @param aRequestedFeedback a requested feedback
         */

        void SetRequestedFeedbackL(const TDesC8& aRequestedFeedback);


        /**
         * Set returned parameters in compartment
         * Internal use only
         * Can leave if system resourcess missing (no memory for egz.)
         *
         * @param aReturnedParameters a returned parameters
         */

        void SetReturnedParametersL(const TDesC8& aReturnedParameters);

        /**
         * Get returned feedback
         * Internal use only
         *
         * @return returned feedback
         */
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#if defined(SIGCOMP_DEBUG)
        IMPORT_C
#endif
#endif
        const TDesC8& ReturnedFeedback() const;

        /**
         * Get requested feedback
         * Internal use only
         *
         * @return requested feedback
         */
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#if defined(SIGCOMP_DEBUG)
        IMPORT_C
#endif
#endif
        const TDesC8& RequestedFeedback() const;

        /**
         * Get returned parameters
         * Internal use only
         *
         * @return returned parameters
         */
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#if defined(SIGCOMP_DEBUG)
        IMPORT_C
#endif
#endif
        const TDesC8& ReturnedParameters() const;

        /**
         * Get compressor context
         * Internal use only
         *
         * @return compressor context
         */
        const CSigCompCompressionContext* CompressionContext() const;

        /**
         * Set compressor context
         * Internal use only
         *
         * @param aPrivateData compressor context
         */
        void SetCompressionContext(CSigCompCompressionContext* aContext);

        /**
         * Get states holder
         * Internal use only
         *
         * @return states holder
         */
        const CSigCompCompartmentStatesHolder* StatesHolder() const;

       /**
        * Get type of compression (dynamic or not)
        * 
        * @return ETrue for dynamic compression, EFalse for static).
        */
        TBool IsDynamicCompression() const;

       /**
        * Get cycles per bit
        * 
        * @return Cycles per bit
        */
        CSigComp::TCyclesPerBit CyclesPerBit() const;

       /**
        * Get decompression memory size
        * 
        * @return Decompression memory size
        */
        CSigComp::TMemorySize DecompressionMemorySize() const;

    private:

        CSigCompCompartment();

        void ConstructL(const CSigComp& aSigComp,
                        TBool aDynamicCompression = EFalse);

        void ConstructL(const CSigComp& aSigComp,
                        CSigComp::TMemorySize aStateMemorySize,
                        TBool aDynamicCompression = EFalse);

    private:

        /** sigcomp instance */
        const CSigComp* iSigComp;

        /** is dynamic compression */
        TBool iDynamicCompression;

        /** state memory size */
        CSigComp::TMemorySize iStateMemorySize;

        /** state manager */
        CStateMgr* iStateMgr;

        /** returned feedback */
        HBufC8* iReturnedFeedback;

        /** requested feedback */
        HBufC8* iRequestedFeedback;

        /** returned parameters */
        HBufC8* iReturnedParameters;

        /** compression context */
        CSigCompCompressionContext* iCompressionContext;

        /** states holder */
        CSigCompCompartmentStatesHolder* iStatesHolder;
    };

#endif // CSIGCOMPCOMPARTMENT_H
