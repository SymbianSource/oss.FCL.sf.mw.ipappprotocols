/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipgendefs.h
* Part of       : sip profile fsm
* Version       : 1.0 
*
*/




/**
 @internalComponent
*/

_LIT8(KSIPauthheader, "Authorization");
_LIT8(KSIPauthForHeader, "Digest ");
_LIT8(KSIPnonce, "nonce");
_LIT8(KSIPresponse, "response");
_LIT8(KSIPuri, "uri");
_LIT8(KSIPequals, "=");
_LIT8(KSIPcomma, ",");
_LIT8(KSIPschema, "sip:");
_LIT8(KSIPUsername, "username");
_LIT8(KSIPrealm, "realm");
_LIT8(KSIPServer, "server");
_LIT8(KSIPdigest, "digest");
_LIT8(KSIPuserid, "userid");
_LIT8(KSIPprotected, "protected");
_LIT8(KSIPpasswd, "passwd");
_LIT8(KSIPImcType,"ims");
_LIT8(KSIPpath, "path");
_LIT8(KSIPcomp, "comp");
_LIT8(KSIPsigcomp, "sigcomp");
_LIT8(KSIPIetfType,"IETF");
_LIT8(KSIPTcp,"TCP");
_LIT8(KSIPlocalhost, "sip:localhost");

const TText8 KSIPquot = '"';
const TUint  KSIPquotneeded = 10;
const TUint	 KSIPIetfExpiresValue = 3600;
const TUint	 KSIPImsExpiresValue = 600000;
const TUint	 KSIPImc1ExpiresValue = 600000;

const TText8 KSIPtab = '\t';
const TUint KSIParrayGran = 5;
const TUint K200Ok 						= 200;
const TUint K300MultipleChoices 		= 300;
const TUint K400BadRequest				= 400;
const TUint K405MethodNotAllowed		= 405;
const TUint K420BadExtension			= 420;
const TUint K481TransactionDoesNotExist = 481;
const TUint K482LoopDetected			= 482;
const TUint K489BadEvent				= 489;
const TUint K503ServiceUnavailable		= 503;
const TUint K408TimeOut					= 408;
const TUint K480TemporarilyUnavailable  = 480;
const TUint K500ServerInternalError		= 500;
const TUint	K504ServerTimeOut			= 504;
