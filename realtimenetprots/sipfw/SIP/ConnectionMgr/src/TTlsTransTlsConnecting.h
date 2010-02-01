/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TTlsTransTlsConnecting.h
* Part of       : ConnectionMgr
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/


#ifndef TTLSTRANSTLSCONNECTING_H
#define TTLSTRANSTLSCONNECTING_H

//  INCLUDES
#include "TTlsTransStateBase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* TLS Transport in connecting TCP state.
*
*/
class TTlsTransTlsConnecting : public TTlsTransStateBase
	{	    
	public: // New functions
	
	    TTlsTransTlsConnecting( MTlsTransStateOwner& aOwner );
	    
		//Boolean which decides whether Server TLS certificate is valid or not
	    TBool iValidCertificate;
	
	public: // From TTlsTransStateBase
	
	    void EnterL();
	    
	    TBool ConnectionOpenL();
	    
	};
	
#endif // TTLSTRANSTLSCONNECTING_H

// End of File
