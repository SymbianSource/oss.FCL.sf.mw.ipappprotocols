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
* Name          : CSIPOptionsHandler.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPOPTIONSHANDLER_H
#define CSIPOPTIONSHANDLER_H

#include <e32base.h>
#include <stringpool.h>
#include "MSipClients.h"
#include "MSipClient.h"
#include "CSIPClientResolver2.h"
#include "_sipcodecdefs.h"

class CSIPRequest;
class CSIPResponse;
class CSdpDocument;
class CSdpOriginField;
class CSdpMediaField;
class CSdpFmtAttributeField;
class CSIPHeaderBase;
class CSIPContentTypeHeader;
class MSipClient;
class CSIPResolvedClient2;
class CSIPAcceptHeader;

class CSIPOptionsHandler : public CBase
    {
public:

    static CSIPOptionsHandler* NewLC(CSIPRequest& aRequest);
    ~CSIPOptionsHandler();

    void AddClientDataL(CSdpDocument& aSdpDocument);
	
	void AddClientDataL(RPointerArray<CSdpMediaField>* aMediaFields);
	
    CSIPResponse* CreateResponseL( RArray<TUid>& aUids,
    							   MSipClients& aSipClients,
    							   CSIPClientResolver2& aClientResolver2 );
    
    static void PushLC(RPointerArray<CSIPContentTypeHeader>* aArray);
	static void ResetAndDestroyArray(TAny* anArray);
	
	static void PushLC(RPointerArray<CSIPAcceptHeader>* aArray);
	static void ResetAndDestroy(TAny* anArray);

private:

    CSIPOptionsHandler();
	void ConstructL(CSIPRequest& aRequest);
    
    TBool SDPAccepted(CSIPRequest& aRequest);
    
    CSdpMediaField* FindMatchingField(const CSdpMediaField& aMediaField);
    
    void AppendMediaFieldL( CSdpMediaField* aMediafield );
                          
    void AddAttributesL(CSdpMediaField& aSource,
                        CSdpMediaField& aDestination);
                        
    void AddAttributesL(CSdpMediaField* aSource,
                        CSdpMediaField& aDestination);
                        
    TBool AppendFormatAttributeFieldsL( CSdpFmtAttributeField* aAttribute,
    									CSdpMediaField& aDestination );
                    
    CSdpOriginField* CreateOriginFieldL(const TDesC8& aLocalHost,
                                        const RStringF& aHostType) const;
    
    void LocalHostL(CSIPRequest& aRequest, 
                    TPtrC8& aLocalHost,
                    RStringF& aHostType) const;
                    
    TBool HasMatchingFmtAttritbute(CSdpMediaField& aMedia,
                                   const CSdpFmtAttributeField& aAttribute);
                                   
    void UpdateFormatListL(CSdpMediaField& aMedia);
    
    void AddAcceptToResponseL( CSIPResponse& aResponse,
    						   RArray<TUid>& aUids,
    						   MSipClients& aSipClients,
    						   CSIPClientResolver2& aClientResolver2 );
    void CollectHeadersL( RArray<TUid>& aUids,
						  MSipClients& aSipClients,
    					  CSIPClientResolver2& aClientResolver2 );
    						   
    void RemoveDuplicateAcceptHeaders();
    void CollectHeadersL( MSipClient* aSipClient );
	void CollectHeadersL( CSIPResolvedClient2* aClient );
private: // Data

	RStringF iSdpNetType;
	RStringF iSdpIPv4Type;
	RStringF iSdpIPv6Type;
    RStringF iRtpAvp;
    RStringF iRtpmap;
    RStringF iApplication;	
	CSdpDocument* iSdpDocument;
	TBool iSdpAccepted;
	RPointerArray<CSIPHeaderBase> iSipAcceptHeaders;
	
private: // For testing purposes

	UNIT_TEST(CSIPOptionsHandlerTest)
    };

#endif // CSIPOPTIONSHANDLER_H

// End of File
