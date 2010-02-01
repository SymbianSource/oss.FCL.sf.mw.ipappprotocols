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
* Name        : sipprofilecssession.h
* Part of     : SIP Profile Server
* Interface   : private
* The class for ITC server session
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILECSSESSION_H
#define CSIPPROFILECSSESSION_H

//  INCLUDES

#include "sipprofilecs.h"
#include "sipextendedconcreteprofileobserver.h"
#include "SipProfileCSSessionHelper.h"

// FORWARD DECLARATIONS

class CSIPProfileCSServer;
class CSIPProfileServerCore;
class CSIPConcreteProfile;
class TSIPProfileSlots;

// CLASS DECLARATION

/**
 * The class providing ITC server session functionality.
 */
class CSIPProfileCSSession :
	public CSession2,
	public MSIPExtendedConcreteProfileObserver
	{
	public:
    
	    /**
        * Two-phased constructor.
		* @param aServer server of the session
		* @param aCore reference to profile server core
		* @return new instance
        */
        static CSIPProfileCSSession* NewL(CSIPProfileCSServer& aServer,
								CSIPProfileServerCore& aCore);

	    /**
        * Two-phased constructor.
		* @param aServer server of the session
		* @param aCore reference to profile server core
		* @return new instance
        */
		static CSIPProfileCSSession* NewLC(CSIPProfileCSServer& aServer,
								CSIPProfileServerCore& aCore);

	    /**
        * Destructor.
        */
		~CSIPProfileCSSession();

	public: // New

	    /**
        * Client is paniced
		* @param aError panic error code
        */
		void PanicClient(TInt aError);

	public: // From base class

	    /**
        * Service request called by client over ITC
		* @param aMessage handle to ITC communication
        */
		void ServiceL(const RMessage2& aMessage);

	public: // From MSIPExtendedConcreteProfileObserver

		void RegistrationStatusChangedL(
			TUint32 aProfileId, 
			TInt aStatus,
			TUint32 aStatusId);

	    void ErrorOccurredL(
			TUint32 aProfileId, 
			TInt aStatus,
			TInt aError);

		void AddedL(TUint32 aProfileId);

		void RemovedL(TUint32 aProfileId);

		void UpdatedL(TUint32 aProfileId, TUint aSize);

		TBool IapAvailableL(TUint32 aProfileId,
						    TUint32 aSnapId,
						    TUint32 aIapId);

		void MigrationStartedL(TUint32 aProfileId,
						   	   TUint32 aSnapId,
						   	   TUint32 aIapId);

		void MigrationCompletedL(TUint32 aProfileId,
						   	     TUint32 aSnapId,
						   	     TUint32 aIapId);

		TInt AlrError(TInt aError,
	                  TUint32 aProfileId,
	                  TUint32 aSnapId,
	                  TUint32 aIapId);

	private:

	    /**
        * Constructor.
		* @param aServer server of the session
		* @param aCore reference to profile server core
        */
        CSIPProfileCSSession(CSIPProfileCSServer& aServer,
				   			 CSIPProfileServerCore& aCore);

	    /**
        * Symbian 2nd phase constructor.
        */
		void ConstructL();

	    /**
        * Deserializes profile from client
		* @param aProfile serialized profile
		* @return SIP profile instance
        */
		CSIPConcreteProfile* InternalizeProfileLC(const TDesC8& aProfile);

	    /**
        * Serializes object from server to client
		* @param aElements object to be serialized
		* @return serialized object
        */
		template<class T> CBufFlat* ExternalizeLC(const T& aElements);

		/**
        * Sends an (asyncronous) event to client.
		* @param aProfileId Profile id event is related to
		* @param aEventId Event id
		* @param aSlot Additional data related to event
		* @param aSlot2 Additional data related to event
		* @param aError Error related to the event
		* @return KErrNone if successful, otherwise a system-wide error code
        */
		TInt SendEvent(TUint aProfileId,
					    TUint aEventId,
					    TInt aSlot = 0,
					    TInt aSlot2 = 0,
					    TInt aError = KErrNone);

		/**
        * Sends an (asyncronous) event to client.
		* @param aProfileId Profile id event is related to
		* @param aEventId Event id
		* @param aSlot Additional data related to event
		* @param aSlot2 Additional data related to event
		* @param aError Error related to the event
        */
		void SendEventL(TUint aProfileId,
					    TUint aEventId,
					    TInt aSlot = 0,
					    TInt aSlot2 = 0,
					    TInt aError = KErrNone);

	    /**
        * Gets size of profile plugins. Value 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void ProfilePluginsSizeL(const RMessage2& aMessage);

	    /**
        * Gets profile plugins containing plugin types. Data 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void ProfilePluginsL(const RMessage2& aMessage);

	    /**
        * Add profile from client to server. Profile id 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void AddProfileL(const RMessage2& aMessage);

	    /**
        * Update profile from client to server. 
		* @param aMessage handle for ITC communication
        */
		void UpdateProfileL(const RMessage2& aMessage);

	    /**
        * Remove profile from server. 
		* @param aMessage handle for ITC communication
        */
		void RemoveProfileL(const RMessage2& aMessage);

	    /**
        * Enables profile for use of client 
		* @param aMessage handle for ITC communication
        */
		void EnableL(const RMessage2& aMessage);

	    /**
        * Disables profile from use of client 
		* @param aMessage handle for ITC communication
        */
		void DisableL(const RMessage2& aMessage);
		
		/**
		* Force disables profile from use of client 
		* @param aMessage handle for ITC communication
		*/
		void ForceDisableL(const RMessage2& aMessage);

	    /**
        * Gets number of clients using profile. Value 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void UsageL(const RMessage2& aMessage);

	    /**
        * Gets size of profile. Value 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void SipProfileSizeL(const RMessage2& aMessage);

	    /**
        * Gets size and id of default profile. Values 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void SipProfileSizeDefaultL(const RMessage2& aMessage);

	    /**
        * Gets profile. Data returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void SipProfileL(const RMessage2& aMessage);

	    /**
        * Refresh previously got profile afted update. Data 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void SipProfileRefreshL(const RMessage2& aMessage);

	    /**
        * Gets size of all profiles. Values 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void SipProfilesSizeL(const RMessage2& aMessage);

	    /**
        * Gets all profiles. Data
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void SipProfilesL(const RMessage2& aMessage);

	    /**
        * Gets size of all profiles with matching AOR. Value 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void SipProfilesSizeByAORL(const RMessage2& aMessage);

	    /**
        * Gets all profiles with matching AOR. Data
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void SipProfilesByAORL(const RMessage2& aMessage);

	    /**
        * Gets size of all profiles with matching type. Value 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void SipProfilesSizeByTypeL(const RMessage2& aMessage);

	    /**
        * Gets all profiles with matching type. Data
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void SipProfilesByTypeL(const RMessage2& aMessage);

	    /**
        * Removes client as a observer for profile. Client
		* has deleted the instance of the profile
		* @param aMessage handle for ITC communication
        */
		void DeleteSipProfileL(const RMessage2& aMessage);
	
	    /**
        * Stores ITC message handle for asyncronous events to client
		* @param aMessage handle for asyncronous ITC communication
        */
		void ClientReadyToReceiveL(const RMessage2& aMessage);

	    /**
        * Clears ITC message handle for asyncronous events.
        */
		void CancelClientReceiveL();

	    /**
        * Get the size of profile with default values
		* of given type
		* @param aMessage handle for ITC communication
        */
		void CreateProfileSizeL(const RMessage2& aMessage);

	    /**
        * Get the profile with default values
		* of given type
		* @param aMessage handle for ITC communication
        */
		void CreateProfileL(const RMessage2& aMessage);

	    /**
        * Gets size of negotiated security mechanism. Value 
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void NegotiatedSecuritySizeL(const RMessage2& aMessage);

	    /**
        * Negotiated security mechanism. Data
		* returned syncronously to client over ITC.
		* @param aMessage handle for ITC communication
        */
		void NegotiatedSecurityL(const RMessage2& aMessage);

	    /**
		* @param aMessage handle for ITC communication
        */
		void RegisteredAORsSizeL(const RMessage2& aMessage);

	    /**
		* @param aMessage handle for ITC communication
        */
		void RegisteredAORsL(const RMessage2& aMessage);

	    /**
        * Writes profile to client
		* @param aMessage handle to client
		* @param aProfile profile to be written
        */
		void WriteProfileL(const RMessage2& aMessage, 
			CSIPConcreteProfile* aProfile);
					
		/**
		* Allows the migration from the old
		* IAP to the new one.
		* @param aMessage handle for ITC communication
		*/
		void AllowMigrationL(const RMessage2& aMessage);
		
		/**
		* Disallows the migration from the 
		* old IAP to the new one.
		* @param aMessage handle for ITC communication
		*/
		void DisallowMigrationL(const RMessage2& aMessage);
		
		/**
        * Refreshes the list of available IAPs for a profile.
		* @param aMessage handle for ITC communication
		*/
		void RefreshIapAvailabilityL(const RMessage2& aMessage);

		void SetProfileSlots(TSIPProfileSlots& aSlots,
						     TUint aProfileId,
  						     TUint aEventId,
						     TInt aSlot,
						     TInt aSlot2,
						     TInt aError) const;
		/**
		* Cleans proxy- and registrar server passwords from the profile
		* @param aProfile Original profile
		* @return Copy of the original profile without passwords, 
		* 		  the ownership is transferred
		*/				     
		CSIPConcreteProfile* CloneProfileWithoutPasswordsLC(
			const CSIPConcreteProfile* aProfile);

	private:

		CSIPProfileCSServer&   iProfileServer;
		CSIPProfileServerCore& iCore;
		RMessage2 iEventMessage;
		TBool iClientReadyToReceive;
		TBool iSessionTerminated;
		RArray<TSIPProfileSlots> iEventQueue;
		CSIPProfileCSSessionHelper iHelper;
		TBool iALRObserverExists;

	private: //For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPProfileCSSessionTest;
#endif
	};

#endif
