# MSRP String table
fstringtable MSRPStrConsts
# Strings are case-sensitive
#
#
!// General constants
EEmpty
EMSRP MSRP
EUDP UDP
EUdp udp
ETCP TCP
ETcp tcp
!// MSRP methods
ESend SEND
EReport REPORT
#
!// MSRP reason phrases:
EAllOk OK
EUnintelligibleRequest Unintelligible Request
EActionNotAllowed Action Not Allowed
ETimeout Timeout
EStopSending Stop Sending
EMimeNotUnderstood Mime Not Understood
EParameterOutOfBounds Parameter Out of Bounds
ESessionDoesNotExist Session Does Not Exist
EUnknownRequestMethod Unknown Request
ESessionAlreadyBound Session Already in Use
#
!// MSRP URL scheme
EMSRPUrlScheme msrp
EMSRPUrlSchemeSecure msrps
#
!// MSRP transport protocol
ETcpMsrp TCP/MSRP
ETcpMsrpTls TCP/TLS/MSRP
#
!// MSRP SDP media type
EMessageMediaType message
#
!// MSRP SDP attributes
EAcceptTypes accept-types
EWrappedTypes accept-wrapped-types
EMaxSize max-size
EPath path
EFileName filename
EFileType filetype
EDisposition disposition
EFileSize filesize
EFileSelector file-selector
EFileDisposition file-disposition
EFileTransferId file-transfer-id
EFileTransfer file-transfer
EIcon icon
EHash hash
#
!// RFC 4145 additional attributes, used with MSRP
EMTSetup setup
EMTConnection connection
#
!// MSRP header names:
EToPath To-Path
EFromPath From-Path
ESuccessReport Success-Report
EFailureReport Failure-Report
EByteRange Byte-Range
EStatus Status
EContentType Content-Type
EMessageId Message-ID
EContentDisposition Content-Disposition
#
!// MSRP Report variable
EYes yes
ENo no
EPartial partial
!// MSRP status code field namespace
EStatusNameSpace 000
#
!// Feature tags as defined in GSMA Image Share Definition 1.0
EFeatureTagCsVoice +g.3gpp.cs-voice
EFeatureTagMmTel +g.ims.icsi.mmtel
EFeatureTagMessaging +g.oma.sip-im
EFeatureTagImageShare +g.3gpp.iari-ref%3Aurn%3Aurn-xxx%3A3gpp-application.ims.iari.gsma-is
EFeatureTagImageShareName +g.3gpp.app_ref
EFeatureTagImageShareValue \"urn%3Aurn-xxx%3A3gpp-application.ims.iari.gsma-is\"
