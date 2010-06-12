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

#ifndef CMSRPSESSIONPARAMS_H_
#define CMSRPSESSIONPARAMS_H_


class CMSRPSessionParams : public CBase
    {
public:
    
    /**
     * Creates the CMSRPSessionParams object required to set
     * some of the msrp session parameters
     */
    IMPORT_C static CMSRPSessionParams* NewL( );
    
    IMPORT_C ~CMSRPSessionParams();
    
public:
    
    /**
     * Sets the Success and Failure Report headers in the msrp message.
     * This can be done at any point of time in an msrp session when there 
     * is a need to change these headers.
     * @param aSuccessReport Success Report header value EYes/ENo
     * @param aFailureReport Failure Report header value EYEs/ENo/EPartial
     * Giving the default values will not include the header in the message
     * SR value ignored as of now since REPORTs are not supported
     * SR header never addded in outgoing message
     */    
    IMPORT_C void SetReportHeadersL( TReportStatus aSuccessReport, TReportStatus aFailureReport );
    
    /**
     * Fetches the value of the Success Report header
     * @return SR header value
     */
    IMPORT_C TReportStatus SuccessReportHeader( );
    
    /**
     * Fetches the value of the Failure Report header
     * @return FR header value
     */
    IMPORT_C TReportStatus FailureReportHeader( );
    
private:
    CMSRPSessionParams( );
    
private:
        
    TReportStatus iSuccessReport;
    TReportStatus iFailureReport;
    
    };


#endif /* CMSRPSESSIONPARAMS_H_ */
