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

// CLASS HEADER
#include "CMSRPMessageBase.h"

// EXTERNAL INCLUDES
#include <MsrpStrConsts.h>

// INTERNAL INCLUDES
#include "MsrpCommon.h"
#include "CMsrpToPathHeader.h"
#include "CMsrpFromPathHeader.h"
#include "CMsrpContentTypeHeader.h"
#include "CMSRPMessageIdHeader.h"
#include "CMsrpByteRangeHeader.h"
#include "CMsrpFailureReportHeader.h"
#include "CMsrpSuccessReportHeader.h"
#include "CMsrpStatusHeader.h"

// -----------------------------------------------------------------------------
// CMSRPMessageBase::CMSRPMessageBase
// -----------------------------------------------------------------------------
//
CMSRPMessageBase::CMSRPMessageBase()
	{
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::~CMSRPMessageBase
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPMessageBase::~CMSRPMessageBase()
	{
	delete iToPath;
	delete iFromPath;
	delete iContentType;
	delete iMessageId;
	delete iByteRange;
	delete iFailureReport;
	delete iSuccessReport;
    delete iStatusHeader;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::SetToPathHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessageBase::SetToPathHeader( CMSRPToPathHeader* aToPath )
	{
    if(iToPath)
        {
        delete iToPath;
        iToPath = NULL;
        }
	iToPath = aToPath;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::ToPathHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CMSRPToPathHeader* CMSRPMessageBase::ToPathHeader() const
	{
    if( iToPath )
        {
        return iToPath;
        }
	return NULL;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::SetFromPathHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessageBase::SetFromPathHeader( CMSRPFromPathHeader* aFromPath )
	{
    if(iFromPath)
        {
        delete iFromPath;
        iFromPath = NULL;
        }
	iFromPath = aFromPath;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::FromPathHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CMSRPFromPathHeader* CMSRPMessageBase::FromPathHeader() const
	{
    if( iFromPath )
        {
        return iFromPath;
        }
    return NULL;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::SetContentTypeHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessageBase::SetContentTypeHeader(
	CMSRPContentTypeHeader* aContentType )
	{
    if(iContentType)
        {
        delete iContentType;
        iContentType = NULL;
        }
	iContentType = aContentType;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::ContentTypeHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CMSRPContentTypeHeader* CMSRPMessageBase::ContentTypeHeader() const
	{
    if( iContentType )
        {
        return iContentType;
        }
    return NULL;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::SetFailureReportHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessageBase::SetFailureReportHeader(
	CMSRPFailureReportHeader* aFailureReport )
	{
    if(iFailureReport)
        {
        delete iFailureReport;
        iFailureReport = NULL;
        }
	iFailureReport = aFailureReport;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::FailureReportHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CMSRPFailureReportHeader* CMSRPMessageBase::FailureReportHeader() const
	{
    if( iFailureReport )
        {
        return iFailureReport;
        }
    return NULL;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::SetSuccessReportHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessageBase::SetSuccessReportHeader(
	CMSRPSuccessReportHeader* aSuccessReport )
	{
    if(iSuccessReport)
        {
        delete iSuccessReport;
        iSuccessReport = NULL;
        }
	iSuccessReport = aSuccessReport;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::SuccessReportHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CMSRPSuccessReportHeader* CMSRPMessageBase::SuccessReportHeader() const
	{
    if( iSuccessReport )
        {
        return iSuccessReport;
        }
    return NULL;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::SetMessageIdHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessageBase::SetMessageIdHeader( CMSRPMessageIdHeader* aMessageId )
	{
    if(iMessageId)
        {
        delete iMessageId;
        iMessageId = NULL;
        }
	iMessageId = aMessageId;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::MessageIdHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CMSRPMessageIdHeader* CMSRPMessageBase::MessageIdHeader() const
	{
    if( iMessageId )
        {
        return iMessageId;
        }
    return NULL;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::SetByteRangeHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessageBase::SetByteRangeHeader( CMSRPByteRangeHeader* aByteRange )
	{
    if(iByteRange)
        {
        delete iByteRange;
        iByteRange = NULL;
        }
	iByteRange = aByteRange;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::ByteRangeHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CMSRPByteRangeHeader* CMSRPMessageBase::ByteRangeHeader() const
	{
    if( iByteRange )
        {
        return iByteRange;
        }
    return NULL;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageBase::SetStatusHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessageBase::SetStatusHeader( CMSRPStatusHeader* aStatus )
    {
    iStatusHeader = aStatus;
    }

// -----------------------------------------------------------------------------
// CMSRPMessageBase::StatusHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CMSRPStatusHeader* CMSRPMessageBase::StatusHeader() const
    {
    return iStatusHeader;
    }

// End of File
