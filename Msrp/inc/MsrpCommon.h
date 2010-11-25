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

#ifndef __MSRPCOMMON_H__
#define __MSRPCOMMON_H__

// EXTERNAL INCLUDES
#include <e32base.h>
#include <e32debug.h>
#include <flogger.h>


#ifdef UT_TEST

const TInt KTimeOutInSeconds( 5 );
const TInt KSecondinMicroseconds( 1000000 );

const TInt KListenTimeoutInSeconds( 10 );
const TInt KBufSize = 4096;
const TInt KThreshold = 3839;

#else

const TInt KTimeOutInSeconds( 30 );
const TInt KSecondinMicroseconds( 1000000 );

const TInt KListenTimeoutInSeconds( 30 );
const TInt KBufSize = 16384;
const TInt KThreshold = 1024;

#endif

// CONSTANTS
// Server name.
_LIT( KMSRPServerName, "!MSRPServer" );
// The name of the MSRP Client module
_LIT( KMSRPClientName, "MSRPClient" );

_LIT( KLogDir1, "MSRP" );
_LIT( KLogFile1, "msrp.txt" );
_LIT8( KLog8Dir1, "MSRP" );
_LIT8( KLog8File1, "msrp.txt" );
_LIT( KMSRPExe, "MSRPServer.exe" );

#ifdef __WINSCW__
_LIT( KProgramsDir, "\\" );
#else
_LIT( KProgramsDir, "\\sys\\bin\\" );
#endif

_LIT8( KCRAndLF, "\r\n" );
_LIT8( KMatchCRAndLF, "*\r\n*" );
_LIT8( KMatch2CRAndLF, "*\r\n\r\n*" );
_LIT8( KSpaceChar, " " );
_LIT8( KDashLine, "-------" );
_LIT8( KMessageEndSign, "$" );
_LIT8( KMessageContinuesSign, "+" );
_LIT8( KMessageTerminatedSign, "#" );
_LIT8( KAsterisk, "*" );

_LIT8( KMsrpUriScheme, "msrp://" );
_LIT8( KColon, ":" );
_LIT8( KSemicolon, ";" );
_LIT8( KForwardSlash, "/" );
_LIT8( KTransport, "tcp" );

_LIT8( KMSRP,"MSRP" );
_LIT8( KMSRPSend,"SEND" );
_LIT8( KMSRPReport,"REPORT" );
_LIT8( KMSRPMessageID, "Message-ID" );
_LIT8( KMSRPByteRange,"Byte-Range" );
_LIT8( KMSRPContentType, "Content-Type" );
_LIT8( KMSRPToPath, "To-Path" );
_LIT8( KMSRPFromPath,"From-Path" );
_LIT8( KMSRPSuccessReport,"Success-Report" );
_LIT8( KMSRPFailureReport,"Failure-Report" );
_LIT8( KMSRPStatus, "Status" );

// temporary file path
_LIT( KDefaultTempFilePath, "C:\\system\\temp\\" );

// CONSTANTS
const TInt KMaxLengthOfUrl = 255;
const TInt KMaxLengthOfHost = 255;
const TInt KMaxLengthOfSessionId = 255;
const TInt KSizeOfProgramPath = 50;
const TInt KBufExpandSize = 256;

// maximum length of incoming message buffer (for externalizing CMSRPMessage
// class
const TInt KMaxLengthOfIncomingMessageExt = 4096;

// max length of small MSRP message
const TInt KMaxLengthOfSmallMSRPMessage = 2048;

// MSRP port number, IANA assigned
const TInt KMsrpPort = 2855;

// MSRP server version
const TUint KMSRPServerMajorVersionNumber = 1;
const TUint KMSRPServerMinorVersionNumber = 0;
const TUint KMSRPServerBuildVersionNumber = 0;

// end of line chars
const TUint KDashCharacter = 0x2D;
const TUint KDividedCharacter = 0x2F;
const TUint KSpaceCharacter = 0x20;

const TUint KColonCharacter = 0x3A;
const TUint KSemiColonCharacter = 0x3B;
const TInt KMaxLengthOfStatusCode = 3;

// the maximum length of MSRP end line string
const TInt KMaxLengthOfMessageEndString = 100;

// the maximum length of transaction id string
const TInt KMaxLengthOfTransactionIdString = 100;

const TInt KUnknownRange = -1;

