
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
*
*/



#ifndef _SIPEXSIPSTATEBASE_H_
#define _SIPEXSIPSTATEBASE_H_

//  INCLUDES
#include <e32base.h>

// From sipapi
#include <sipclienttransaction.h>
#include <sipservertransaction.h>
#include <sipresponseelements.h>

// From Codec
#include <siptoheader.h>

// FORWARD DECLARATIONS
class CSIPExSIPEngine;
class CSIPClientTransaction;
class CSIPResponseElements;

// CLASS DECLARATION

/**
* SIP state base; provides function default implementation,
* common error handling and function for getting state name.
*
* Function will leave with KErrTotalLossOfPrecision in case of
* invalid state.
*/
class CSIPExSIPStateBase: public CBase
	{
	public:// Constructors and destructor
		/**
		* Destructor
		*/
		virtual ~CSIPExSIPStateBase();


	public://new functions
	
		/**
		* SendInviteL
		* Sends an INVITE to peer.
		* @param aEngine Reference to SIP Engine.
		* @param aSipUri uri of recipient.
		*/
		virtual void SendInviteL(
			CSIPExSIPEngine& aEngine,
			const TDesC8& aSipUri );
			

		/**
		* CancelInviteL
		* Cancel an INVITE sent previously.
		* @param aEngine Reference to Engine object.
		*/
		virtual void CancelInviteL(
			CSIPExSIPEngine& aEngine );


		/**
		* AcceptInviteL
		* Send 200 (OK) as response to an INVITE
		* received from peer.
		* @param aEngine Reference to Engine object.
		*/
		virtual void AcceptInviteL(
			CSIPExSIPEngine& aEngine );
			
		/**
		* DeclineInviteL
		* Send 486 (Busy Here) as response
		* to an INVITE received from peer.
		* @param aEngine Reference to Engine object.
		*/
		virtual void DeclineInviteL(
			CSIPExSIPEngine& aEngine );
			
			
			
		/**
		* EndSessionL
		* Send BYE in an established session.
		* @param aEngine Reference to Engine object.
		*/
		virtual void EndSessionL(
			CSIPExSIPEngine& aEngine );
		

		/**
		* ResponseReceivedL
		* A SIP response has been received from the network.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		virtual void ResponseReceivedL(
			CSIPExSIPEngine& aEngine,
			CSIPClientTransaction& aTransaction );


		/**
		* AckReceivedL
		* ACK has been received.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		virtual void AckReceivedL(
			CSIPExSIPEngine& aEngine,
			CSIPServerTransaction& aTransaction );


		/**
		* ByeReceivedL
		* BYE has been received.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		virtual void ByeReceivedL(
			CSIPExSIPEngine& aEngine,
			CSIPServerTransaction& aTransaction );


		/**
		* CancelReceivedL
		* CANCEL has been received.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		virtual void CancelReceivedL(
			CSIPExSIPEngine& aEngine,
			CSIPClientTransaction& aTransaction );


		/**
		* InviteReceivedL
		* INVITE has been received.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		* The ownership is transferred.
		*/
		virtual void InviteReceivedL(
			CSIPExSIPEngine& aEngine,
			CSIPServerTransaction* aTransaction );



		/**
		* An asynchronous error has occured related to a periodical refresh 
		* that relates to a registration.
		* @param aEngine Reference to Engine object.
		* @param aError error code
		* @param aSIPRegistration associated registration
		*/
		virtual void ErrorOccured(
			CSIPExSIPEngine& aEngine,
			TInt aError );


	protected:
	    /**
		* C++ default constructor.
		* @param aName a state name
		*/
		CSIPExSIPStateBase();


	private://data members

	};

#endif	// _SIPEXSIPSTATEBASE_H_

