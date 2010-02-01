
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// INCLUDE FILES
//



#include "SIPExSIPEngine.h"
#include "SIPExSIPStateBase.h"

#include <uri8.h>
#include <sipstrings.h>
#include <sipstrconsts.h>

// CONSTANTS

_LIT8( KSdpNoInfoDesC8, "-" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CSIPExSIPEngine* CSIPExSIPEngine::NewL( 
    TUid aAppUid,
	MSIPExSIPEngineObserver* aObserver )
	{
	CSIPExSIPEngine* self = new( ELeave ) CSIPExSIPEngine();
	CleanupStack::PushL( self );
	self->ConstructL( aAppUid, aObserver );
	CleanupStack::Pop( self );
	
	return self;
	}


// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CSIPExSIPEngine::~CSIPExSIPEngine()
	{
	SdpCodecStringPool::Close();
	
	delete iIdle;
	delete iClientEstablishing;
	delete iClientOffering;
	delete iServerOffering;
	delete iServerEstablishing;
	delete iEstablished;
	delete iTerminating;

	delete iClientTx;
	delete iServerTx;

	delete iDialogAssoc;

	delete iConnection;
	delete iProfile;
	delete iProfileRegistry;
	delete iSIP;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::CSIPExSIPEngine
// C++ default constructor.
// -----------------------------------------------------------------------------
CSIPExSIPEngine::CSIPExSIPEngine()
	{
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ConstructL
// Symbian 2nd phase constructor. Can leave.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ConstructL( 
    TUid aAppUid,
	MSIPExSIPEngineObserver* aObserver )
	{
	// Make note of the observer class.
	iObserver = aObserver;

	// Create instances of the CSIP and Profile Registry classes
	iSIP = CSIP::NewL( aAppUid, *this );
	iProfileRegistry = CSIPProfileRegistry::NewL( *iSIP, *this );

	// Open SDP Codec String Pool
	StringPoolL();

	// Set our local address
	const TUint32 KInetAddr = INET_ADDR(127,0,0,1);
	iLocalAddr.SetAddress( KInetAddr );

	// Set up the state machine
	// Create instances of the state classes.
	iIdle = CSIPExSIPIdleState::NewL();
	iClientEstablishing = CSIPExSIPClientEstablishingState::NewL();
	iClientOffering = CSIPExSIPClientOfferingState::NewL();
	iServerOffering = CSIPExSIPServerOfferingState::NewL();
	iServerEstablishing = CSIPExSIPServerEstablishingState::NewL();
	iEstablished = CSIPExSIPEstablishedState::NewL();
	iTerminating = CSIPExSIPTerminatingState::NewL();

	// Create the links for state transitions.
	iIdle->LinkStates( *iClientEstablishing, *iServerOffering );
	iClientEstablishing->LinkStates( *iIdle, *iClientOffering, *iEstablished );
	iClientOffering->LinkStates( *iIdle, *iEstablished, *iIdle );
	iServerOffering->LinkStates( *iServerEstablishing, *iIdle );
	iServerEstablishing->LinkStates( *iEstablished, *iIdle );
	iEstablished->LinkStates( *iTerminating, *iIdle );
	iTerminating->LinkStates( *iIdle );

	// Set current state data
	iCurrentState = iIdle;
	iConnState = CSIPConnection::EInactive;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::EnableProfileL
// First get the default profile from the registry, then, if it is
// an IETF profile, enable it.
// If non-IETF profile is default, call error callback with KErrNotSupported
// and leave
// Not part of the state machine.
// -----------------------------------------------------------------------------
EXPORT_C TBool CSIPExSIPEngine::EnableProfileL()
	{
	// Check for existing profile
	if ( iProfile )
		{
		delete iProfile;
		iProfile = NULL;
		}
    TBool registered( EFalse );
    
    // Leaves with KErrNotFound if default profile is not found
	iProfile = iProfileRegistry->DefaultProfileL();

    // Safety check that DefaultProfile() didn't return NULL pointer.
    if ( !iProfile )
        {
        iObserver->ProfileError( KErrNotFound );
        _LIT8( KProfileError, "Profile not found. Define a profile:");
        iObserver->WriteLog(KProfileError());
		User::Leave( KErrNotFound );
        }
    // Leaves if profile type is not EInternet
    else if ( iProfile->Type().iSIPProfileClass != TSIPProfileTypeInfo::EInternet )
		{
		delete iProfile;
		iProfile = NULL;
		iObserver->ProfileError( KErrNotSupported );
		User::Leave( KErrNotSupported );
		}
	else
		{
		const TDesC8* aor = NULL;
    	iProfile->GetParameter( KSIPUserAor, aor );  	
    	iObserver->WriteLog( *aor );
		iProfileRegistry->EnableL( *iProfile, *this );
		
		// check whether profile was immediately set to registered state
		iProfile->GetParameter( KSIPProfileRegistered, registered );
		}
		
    return registered;
	}
		

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::DisableProfileL
// Disable the profile given as a parameter.
// Not part of the state machine.
// -----------------------------------------------------------------------------
EXPORT_C void CSIPExSIPEngine::DisableProfileL()
	{
	if ( iProfile )
		{
		iProfileRegistry->Disable( *iProfile );
		delete iProfile;
		iProfile = NULL;		
		}
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::SendInviteL
// Send INVITE to the remote peer given as a parameter.
// -----------------------------------------------------------------------------
EXPORT_C void CSIPExSIPEngine::SendInviteL( const TDesC8& aSipUri )
	{
	iCurrentState->SendInviteL( *this, aSipUri );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::CancelInvite
// CANCEL a previously sent INVITE.
// -----------------------------------------------------------------------------
EXPORT_C void CSIPExSIPEngine::CancelInviteL()
	{
	iCurrentState->CancelInviteL( *this );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::AcceptInvite
// Accept a received INVITE with 200 (OK).
// -----------------------------------------------------------------------------
EXPORT_C void CSIPExSIPEngine::AcceptInviteL(const TInetAddr& aIPAddr )
	{
	iLocalAddr = aIPAddr;
	iCurrentState->AcceptInviteL( *this );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::DeclineInvite
// Decline a received INVITE with 486 (Busy Here).
// -----------------------------------------------------------------------------
EXPORT_C void CSIPExSIPEngine::DeclineInviteL()
	{
	iCurrentState->DeclineInviteL( *this );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::EndSession
// End the dialog with BYE.
// -----------------------------------------------------------------------------
EXPORT_C void CSIPExSIPEngine::EndSessionL()
	{
	iCurrentState->EndSessionL( *this );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::CreateIML
// Create and send an Instant Message to recipient defined with parameter.
// This is implemented with the MESSAGE method and is sent outside of a
// dialog.
// -----------------------------------------------------------------------------
EXPORT_C void CSIPExSIPEngine::CreateIML( 
    const TDesC8& aMessage,
	const TDesC8& aSipUri )
	{
	_LIT8( KMediaType, "SIPEx" );	// Part of content type
	_LIT8( KMediaSubType, "InstantMessage" );	// Part of content type

	// Create the necessary elements of the IM
	CSIPRequestElements* reqElem = CreateReqElementsLC( aSipUri );
	CSIPToHeader* toHeader = CreateToHeaderLC( aSipUri );
	reqElem->SetToHeaderL( toHeader );
	CleanupStack::Pop( toHeader );

    // Create the From header value using info from the profile
    const TDesC8* aor = NULL;
    iProfile->GetParameter( KSIPUserAor, aor ); 
	__ASSERT_ALWAYS( aor && *aor != KNullDesC8, User::Leave( KErrNotFound ) );

    CSIPAddress* addr = CSIPAddress::DecodeL( *aor );
	CleanupStack::PushL( addr );
    CSIPFromHeader* fromHeader = CSIPFromHeader::NewL( addr );
	CleanupStack::Pop( addr );

	CleanupStack::PushL( fromHeader );
	reqElem->SetFromHeaderL( fromHeader );
	CleanupStack::Pop( fromHeader );
	

	reqElem->SetMethodL( SIPStrings::StringF( SipStrConsts::EMessage ) );

	// Get reference to the message elements from the request
	// elements, create and insert content type header (ownership
	// of the content type object is transferred)
	CSIPMessageElements& msgElem = reqElem->MessageElements();
	CSIPContentTypeHeader* ct =
   		CSIPContentTypeHeader::NewLC( KMediaType, KMediaSubType );
	msgElem.SetContentL( aMessage.AllocL(), ct );
	CleanupStack::Pop( ct );

	// Get the current connection
	CSIPConnection& conn = ConnectionL();

	// Send the request using the connection (ownership of the
	// request elements object is transferred)
	CSIPClientTransaction* ctx = conn.SendRequestL( reqElem );
	CleanupStack::Pop( reqElem );

	delete ctx;
	}
	

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::IMReceivedL
// An Instant Message is received from the network. Respond with 200 (OK)
// and alert the Engine Observer.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::IMReceivedL( CSIPServerTransaction* aTransaction  )
	{
	const CSIPRequestElements* reqElem = aTransaction->RequestElements();
	const CSIPFromHeader* fromHeader = reqElem->FromHeader();
	
    CSIPResponseElements* respElem =
    	CSIPResponseElements::NewLC( 200, 
    	    SIPStrings::StringF( SipStrConsts::EPhraseOk ) );
	// Use the transaction to send 200 (OK)
	aTransaction->SendResponseL( respElem );
	CleanupStack::Pop( respElem );

	// Inform the observer of the Instant Message
	iObserver->IMReceived(
	    fromHeader->SIPAddress().Uri8().Uri().Extract( EUriUserinfo ),
		reqElem->MessageElements().Content());	

	// We no longer need aTransaction. Just delete it.
	delete aTransaction;
	aTransaction = NULL;
	}
	

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::IMReceived
// Call the IMReceivedL method, trapping the possible errors.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::IMReceived( CSIPServerTransaction* aTransaction  )
	{
	TRAPD( err, IMReceivedL( aTransaction ));

	if ( err != KErrNone )
		{
		iObserver->EngineError( err );
		}
	}
	

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::SetCurrentState
// Sets the new current state of the state machine.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::SetCurrentState( CSIPExSIPStateBase* aState )
	{
	iCurrentState = aState;
	}
	

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ConnectionL
// Returns the active connection.
// -----------------------------------------------------------------------------
CSIPConnection& CSIPExSIPEngine::ConnectionL()
	{
	CSIPConnection* conn = CurrentConnection();
	if ( !conn )
		{
		TUint32 iapId( 0 );
	    User::LeaveIfError( iProfile->GetParameter( KSIPAccessPointId, iapId ) );
		iConnection = CSIPConnection::NewL( *iSIP, iapId, *this );
		return *iConnection;
		}
	return *conn;
	}
	

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::Profile
// Returns the enabled profile.
// -----------------------------------------------------------------------------
CSIPProfile& CSIPExSIPEngine::Profile()
	{
	return *iProfile;
	}
	

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::SetServerTx
// Sets the current Server Transaction.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::SetServerTx( CSIPServerTransaction* aTx )
	{
	delete iServerTx;
	iServerTx = aTx;
	}
	

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ServerTx
// Returns the current Server Transaction.
// -----------------------------------------------------------------------------
CSIPServerTransaction& CSIPExSIPEngine::ServerTx()
	{
	return *iServerTx;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::SetClientTx
// Sets the current Client Transaction.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::SetClientTx( CSIPClientTransaction* aTx )
	{
	delete iClientTx;
	iClientTx = aTx;
	}
	

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ClearClientTx
// Deletes the current Client Transaction.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ClearClientTx()
	{
	delete iClientTx;
	iClientTx = NULL;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ClientTx
// Returns the current Client Transaction.
// -----------------------------------------------------------------------------
CSIPClientTransaction& CSIPExSIPEngine::ClientTx()
	{
	return *iClientTx;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::SetDialogAssoc
// Sets the current Invite Dialog Association.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::SetDialogAssoc( CSIPInviteDialogAssoc& aAssoc )
	{
	delete iDialogAssoc;
	iDialogAssoc = &aAssoc;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::DialogAssoc
// Returns the current Invite Dialog Association.
// -----------------------------------------------------------------------------
CSIPInviteDialogAssoc& CSIPExSIPEngine::DialogAssoc()
	{
	return *iDialogAssoc;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::CreateToHeaderLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CSIPToHeader* CSIPExSIPEngine::CreateToHeaderLC( const TDesC8& aSipUri )
	{
	CSIPAddress* addr = CSIPAddress::DecodeL( aSipUri );
	CleanupStack::PushL( addr );
	
	CSIPToHeader* to = CSIPToHeader::NewL( addr );
	CleanupStack::Pop( addr );
	CleanupStack::PushL( to );

	return to;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::CreateReqElementsLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------	
CSIPRequestElements* CSIPExSIPEngine::CreateReqElementsLC( const TDesC8& aSipUri )
    {
    CUri8* uri8 = ConvertToUri8LC( aSipUri );
    CSIPRequestElements* req = CSIPRequestElements::NewL( uri8 );
    CleanupStack::Pop( uri8 );
    CleanupStack::PushL( req );
    return req;
    }


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::CreateMessageElementsLC
// Create a CSIPMEssageElements instance with the required information.
// -----------------------------------------------------------------------------
CSIPMessageElements* CSIPExSIPEngine::CreateMessageElementsLC()
	{
	_LIT8( KMediaType, "application" );
	_LIT8( KMediaSubType, "sdp" );

	CSIPMessageElements* msgElements = CSIPMessageElements::NewLC();

	CSdpDocument* sdpDocument = SdpDocumentLC();
	MediaFieldsL( sdpDocument );

    HBufC8* content = SdpBodyL( sdpDocument );
    CleanupStack::PushL( content );

   	CSIPContentTypeHeader* ct =
   		CSIPContentTypeHeader::NewLC( KMediaType, KMediaSubType );

	msgElements->SetContentL( content, ct );

	CleanupStack::Pop( ct );
	CleanupStack::Pop( content );
	CleanupStack::PopAndDestroy( sdpDocument );

	return msgElements;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ConvertToUri8LC
// (other items were commented in a header).
// -----------------------------------------------------------------------------  
CUri8* CSIPExSIPEngine::ConvertToUri8LC( const TDesC8& aSipUri )
    {
    TUriParser8 parser;
    User::LeaveIfError( parser.Parse( aSipUri ) ); 
    CUri8* uri8 = CUri8::NewLC( parser );
    return uri8; 
    }


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::SessionId
// Set the session ID using a random number if not set before. Return ID.
// -----------------------------------------------------------------------------
TInt64 CSIPExSIPEngine::SessionId()
	{
	if ( iSessionId > 0 )
		{
		return iSessionId;
		}
	else
		{
		// Create a random number as the session ID
		TTime now;
		now.HomeTime();
		TInt64 seed = now.Int64();

		TInt random = Math::Rand( seed );
		iSessionId = random;
		return iSessionId;
		}
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::SdpDocumentLC
// Create an instance of the SDP Document class, set the required fields, and
// leave it on the cleanup stack. Return pointer to the SDP Document instance.
// Ownership is transferred.
// -----------------------------------------------------------------------------
CSdpDocument* CSIPExSIPEngine::SdpDocumentLC()
	{
	_LIT8( KSessionName, "SIP Example Application" );

	// Create SDP document
	CSdpDocument* sdpDocument = CSdpDocument::NewL();
	CleanupStack::PushL( sdpDocument );

	CSdpOriginField* orgField = CSdpOriginField::NewL( KSdpNoInfoDesC8(), 
													   SessionId(), 0,
													   iLocalAddr );
	sdpDocument->SetOriginField( orgField );

	// Set session name
	sdpDocument->SetSessionNameL( KSessionName );

	// Set connection data
	CSdpConnectionField* connField = CSdpConnectionField::NewL( iLocalAddr );
	sdpDocument->SetConnectionField( connField );

	return sdpDocument;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::MediaFieldsL()
// Insert the media line to the SDP document given as parameter.
// The format of the media line is:
//     application 0 TCP SIPEx
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::MediaFieldsL( CSdpDocument* aDocument )
	{
	_LIT8( KFormat, "SIPEx" );

	// Set media field
	// Set media type: application
	RStringF media = StringPoolL().StringF( SdpCodecStringConstants::EMediaApplication, 
					                        SdpCodecStringPool::StringTableL() );
	// Set media transport: TCP
	RStringF prot = StringPoolL().StringF( SdpCodecStringConstants::EProtocolTcp,
				                           SdpCodecStringPool::StringTableL() );
	// Create the media line
	CSdpMediaField* mediaDescription =
		CSdpMediaField::NewL( media, 0, prot, KFormat );

	aDocument->MediaFields().Append( mediaDescription );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::SdpBodyL()
// Return SDP message body in textual form.
// Ownership is transferred.
// -----------------------------------------------------------------------------
HBufC8* CSIPExSIPEngine::SdpBodyL( CSdpDocument* aDocument )
	{
	HBufC8* sdpBuf = NULL;

	// Build the message body using buffer writer
	if ( aDocument->IsValid() )
		{
		CBufFlat* enBuf = CBufFlat::NewL( 1000 );
		CleanupStack::PushL( enBuf );
		RBufWriteStream writeStream;
		writeStream.Open( *enBuf, 0 );
		aDocument->EncodeL( writeStream );
		writeStream.Close();
		TPtr8 ptr = enBuf->Ptr( 0 );
		sdpBuf = ptr.AllocL();
		CleanupStack::PopAndDestroy( enBuf );
		writeStream.Close();
		}

	return sdpBuf;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::CurrentConnection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CSIPConnection* CSIPExSIPEngine::CurrentConnection()
    {
    if ( iConnection )
        {
        return iConnection;
        }
        
    return 0;
    }


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::IPAddressFromResponseElementsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TInetAddr CSIPExSIPEngine::IPAddressFromResponseElementsL(
	const CSIPResponseElements& aRespElem )
	{
	;
	// First, retrieve remote party IP Address
	// Get Message Elements from Response Elements
	const CSIPMessageElements& msgElem = aRespElem.MessageElements();

	// Get message content from Message Elements
	const TDesC8& content = msgElem.Content();

	// Get the SDP Document from the message content		
	CSdpDocument* SDPDoc = CSdpDocument::DecodeLC( content );

	// Get the Connection Field from the SDP Document
	CSdpConnectionField* connField = SDPDoc->ConnectionField();

	// And, finally, get the IP Address from the Connection Field
	const TInetAddr* addr = connField->InetAddress();
	const TInetAddr inetAddr = *addr;

	CleanupStack::PopAndDestroy( SDPDoc );

	return inetAddr;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::StringPoolL
// Get SDP codec string pool. Open string pool if not opened.
// -----------------------------------------------------------------------------
RStringPool CSIPExSIPEngine::StringPoolL()
	{
	TRAPD( error, SdpCodecStringPool::OpenL() );
	if ( error == KErrAlreadyExists )
		{
		return SdpCodecStringPool::StringPoolL();
		}
	else if ( error == KErrNone )
		{
		//Add missing string to the pool then return it
		return SdpCodecStringPool::StringPoolL();
		}
	else
		{
		User::Leave( error );
		return SdpCodecStringPool::StringPoolL(); //Return here to omit warning
		}
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::Observer
// Return pointer to the class registered as Engine Observer.
// -----------------------------------------------------------------------------
MSIPExSIPEngineObserver* CSIPExSIPEngine::Observer()
	{
	return iObserver;
	}




//
// IMPLEMENTATION OF THE METHODS INHERITED FROM BASE CLASSES
//

// From MSIPObserver:

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::IncomingRequest
// Handle a request coming outside of a SIP dialog.
// We have to use TRAPs because this is a non-leaving method.
// We get the ownership to aTransaction.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::IncomingRequest( 
    TUint32 aIapId,
	CSIPServerTransaction* aTransaction )
	{
	if ( !iProfile )
		{
		TRAPD( err, EnableProfileL() );

		if ( err != KErrNone )
			{
			// A profile error occured.
			// Delete aTransaction and inform the observer.
			delete aTransaction;
			aTransaction = NULL;

			iObserver->ProfileError( err );
			return;
			}
		}
	
	if ( !CurrentConnection() )
		{
		// Create an instance of the SIP Connection object.
		TRAPD( err2, iConnection = CSIPConnection::NewL( *iSIP, aIapId, *this ) );

		if ( err2 != KErrNone )
			{
			// An error occured opening the connection.
			// Delete aTransaction and inform the observer.
			delete aTransaction;
			aTransaction = NULL;

			iObserver->EngineError( err2 );
			return;
			}
		} 

	if ( aTransaction->Type() == SIPStrings::StringF( SipStrConsts::EInvite ) )
		{
		// An INVITE has been received.
		// Save the pointer for later queries and eventual
		// deletion.
		SetServerTx(aTransaction);

		// Let the current state handle the received INVITE.
		// Ownership of aTransaction is transferred.
		TRAPD( err2, iCurrentState->InviteReceivedL( *this, aTransaction ) );
		if ( err2 != KErrNone )
			{
			// An error occured when handling invitation.
			// Delete aTransaction and inform the observer.
			delete aTransaction;
			iServerTx = NULL;
			iObserver->EngineError( err2 );
			return;
			}
		}
	else if ( aTransaction->Type() == SIPStrings::StringF( SipStrConsts::EMessage ) )
		{
		IMReceived( aTransaction );
		}
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::TimedOut - No implementation needed
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::TimedOut(
	CSIPServerTransaction& /* aSIPServerTransaction */ )
	{
	}


// From MSIPConnectionObserver:

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::IncomingRequest
// No dialog has been established yet, so this should be an INVITE or MESSAGE.
// The ownership of aTransaction is transferred.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::IncomingRequest( CSIPServerTransaction* aTransaction )
	{
    if ( aTransaction->Type() == SIPStrings::StringF( SipStrConsts::EInvite ) )
        {
		// An INVITE has been received.
		// Get profile, create connection, and let the
		// state machine handle it.

		if ( !iProfile )
			{
			// If the default profile isn't enabled, do it now
			TRAPD( err, EnableProfileL() );

			if ( err != KErrNone )
				{
				// A profile error occured.
				// Delete aTransaction and inform the observer.
				delete aTransaction;
				aTransaction = NULL;

				iObserver->ProfileError( err );
				}
			}

		// The transaction still exists, which means there
		// was no trouble with the profile part. Go on with
		// request handling.
		if ( aTransaction )
			{
			// The ownership of aTransaction is transferred.
			// Save the pointer for later queries and eventual
			// deletion.
			SetServerTx(aTransaction);

			// Handle the incoming request in the state machine classes
			// The ownership of aTransaction is transferred.
			TRAPD( err2, iCurrentState->InviteReceivedL( *this, aTransaction ));
			}
        }
    else if ( aTransaction->Type() == SIPStrings::StringF( SipStrConsts::EMessage ) )
    	{
    	// An Instant Message is received
		// The state machine isn't needed for this.
    	IMReceived( aTransaction );
    	}
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::IncomingRequest
// A request concerning an established dialog is received.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::IncomingRequest( 
    CSIPServerTransaction* aTransaction,
	CSIPDialog& aDialog )
	{
	const RPointerArray<CSIPDialogAssocBase>& dialogAssoc =
    	aDialog.SIPDialogAssociations();

	for ( TInt i = 0; i < dialogAssoc.Count(); i++ )
		{
		if ( dialogAssoc[i]->Type() == 
		    SIPStrings::StringF( SipStrConsts::EInvite ) )
			{
			iDialogAssoc = 
			    reinterpret_cast<CSIPInviteDialogAssoc*>( dialogAssoc[0] );
			break;
			}
		}

    if ( aTransaction->Type() == SIPStrings::StringF( SipStrConsts::EBye ) )
        {
		// The ownership of aTransaction is transferred.
		// Save the pointer for later queries and eventual
		// deletion.
		SetServerTx(aTransaction);

        TRAPD( err, iCurrentState->ByeReceivedL( *this, *aTransaction ) );

        if ( err != KErrNone )
        	{
        	iObserver->EngineError( err );
        	}
        }
    else if ( aTransaction->Type() == 
        SIPStrings::StringF( SipStrConsts::EAck ) )
        {
		// The ownership of aTransaction is transferred.
		// Save the pointer for later queries and eventual
		// deletion.
		SetServerTx(aTransaction);

        TRAPD( err, iCurrentState->AckReceivedL( *this, *aTransaction ) );

        if ( err != KErrNone )
        	{
        	iObserver->EngineError( err );
        	}
        }
    else if ( aTransaction->Type() == 
        SIPStrings::StringF( SipStrConsts::EMessage ) )
    	{
    	IMReceived( aTransaction );
    	}
    else
        {
        // Unknown, or unimplemented, request received
		delete aTransaction;
		aTransaction = NULL;
        iObserver->EngineError( KErrNotSupported );
        }
	}

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::IncomingResponse - No implementation needed
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::IncomingResponse( 
    CSIPClientTransaction& /* aTransaction */ )
	{
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::IncomingResponse
// A response inside an established dialog is received.
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::IncomingResponse( CSIPClientTransaction& aTransaction,
										CSIPDialogAssocBase& aDialogAssoc )
	{
	if ( aDialogAssoc.Type() == SIPStrings::StringF( SipStrConsts::EInvite )  )
		{
		iDialogAssoc = reinterpret_cast<CSIPInviteDialogAssoc*>( &aDialogAssoc );
		}
	
	TRAPD( err, iCurrentState->ResponseReceivedL( *this, aTransaction ) );
	
	if ( err != KErrNone )
		{
		iObserver->EngineError( err );
		}
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::IncomingResponse
// Since the ownership is transferred, aDialogAssoc needs to be deleted
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::IncomingResponse( 
    CSIPClientTransaction& /* aTransaction */,
	CSIPInviteDialogAssoc* aDialogAssoc )
	{
	delete aDialogAssoc;
	aDialogAssoc = NULL;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::IncomingResponse - No implementation needed
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::IncomingResponse( 
    CSIPClientTransaction& /* aTransaction */,
	CSIPRegistrationBinding& /* aRegistration */ )
	{
	
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ErrorOccured( 
    TInt /* aError */,
	CSIPTransactionBase& /* aTransaction */ )
	{
	
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ErrorOccured( 
    TInt /* aError */,
	CSIPClientTransaction& /* aTransaction */,
	CSIPRegistrationBinding& /* aRegistration */ )
	{
	
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ErrorOccured( 
    TInt /* aError */,
	CSIPTransactionBase& /* aTransaction */,
	CSIPDialogAssocBase& /* aDialogAssoc */ )
	{
	
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ErrorOccured( 
    TInt /* aError */, 
    CSIPRefresh& /* aSIPRefresh */ )
	{
	
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ErrorOccured( 
    TInt /* aError */,
	CSIPRegistrationBinding& /* aRegistration */ )
	{
	
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ErrorOccured( 
    TInt /* aError */,
	CSIPDialogAssocBase& /* aDialogAssoc */ )
	{
	
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::InviteCompleted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::InviteCompleted( 
    CSIPClientTransaction& /* aTransaction */ )
	{
	delete iClientTx;
	iClientTx = NULL;
	}

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::InviteCanceled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::InviteCanceled( 
    CSIPServerTransaction& /* aTransaction */ )
    {
    iCurrentState->CancelReceivedL( *this, *iClientTx );
    delete iClientTx; 
	iClientTx = NULL;
    }

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ConnectionStateChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ConnectionStateChanged( CSIPConnection::TState aState )
	{
	if ( iConnState == CSIPConnection::EActive &&
		 aState != CSIPConnection::EActive )
		{
		// A previously active connection is active no more
		iObserver->ConnectionLost();
		}
	else
		{
		iConnState = aState;
		}
	}



// From MSIPProfileRegistryObserver


// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ProfileRegistryErrorOccurred
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ProfileRegistryEventOccurred( 
    TUint32 aProfileId, 
    TEvent aEvent )
    {
    switch (aEvent)
	    {
	    case MSIPProfileRegistryObserver::EProfileRegistered:
	        {
	        HandleProfileRegistered( aProfileId );
	        break;
	        }
	    case MSIPProfileRegistryObserver::EProfileDeregistered:
	        {
	        HandleProfileDeregistered( aProfileId );
	        break;
	        }
	    case MSIPProfileRegistryObserver::EProfileDestroyed:
	        {
            HandleProfileDestroyed( aProfileId );
	        break;
	        }
	    default:
	        {
	        // MSIPProfileRegistryObserver::EProfileCreated and
	        // MSIPProfileRegistryObserver::EProfileUpdated
	        // are ignored
	        break;
	        }
	    }
    }

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::ProfileRegistryErrorOccurred
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::ProfileRegistryErrorOccurred( 
    TUint32 /* aSIPProfileId */,
	TInt aError )
	{
	iObserver->ProfileError( aError );
	}
	

	
// Private handling for profile events

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::HandleProfileRegistered
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::HandleProfileRegistered( TUint32 aSIPProfileId )
	{
    TUint32 profileId( 0 );
	iProfile->GetParameter( KSIPProfileId, profileId );
    if ( aSIPProfileId == profileId )
        {
		// The profile registration is complete
		// Inform the Engine Observer
		iObserver->ProfileEnabled( aSIPProfileId );
        }
	}
	
// -----------------------------------------------------------------------------
// CSIPExSIPEngine::HandleProfileDeregistered
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::HandleProfileDeregistered( TUint32 aSIPProfileId )
	{
	TUint32 profileId( 0 );
	iProfile->GetParameter( KSIPProfileId, profileId );
    if ( aSIPProfileId == profileId )
        {
		// The previously registered profile has
		// become unregistered.
    	delete iDialogAssoc;
    	iDialogAssoc = 0;
    	delete iConnection;
    	iConnection = 0;
    	//iNotOwnedConnection = 0;
    	delete iProfile;
    	iProfile = 0;
    	
    	iObserver->ProfileError( KErrGeneral );     	
        }
	}

// -----------------------------------------------------------------------------
// CSIPExSIPEngine::HandleProfileDestroyed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEngine::HandleProfileDestroyed( TUint32 /* aSIPProfileId */ )
	{
	delete iDialogAssoc;
	iDialogAssoc = 0;
	delete iConnection;
	iConnection = 0;
	//iNotOwnedConnection = 0;
	delete iProfile;
	iProfile = 0;

	iObserver->ProfileError( KErrGeneral );
	}
	
// End of file