// MACROS
#ifdef _DEBUG

	// logging
	#define MSRPLOG( text ) \
		{ \
		_LIT( KText, text ); \
		RFileLogger::Write( KLogDir1, KLogFile1, EFileLoggingModeAppend, KText ); \
		RDebug::Print( KText ); \
		}

	#define MSRPLOG2( text, value ) \
		{ \
		_LIT( KText, text ); \
		RFileLogger::WriteFormat( KLogDir1, KLogFile1, EFileLoggingModeAppend, \
		TRefByValue<const TDesC>( KText() ), value ); \
		RDebug::Print( KText, value ); \
		}

	#define MSRPLOG3( text, value, value2 ) \
		{ \
		_LIT( KText, text ); \
		RFileLogger::WriteFormat( KLogDir1, KLogFile1, EFileLoggingModeAppend, \
		TRefByValue<const TDesC>( KText() ), value, value2 ); \
		RDebug::Print( KText, value, value2 ); \
		}

#else

	// not used
	#define MSRPLOG( text )
	#define MSRPLOG2( text, value1 )
	#define MSRPLOG3( text, value1, value2 )

#endif // END MACROS

// ENUMS

/** Error codes returned to the client */
enum TMSRPErrorCodes
    {
    EUnknownCode = 1,
    EAllOk = 200,
    EUnintelligibleRequest = 400,
    EActionNotAllowed = 403,
    ETimeout = 408,
    EStopSending = 413,
    EMimeNotUnderstood = 415,
    EParameterOutOfBounds = 423,
    ESessionDoesNotExist = 481,
    EUnknownRequestMethod = 501,
    ESessionAlreadyBound = 506
    };

// MSRP API IPC request definitions
enum TMSRPRequests
	{
    EMSRPCreateSubSession,
	EMSRPCloseSubSession,
	EMSRPLocalPath,
	EMSRPConnect,
	EMSRPListenConnections,
    EMSRPListenMessages,
    EMSRPListenSendResult,
    EMSRPSendMessage,    
	EMSRPCancelSending,
	EMSRPCancelReceiving,
	EMSRPCancelSendRespListening,
	EMSRPProcessQueuedRequests,
	EMSRPProgressReports,
	EMSRPReserved
	};

enum TMSRPMessageContent
	{
	EMSRPChunkEndHeaders,
	EMSRPChunkEndNoHeaders,
	EMSRPChunkPartialChunkNoHeaders,
	EMSRPChunkPartialChunkHeaders
	};

enum TServerClientResponses
    {
    ESubSessionHandle=3, // type integer and returned as response to EMSRPCreateSubSession  
    // Reserved,    
    };

enum TPanicCode
    {
    EBadRequest = 1,
    EBadDescriptor,
    EBadSubsessionHandle 
    };

enum TReportStatus
    {
    EYes,
    ENo,
    EPartial
    };

// Local host name and session id
typedef struct TLocalPathMSRPData
    {
    TBuf8< KMaxLengthOfHost> iLocalHost;
    } TLocalPathMSRPData;
    

// for connection parameters
typedef struct TConnectMSRPData
	{
	TBuf8< KMaxLengthOfHost > iRemoteHost;
	TUint iRemotePort;
	TBuf8< KMaxLengthOfSessionId > iRemoteSessionID;
	} TConnectMSRPData;

	
// for listening parameters
typedef struct TListenMSRPData
	{
	TBuf8< KMaxLengthOfHost > iRemoteHost;
	TUint iRemotePort;
	TBuf8< KMaxLengthOfSessionId > iRemoteSessionID;
	TBuf8< KMaxLengthOfIncomingMessageExt > iExtMessageBuffer;
    TBuf8< KMaxLengthOfSessionId > iMessageId;
	TBool iIsMessage;
	TBool iIsProgress;
    TInt iBytesRecvd;
    TInt iTotalBytes;
	TInt iStatus; 
	} TListenMSRPData;

	
// for sending a message
typedef struct TSendMSRPData
	{
	TBuf8< KMaxLengthOfIncomingMessageExt > iExtMessageBuffer;
	} TSendMSRPData;

    
// for listening to result of a sent message
typedef struct TListenSendResultMSRPData
	{
	TBuf8< KMaxLengthOfSessionId > iSessionID;
	TBuf8< KMaxLengthOfSessionId > iMessageId;
    TBool iIsProgress;
    TInt iBytesSent;
    TInt iTotalBytes;
	TInt iStatus; // error codes propagated to the client
	} TListenSendResultMSRPData;


#endif // __MSRPCOMMON_H__

// End of File
