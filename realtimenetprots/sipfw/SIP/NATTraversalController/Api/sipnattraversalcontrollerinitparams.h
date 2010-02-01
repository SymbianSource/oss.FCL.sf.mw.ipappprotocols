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
* Name          : sipnattraversalcontrollerinitparams.h
* Part of       : SIP NAT Traversal Controller
* Version       : SIP/5.0 
*
*/



#ifndef TSIPNATTRAVERSALCONTROLLERINITPARAMS_H
#define TSIPNATTRAVERSALCONTROLLERINITPARAMS_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* Initialization parameters for SIP NAT Traversal Controller ECOM plug-in(s).
*/
class TSIPNATTraversalControllerInitParams
	{
	public: // Constructors and destructor

        /**
        * Constructor.
        * @param aSocketServ an opened socket server handle
        */
		TSIPNATTraversalControllerInitParams(RSocketServ& aSocketServ);

	public: // Data

		RSocketServ& iSocketServ;
	};

#include "sipnattraversalcontrollerinitparams.inl"

#endif // TSIPNATTRAVERSALCONTROLLERINITPARAMS_H
