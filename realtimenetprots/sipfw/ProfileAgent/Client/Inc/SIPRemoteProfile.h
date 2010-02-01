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
* Name        : sipremoteprofile.h
* Part of     : SIP Profile Client
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef RSIPPROFILE_H
#define RSIPPROFILE_H

//  INCLUDES

#include <e32std.h>
#include "sipprofilecs.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  The class that provides connection to profile server over ITC.
*
*  @lib sipprofilecli.lib
*/
class RSIPProfile: public RSessionBase
	{	
	public:

		/**
		* Default constructor
		*/
		RSIPProfile();

		/**
		* Connect to profile server
		* @return KErrNone if successfull
		*/
		TInt Connect();

		/**
		* Close connection to profile server
		*/
		void Close();

		/**
		* Gets the version number of the server
		* @return version number of server
		*/
		TVersion Version() const;

		/**
		* Send syncronous request to the server
		* @param aArgs, the message argument array 
		* @param aFunction, opcode specifying the requested service 
		* @return KErrNone if successfull
		*/
		TInt Send(TIpcArgs& aArgs, TSipProfileItcFunctions aFunction);

		/**
		* Send asyncronous request to the server, events allowed
		* @param aArgs, the message argument array 
		* @param aStatus, a status of the asynchronous request 
		*/
		void Receive(TIpcArgs& aArgs, TRequestStatus& aStatus);

		/**
		* Cancel asyncronous request to the server, events cancelled
		* @param aPtr, pointer to the message argument array 
		* @param aStatus, a status of the asynchronous request 
		*/
		void CancelReceive();

	private:

		/**
		* Panic this client
		* @param aReason, a panic reason code
		*/
		void Panic(TInt aReason);

	private:

		TBool iConnected;
	};

#endif
