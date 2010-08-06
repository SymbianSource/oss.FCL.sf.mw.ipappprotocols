/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Constants for DM SIP Adapter.
*
*/


#include <e32base.h>

const TInt KDefaultPortNumber        = 5060;
const TInt KMaxProviderNameLength    = 100;
const TInt KUriPathMaxLength         = 100; 
const TInt KNSmlSIPMaxUriLength      = 256;
const TInt KDefaultResultSize        = 64;
const TInt KNSmlSIPMaxResultLength   = 256;
const TInt KMaxMappingLength         = 15; //SIPId0x00000002
const TInt KNSmlDMFatMinimumFileSize = 512;
const TInt KMaxSchemeLength          = 5;

// Reference: OMA DM Management Object for Nokia SIP.
_LIT8( KNSmlDMSIPDDFVersion,
    "1.0" );
_LIT8( KNSmlDMSIPNodeName,
    "SIP" );
_LIT8( KNSmlDMSIPSipID,
    "SIPId" );
_LIT8( KNSmlDMSIPName,
    "Name" );
_LIT8( KNSmlDMSIPProfileType,
    "ProfileType" );
_LIT8( KNSmlDMSIPAOR,
    "AddressOfRecord" );
_LIT8( KNSmlDMSIPOutboundProxy,
    "OutboundProxy" );
_LIT8( KNSmlDMSIPHost,
    "Host" );
_LIT8( KNSmlDMSIPPort,
    "Port" );
_LIT8( KNSmlDMSIPTransport,
    "Transport" );
_LIT8( KNSmlDMSIPUsername,
    "Username" );
_LIT8( KNSmlDMSIPPrivateIdentity,
    "PrivateIdentity" );
_LIT8( KNSmlDMSIPPasswd,
    "Passwd" );
_LIT8( KNSmlDMSIPRealm,
    "Realm" );
_LIT8( KNSmlDMSIPRegistrarServer,
    "RegistrarServer" );
_LIT8( KNSmlDMSIPDefault,
    "Default" );
_LIT8( KNSmlDMSIPLooseRouting,
    "LooseRouting" );
_LIT8( KNSmlDMSIPEnableSecurityNegotiation,
    "EnableSecurityNegotiation" );
_LIT8( KNSmlDMSIPEnableSigComp,
    "EnableSignalCompression" );
_LIT8( KNSmlDMSIPEnableAutoRegistration,
    "EnableAutoRegistration" );
_LIT8( KNSmlDMSIPConRef,
    "PrefConRef" );
_LIT8( KNSmlDMSIPProfileLock,
    "ProfileLock" );
_LIT8( KNSmlDMSIPSnap,
    "DestinationNetwork" );
_LIT8( KNSmlDMSIPSigQos,
    "SignalingQoS" );
_LIT8( KNSmlDMSIPBearerID,
    "BearerID" );

// Explanations of parameters
_LIT8( KNSmlDMSIPNodeNameExp,
    "The interior object holds all SIP objects" );
_LIT8( KNSmlDMSIPDynamicNodeExp,
    "Placeholder for settings ID's" );
_LIT8( KNSmlDMSIPSipIDExp,
    "Settings ID number" ); // 1
_LIT8( KNSmlDMSIPNameExp,
    "Name (Provider)" );
_LIT8( KNSmlDMSIPProfileTypeExp,
    "ProfileType" );
_LIT8( KNSmlDMSIPAORExp,
    "Address Of Record (Public User Name)" );
_LIT8( KNSmlDMSIPOutboundProxyExp,
    "Outbound Proxy node" );
_LIT8( KNSmlDMSIPHostExp,
    "Host" );
_LIT8( KNSmlDMSIPPortExp,
    "Port" );
_LIT8( KNSmlDMSIPTransportExp,
    "Transport" );
_LIT8( KNSmlDMSIPUsernameExp,
    "Username" );
_LIT8( KNSmlDMSIPPrivateIdentityExp,
    "Private Identity" );
_LIT8( KNSmlDMSIPPasswdExp,
    "Password" );
_LIT8( KNSmlDMSIPRealmExp,
    "Realm" );
_LIT8( KNSmlDMSIPRegistrarServerExp,
    "Registrar Server node" );
_LIT8( KNSmlDMSIPDefaultExp,
    "Default" );
_LIT8( KNSmlDMSIPLooseRoutingExp,
    "Loose Routing" );
_LIT8( KNSmlDMSIPEnableSecurityNegotiationExp,
    "Enable Security Negotiation" );
_LIT8( KNSmlDMSIPEnableSigCompExp,
    "Enable Signal Compression" );
_LIT8( KNSmlDMSIPEnableAutoRegistrationExp,
    "Enable Auto Registration" );
_LIT8( KNSmlDMSIPConRefExp,
    "Reference to connectivity information" );
_LIT8( KNSmlDMSIPProfileLockExp,
    "Profile lock" );
_LIT8( KNSmlDMSIPSnapExp,
    "Reference to Destination Network" );
_LIT8( KNSmlDMSIPSigQosExp,
    "Signaling QoS" );
_LIT8( KNSmlDMSIPBearerIDExp,
    "Bearer ID" );

// For SIP URI.
// References: OMA DM MO for Nokia SIP and IETF RFC3261.
_LIT8( KPTypeIMS,                "ims" );
_LIT8( KPTypeIETF,               "IETF" );
_LIT8( KPTypeOther,              "OTHER" );
_LIT8( KNSmlDMSIPValueTrue,      "True");
_LIT8( KNSmlDMSIPValueFalse,     "False");
_LIT8( KNSmlDMSIPTransportTCP,   "TCP");
_LIT8( KSIPTransportTCP,         ";transport=tcp");
_LIT8( KSIPTransportUDP,         ";transport=udp");
_LIT8( KSIPTransportPrefix,      ";transport=");
_LIT8( KSIPlr,                   ";lr");
_LIT8( KNSmlDMSIPTransportUDP,   "UDP");
_LIT8( KNSmlDMSIPTransportAuto,  "Auto" );

// Other
_LIT8( KHexPrefix,               "0x" );
_LIT8( KSIPPrefix,               "SIPId" );
_LIT8( KSIPHexPrefix,            "SIPId0x" );
_LIT8( KNSmlDMSIPTextPlain,      "text/plain" );
_LIT8( KNSmlDMSIPSeparator8,     "/" );
_LIT8( KNSmlSIPUriDotSlash,      "./");
_LIT8( KOpenParenthesis,         "(" );
_LIT8( KClosedParenthesis,       ")" );
_LIT8( KNSmlDMSIPSIPPrefix,      "sip:" );
_LIT8( KNSmlDMSIPSIPSPrefix,     "sips:");
_LIT8( KNSmlDMSIPSip,            "sip" );
_LIT8( KNSmlDMSIPSips,           "sips" );
_LIT8( KNSmlDMSIPSnapUriPrefix,  "BearerManagementSNAP" );

const TUint32 KMaxSigQosValue = 63;
const TUint8 KDMSIPSeparator = '/';

// End of file.
