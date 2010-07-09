!/**
! * @publishedAll
! * @released
!*/
# SIP String table
fstringtable SipStrConsts
# Strings are case-sensitive
#
#
!// General constants
EEmpty
ESIP SIP
ESip sip
ESips sips
EDefaultVersionNumber 2.0
EDefaultProtocolVersion SIP/2.0
EUDP UDP
EUdp udp
ETCP TCP
ETcp tcp
ELocalHost localhost
!// SIP methods
ERegister REGISTER
EInvite INVITE
ECancel CANCEL
EAck ACK
EBye BYE
EPrack PRACK
EUpdate UPDATE
ESubscribe SUBSCRIBE
ENotify NOTIFY
ERefer REFER
EMessage MESSAGE
EOptions OPTIONS
EInfo INFO
#
!// SIP reason phrases:
EPhraseOk OK
EPhraseTrying Trying
EPhraseRinging Ringing
EPhraseSessionProgress Session Progress
EPhraseBadRequest Bad Request
EPhraseUnauthorized Unauthorized
EPhraseForbidden Forbidden
EPhraseNotFound Not Found
EPhraseMethodNotAllowed Method Not Allowed
EPhraseProxyAuthenticationRequired Proxy Authentication Required
EPhraseRequestTimeout Request Timeout
EPhraseUnsupportedMediaType Unsupported Media Type
EPhraseUnsupportedURIScheme Unsupported URI Scheme
EPhraseBadExtension Bad Extension
EPhraseExtensionRequired Extension Required
EPhraseIntervalTooBrief Interval Too Brief
EPhraseTemporarilyNotAvailable Temporarily Not Available
EPhraseCallTransactionDoesNotExist Call/Transaction Does Not Exist
EPhraseLoopDetected Loop Detected
EPhraseTooManyHops Too Many Hops
EPhraseBusyHere Busy Here
EPhraseRequestTerminated Request Terminated
EPhraseNotAcceptableHere Not Acceptable Here
EPhraseBadEvent Bad Event
EPhraseSecurityAgreementRequired Security Agreement Required
EPhraseServerInternalError Server Internal Error
EPhraseServiceUnavailable Service Unavailable
EPhraseVersionNotSupported Version Not Supported
EPhraseDecline Decline
#
!// SIP parameters:
ETtl ttl
EMaddr maddr
EReceived received
EBranch branch
EQ q
EExpires expires
ETag tag
ETransport transport
EUser user
EMethod method
ELr lr
EDigestAlgorithm d-alg
EDigestQop d-qop
EDigestVerify d-ver
EHandling handling
EReason reason
ERetryAfter retry-after
EId id
ENextNonce nextnonce
EQop qop
EResponseAuth rspauth
ECNonce cnonce
ENonceCount nc
ERealm realm
EDomain domain
ENonce nonce
EOpaque opaque
EStale stale
EAlgorithm algorithm
EUserName username
EUri uri
EResponse response
EComp comp
ESigComp sigcomp
ESecAgreeTag sec-agree
EDuration duration
EAudio audio
EAutomata automata
EClass class
EDuplex duplex
EData data
EControl control
EMobility mobility
EDescription description
EEvents events
EPriority priority
EMethods methods
ESchemes schemes
EApplication application
EVideo video
ELanguage language
EType type
EIsfocus isfocus
EActor actor
EText text
EExtensions extensions
ERequire require
EExplicit explicit
ECgi3gpp cgi-3gpp
EUtranCellId3gpp utran-cell-id-3gpp
EAuts auts
#
!// SIP header names:
EToHeader To
EToHeaderCompact t
EFromHeader From
EFromHeaderCompact f
ECSeqHeader CSeq
ECallIDHeader Call-ID
ECallIDHeaderCompact i
EContentLengthHeader Content-Length
EContentLengthHeaderCompact l
EViaHeader Via
EViaHeaderCompact v
ERouteHeader Route
ERecordRouteHeader Record-Route
EServiceRouteHeader Service-Route
EContactHeader Contact
EContactHeaderCompact m
EContentTypeHeader Content-Type
EContentTypeHeaderCompact c
EExpiresHeader Expires
EMinExpiresHeader Min-Expires
EMaxForwardsHeader Max-Forwards
EAllowHeader Allow
EProxyRequireHeader Proxy-Require
ERequireHeader Require
ESupportedHeader Supported
ESupportedHeaderCompact k
EUnsupportedHeader Unsupported
EContentEncodingHeader Content-Encoding
EContentEncodingHeaderCompact e
EReferToHeader Refer-To
EReferToHeaderCompact r
ESecurityClientHeader Security-Client
ESecurityServerHeader Security-Server
ESecurityVerifyHeader Security-Verify
EAuthenticationInfoHeader Authentication-Info
EAcceptHeader Accept
EAcceptEncodingHeader Accept-Encoding
EAcceptLanguageHeader Accept-Language
EContentDispositionHeader Content-Disposition
ESubscriptionStateHeader Subscription-State
EAllowEventsHeader Allow-Events
EAllowEventsHeaderCompact u
EEventHeader Event
EEventHeaderCompact o
EAuthorizationHeader Authorization
EProxyAuthenticateHeader Proxy-Authenticate
EProxyAuthorizationHeader Proxy-Authorization
EWWWAuthenticateHeader WWW-Authenticate
EReplyToHeader Reply-To
ERAckHeader RAck
ERSeqHeader RSeq
ETimestampHeader Timestamp
ERetryAfterHeader Retry-After
EAcceptContactHeader Accept-Contact
EAcceptContactHeaderCompact a
EPAssociatedURIHeader P-Associated-URI
EPAccessNetworkInfoHeader P-Access-Network-Info
!// SIP reason phrases:
EPhraseNotImplemented Not Implemented
!// TLS transport for Via-header
ETLS TLS
!// Algorithms supported by SIPSec Digest plugin
EMD5 MD5
EAKAv1MD5 AKAv1-MD5
!// rport parameter
ERport rport
