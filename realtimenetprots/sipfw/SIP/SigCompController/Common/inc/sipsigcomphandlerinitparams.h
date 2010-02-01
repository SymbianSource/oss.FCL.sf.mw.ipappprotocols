/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipsigcomphandlerinitparams.h
* Part of     : SIP SigComp Controller
* Interface   : 
* part of sip signaling compression plugin interface
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef __SIPSIGCOMPHANDLERINITPARAMS_H__
#define __SIPSIGCOMPHANDLERINITPARAMS_H__

//  INCLUDES
#include <e32base.h>


//FORWARD DECLARATIONS
class MLocalName;

// CLASS DECLARATION

/**
*  Contains parameters needed for profile plug-ins.
*  Contains reference to MLocalName
*
*  @lib none
*  @since 2.0
*/
class TSIPSigCompHandlerInitParams
	{

	public:  // Constructors and destructor

        /**
        * Constructor
		* @param aSIP a handle to SIP server
		* @param aSIPProfileObserver a sip profile observer
        */

		TSIPSigCompHandlerInitParams
			(MLocalName& aLocalName);



	public: // Data


		/**
		* MLocalName
		*/

		MLocalName& iLocalName;

	};

#include "sipsigcomphandlerinitparams.inl"

#endif // __SIPSIGCOMPHANDLERINITPARAMS_H__
