/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MSIPSecAlgorithm.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __MSIPSECALGORITHM_H__
#define __MSIPSECALGORITHM_H__

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class TSIPSecDigestCtxSetup;
class TSIPSecDigestCtxProcess;

// CLASS DECLARATION
/**
 * @brief Description
 * 
 * Description 
 */
class MSIPSecAlgorithm
    {
public: // New pure virtual functions

    /**
    * Returns the name of the algorithm
    * @return Algorithm name
    */
    virtual RStringF AlgorithmName() const = 0;

    /**
    * Processes the challenge based on a SIP response
    * @param aContext Digest context
    * @param aAskCredentials OUT: ETrue If the credentials for the challenge(s)
    *		 present in the current SIP response, must be asked now.
    *		 EFalse If there is no need to ask the credentials, and maybe they
    *	     are already being asked for due to some earlier SIP response.
    * @return ETrue, if credentials must be obtained
    */
    virtual TBool ProcessResponseL( TSIPSecDigestCtxSetup& aContext,
    					    		TBool& aAskCredentials ) const = 0;

    /**
    * Processes the challenge based on a SIP request
    * @param aContext Digest context
    */
    virtual void ProcessRequestL( TSIPSecDigestCtxProcess& aContext ) const = 0;

    /**
    * Generates CNonce
    * @param aContext Digest context
    */
    virtual void GenerateCNonceL( TSIPSecDigestCtxSetup& aContext ) const = 0;
    };
	
#endif //end of __MSIPSECALGORITHM_H__

// End of File
