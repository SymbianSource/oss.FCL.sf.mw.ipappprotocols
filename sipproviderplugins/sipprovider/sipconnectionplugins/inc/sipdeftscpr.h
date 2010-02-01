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
// SIP Default SubConnection Provider class.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SIPDEFTSCPR_H
#define SYMBIAN_SIPDEFTSCPR_H



#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/corescpr.h>
#include <comms-infras/corescprstates.h>
#include <comms-infras/corescpractivities.h>
#include <cs_subconevents.h>
#include <cs_subconparams.h>
#include <sip_subconevents.h>
#include <sip_subconparams.h>
#include <comms-infras/ss_log.h>
#include "sipscprfactory.h"
#include <comms-infras/ss_corepractivities.h>

#ifdef __CFLOG_ACTIVE
	#define KSipSCprTag KESockSubConnectionTag
	_LIT8(KSipDeftSCprSubTag, "sipdeftscpr");
#endif



class CSipDefaultSubConnectionProvider : public CCoreSubConnectionProvider
/** Default SIP subconnection provider

@internalTechnology
@released Since 9.5 */
    {
    friend class CSipSubConnectionProviderFactory;
public:
    typedef CSipSubConnectionProviderFactory FactoryType;
    
    /**
    Destructor of the class
    */
	virtual ~CSipDefaultSubConnectionProvider();

protected:
    CSipDefaultSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory);
    void ConstructL();
    static CSipDefaultSubConnectionProvider* NewL(ESock::CSubConnectionProviderFactoryBase& aFactory);
    virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
    };

#endif //SYMBIAN_SIPDEFTSCPR_H
