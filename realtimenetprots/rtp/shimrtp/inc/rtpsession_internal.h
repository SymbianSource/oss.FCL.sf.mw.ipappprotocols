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
//

/**
 @file
 @internalComponent
*/

#ifndef RTP_INTERNAL_H_
#define RTP_INTERNAL_H_
#include "rtp.h"



class RRtpSession_Internal: public RRtpSession
	{
	
public:
	using RRtpSession::OpenL;


	/** 
	@internalComponent
    
	Opens the session and initialises it.

	No events will be generated until after control returns to the
	active scheduler. The client has this time to initialise the
	object by adding event callbacks.

	This API uses the RTP Stack implemented as a CF Prorotocol. The SubConnection
	which is passed to the API should be initialised with the RTP Params. 

	@param aServer         Socket Server. 
	@param aLocalAddr      The maximum size of a received packet.
	@param aRemoteAddr     The RTCP socket.
	@param aMaxRXSize      The maximum size of a received packet.
	@param aSubConnection  The subconnection with RTP Parametrs set on it.
	@param aCName          The CNAME. It needs to be same as the one set using the API:
    					   CSubConRTPGenericParamSet::SetCNAMEL(const TDesC8& aCName).
    					   If the descriptor is KNullDesC8,the RTCP session is not created.
                           
	*/
	IMPORT_C void OpenL(RSocketServ& aServer, TSockAddr& aLocalAddr,
						TSockAddr& aRemoteAddr,
						TInt aMaxRXSize, RSubConnection& aSubConnection, TInt aPriority = EPriorityNormal, 
						const TDesC8& aCNAME = KNullDesC8);
						
						
	
	/**
	@internalComponent

	Gets the remote RTCP Statistics for the current RTP session
    
	@param SSRC of the sender
	@param RTCP Statistics class that stores the RTCP statistics for the 
	current RTP session
	*/
	
	IMPORT_C TInt RtcpStatistics(TUint32 aSsrc, TRtcpStatistics &aRtcpStatistics);
	
	
	
	/**
	@internalComponent

	Pre and Post processing function for RTP and RTCP packets

	If a function is registered for a particular event, it will be called when 
	that event occurs. One callback function can be associated with more than 1
	callback registration. Callback functions take a pointer argument
	which was supplied as part of the registration

	@param aType Event type
		
	@param aCallback Callback object
		
	@param aPtr Pointer to data that needs to be passed to the callback function
	*/
	IMPORT_C void SetPrePostProcessingRegisterCallback(MPrePostProcessingCallback* aPrePostProcessingCallback);


	};

#endif //RTP_INTERNAL_H_

 