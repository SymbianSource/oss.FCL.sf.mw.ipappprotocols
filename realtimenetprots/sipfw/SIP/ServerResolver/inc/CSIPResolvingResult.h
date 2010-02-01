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
* Name          : CSIPResolvingResult.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPRESOLVINGRESULT_H
#define CSIPRESOLVINGRESULT_H

//INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include <stringpool.h>
#include "MSIPResolvingResult.h"

// CLASS DEFINITION
/**
 * CSIPResolvingResult implements a
 */
class CSIPResolvingResult : public CBase, public MSIPResolvingResult 
	{
	public:// Constructors and destructor

		/**
	 	* Creates a new CSIPResolvingResult instance 
	 	* and pushes it to CleanupStack
	 	* @pre if aForceUDP is ETrue, then aProtocol must be UDP
	 	* @param aAddr the resolved address
	 	* @param aProtocol the resolved protocol
	 	* @param aForceUDP ETrue if protocol must remain UDP.
	 	* @returns a new instance of CSIPResolvingResult
	 	*/
		static CSIPResolvingResult* NewLC( TInetAddr aAddr,
										   RStringF aProtocol,
										   TBool aForceUDP );

		/**
	 	* Destructor.
	 	*/
		~CSIPResolvingResult();
		
	public:// From MSIPResolvingResult

		TInetAddr& Address();
		
		RStringF Protocol();
		
		void SetFailed(TBool aFailed);
		
		TBool Failed();
		
		TBool ForceUDP();

	public:

		TInt SetProtocol( RStringF aProtocol );

	private:// Constructors and destructor

		CSIPResolvingResult( TInetAddr aAddr,
							 RStringF aProtocol,
							 TBool aForceUDP );
		
	private:// Data

		RStringF iProtocol;

		TInetAddr iAddr;

		TBool iFailed;
		
		TBool iForceUDP;
	};

#endif // CSIPRESOLVINGRESULT_H

// End of File
