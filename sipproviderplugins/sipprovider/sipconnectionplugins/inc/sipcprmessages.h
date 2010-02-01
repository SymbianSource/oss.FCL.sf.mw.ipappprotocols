// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SIP Connection Provider messages
// 
//

/**
 @file
 @internalTechnology
*/
 
#ifndef SIPCPRMESSAGES_H_
#define SIPCPRMESSAGES_H_

#include <comms-infras/ss_nodemessages.h>

namespace SipCpr
{
enum TCprStage
  	{
   	EFresh = 0,
   	EStarting,
   	EActive,
   	EStopping,
   	EStopped
   	};

enum TConnType
	{
	EConnIncoming,
	EConnOutgoing
    };
   
            
class TSipCprMessages
	{
public:
	enum {ERealmId = 0x10274C37 };
private:
	enum
		{		
		ESipCprRegistrationComplete = Messages::TEBase::TNull::EId+5000,
		ESipCprDeRegistrationComplete,				
		EIncomingConnection
		};
		
public:
	typedef Messages::TMessageSigNumber<ESipCprRegistrationComplete, TSipCprMessages::ERealmId> TRegistrationComplete;
	typedef Messages::TMessageSigNumber<ESipCprDeRegistrationComplete, TSipCprMessages::ERealmId> TDeRegistrationComplete;	
	typedef Messages::TMessageSigNumber<EIncomingConnection, TSipCprMessages::ERealmId> TIncomingConnection;
	};
	
}

#endif //SIPCPRMESSAGES_H_

