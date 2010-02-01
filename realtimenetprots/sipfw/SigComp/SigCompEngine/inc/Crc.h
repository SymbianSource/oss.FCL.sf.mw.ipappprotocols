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
* Name        : Crc.h
* Part of     : SigComp / misc
* Interface   : 
* Implementation of the CRC (PPP Frame Check Sequence)
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef CRC_H
#define CRC_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
*  @brief This class mplements Fast Frame Check computation,
*         as described in RFC 1662, APPENDIX C.2
*
*  @lib sigcomp
*/

class Crc
    {

    public:

        enum
            {
            /** initial value of crc sum */
            KFcsInit = 0xffff
            };

        /**
         * Calculate crc sum.
         *
         * @param aBuffer buffer with input data
         * @param aLength length of input data
         * @param aFcs initial value of sum
         *
         * @returns crc sum
         */

        static TUint16 Calc(const TUint8* aBuffer,
                            TUint aLength,
                            TUint16 aFcs = KFcsInit);

    };

#endif
            
// End of File
