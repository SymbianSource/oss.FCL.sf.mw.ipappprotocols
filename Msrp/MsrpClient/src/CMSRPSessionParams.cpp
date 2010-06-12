/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/


//  Include Files
#include "MsrpCommon.h"
#include "CMSRPSessionParams.h"


//  Member Functions

EXPORT_C CMSRPSessionParams* CMSRPSessionParams::NewL( )
    {
    CMSRPSessionParams* self = new ( ELeave ) CMSRPSessionParams( );
    return self;
    }


EXPORT_C CMSRPSessionParams::~CMSRPSessionParams()
    {
    }


CMSRPSessionParams::CMSRPSessionParams( ) : iSuccessReport(ENo),
                                            iFailureReport(EYes)
    {
    }


EXPORT_C void CMSRPSessionParams::SetReportHeadersL( TReportStatus aSuccessReport, TReportStatus aFailureReport )
    {
    if(aSuccessReport == EPartial)
        {
        User::Leave( KErrArgument );
        }
    iSuccessReport = ENo;   // Since Reports are not supported now, the SR value given is ignored
    iFailureReport = aFailureReport;
    }


EXPORT_C TReportStatus CMSRPSessionParams::SuccessReportHeader( )
    {
    return iSuccessReport;
    }


EXPORT_C TReportStatus CMSRPSessionParams::FailureReportHeader( )
    {
    return iFailureReport;
    }
