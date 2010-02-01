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
* Name          : CSdpMLineNameStrategy.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPMLINESTRATEGYBASE_H
#define CSDPMLINESTRATEGYBASE_H

#include "CSdpStrategyBase.h"

class MSipClients;
class MSipClient;
class CSdpMediaField;



class CSdpMLineStrategyBase : public CSdpStrategyBase
    {
public:

    virtual ~CSdpMLineStrategyBase();
    
    CSIPResponse* ApplyL(CSdpDocument& aDocument, RArray<TUid>& aUids);
    
    TBool IsApplicable(CSdpDocument& aDocument);   

protected:

	CSdpMLineStrategyBase(const MSipClients& aSipClients);
						
	static CSIPResponse* MediaLineExistL(CSdpDocument& aDocument);
	
	void FindApplicationL(CSdpDocument& aDocument,RArray<TUid>& aUids);
	
    TBool MatchL(CSdpDocument& aDocument, MSipClient& aClient);	

    virtual TBool IsApplicable(CSdpMediaField& aMediaField);
	
	virtual TBool FieldsMatchL(const CSdpMediaField& aReceivedMedia,
	                           const CSdpMediaField& aClientMedia) = 0;
    };

#endif // CSDPMLINESTRATEGYBASE_H

// End of File
