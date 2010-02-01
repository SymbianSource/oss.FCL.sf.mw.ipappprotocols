/*
* Copyright (c) 2003 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Simple pure-inline class for wrapping the validation function
*                and its parameter
*
*/



#ifndef T_GS_SIP_VALIDATOR_WRAPPER_H
#define T_GS_SIP_VALIDATOR_WRAPPER_H

//  INCLUDES
#include <e32std.h>

/**
*  TGSSIPValidatorWrapper class
*  @since 3.0
*  Wraps the validation method in a class
*/
class TGSSIPValidatorWrapper
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        * @param aFunc Validation method
        * @param aPtr  Parameter that is passed to validation method
        */
        inline TGSSIPValidatorWrapper( 
            TBool (*aFunctionL)( TAny* aPtr ), TAny* aPtr );
        
    public: // New functions
        
        /**
        * Calls validation method
        * @return ETrue = Data is ok, otherwise not
        */
        inline TBool ValidateL();

    public:     // Data
        
        // Validation method
        TBool (*iFunctionL)( TAny* aPtr );
        // Ptr passed to the method
	    TAny* iPtr;  
    };

// INLINE METHODS

// Constructor
inline TGSSIPValidatorWrapper::TGSSIPValidatorWrapper(
    TBool (*aFunctionL)( TAny* aPtr ), TAny* aPtr ) :
    iFunctionL( aFunctionL ),
    iPtr( aPtr )
    {}

// Runs the validation method and returns the result
inline TBool TGSSIPValidatorWrapper::ValidateL()
    { 
    return (*iFunctionL)( iPtr );
    }

#endif      // T_GS_SIP_VALIDATOR_WRAPPER_H
            
// End of File
