#if TARGET_OS_IPHONE
#import <Flutter/Flutter.h>
#elif TARGET_OS_OSX
#import <FlutterMacOS/FlutterMacOS.h>
#endif

#import <Foundation/Foundation.h>
#import <WebRTC/WebRTC.h>
#import <ReplayKit/ReplayKit.h>
#import <FlutterRTCAudioSink.h>

@class FlutterRTCVideoRenderer;
@class FlutterRTCFrameCapturer;
@class FlutterRTCMediaRecorder;

typedef void (^CompletionHandler)(void);

typedef void (^CapturerStopHandler)(CompletionHandler handler);

@interface FlutterWebRTCPlugin : NSObject <FlutterPlugin,
                                           RTCPeerConnectionDelegate,
                                           FlutterStreamHandler
#if TARGET_OS_OSX
                                           ,
                                           RTCDesktopMediaListDelegate,
                                           RTCDesktopCapturerDelegate
#endif
                                           >

@property (nonatomic, strong) RTCPeerConnectionFactory *peerConnectionFactory;
@property (nonatomic, strong) NSMutableDictionary<NSString *, RTCPeerConnection *> *peerConnections;
@property (nonatomic, strong) NSMutableDictionary<NSString *, RTCMediaStream *> *localStreams;
@property (nonatomic, strong) NSMutableDictionary<NSString *, RTCMediaStreamTrack *> *localTracks;
@property (nonatomic, strong) NSMutableDictionary<NSNumber *, FlutterRTCVideoRenderer *> *renders;
@property (nonatomic, strong) NSMutableDictionary<NSNumber *, FlutterRTCMediaRecorder *> *recorders;
@property (nonatomic, strong) NSMutableDictionary<NSString *, CapturerStopHandler> *videoCapturerStopHandlers;

#if TARGET_OS_IPHONE
@property(nonatomic, retain) UIViewController* viewController; /*for broadcast or ReplayKit */
#endif

@property(nonatomic, strong) FlutterEventSink eventSink;
@property(nonatomic, strong) NSObject<FlutterBinaryMessenger>* messenger;
@property(nonatomic, strong) RTCCameraVideoCapturer* videoCapturer;
@property(nonatomic, strong) FlutterRTCFrameCapturer* frameCapturer;
@property(nonatomic, strong) AVAudioSessionPort preferredInput;
@property(nonatomic) BOOL _usingFrontCamera;
@property(nonatomic) int _targetWidth;
@property(nonatomic) int _targetHeight;
@property(nonatomic) int _targetFps;

- (RTCMediaStream*)streamForId:(NSString*)streamId peerConnectionId:(NSString*)peerConnectionId;
- (NSDictionary*)mediaStreamToMap:(RTCMediaStream*)stream ownerTag:(NSString*)ownerTag;
- (NSDictionary*)mediaTrackToMap:(RTCMediaStreamTrack*)track;
- (NSDictionary*)receiverToMap:(RTCRtpReceiver*)receiver;
- (NSDictionary*)transceiverToMap:(RTCRtpTransceiver*)transceiver;

- (BOOL)hasLocalAudioTrack;
- (void)ensureAudioSession;
- (void)deactiveRtcAudioSession;

@end


@interface LocalMicrophoneAudioRecorder : NSObject

@property (nonatomic, strong) AVAssetWriter * _Nonnull appRecordingWriter;
@property (nonatomic, strong) AVAssetWriterInput * _Nonnull localMicrophoneAudioInput;
@property (nonatomic, strong) AVCaptureSession * _Nonnull localMicrophoneCaptureSession;
@property (nonatomic, assign) BOOL isAssetWriterRecordingToFile;

- (instancetype _Nonnull )initWithAppRecordingWriter:(AVAssetWriter *_Nonnull)inkAppRecordingWriter;
- (void)startRecording;
- (void)stopRecording;

@end

@interface AppRecorder : NSObject

@property (nonatomic, strong) AVAssetWriter * _Nullable appRecordingWriter;
@property (nonatomic, strong) AVAssetWriterInput * _Nonnull appVideoInput;
@property (nonatomic, assign) BOOL isRecordingToFile;
@property (nonatomic, assign) BOOL shouldSkipFrame;
@property (nonatomic, strong) AVAssetWriterInput * _Nonnull peerAudioInput;
@property (nonatomic, strong) NSURL * _Nullable appRecordingFileURL;
@property (nonatomic, strong) RPScreenRecorder * _Nullable replayKitRecorder;
@property (nonatomic, strong) LocalMicrophoneAudioRecorder * _Nullable audioCapture;
@property (nonatomic, strong) FlutterRTCAudioSink * _Nullable audioSink;


- (instancetype _Nonnull )init;
- (void)startWithFileName:(NSString *_Nonnull)fileName recordingHandler:(void (^_Nonnull)(NSError * _Nullable))recordingHandler;
- (void)stopRecordingWithHandler:(void (^_Nullable)(NSError * _Nullable))handler;

@end


@interface AppRecorderCoordinator : NSObject

@property (nonatomic, strong) AppRecorder * _Nonnull appRecorder;
@property (nonatomic, copy) void (^ _Nullable done)(NSError * _Nullable);

- (instancetype _Nonnull )init;
- (void)startWithFileName:(NSString *_Nonnull)fileName recordingHandler:(void (^_Nonnull)(NSError * _Nullable))recordingHandler onCompletion:(void (^_Nonnull)(NSError * _Nullable))onCompletion;
- (void)resume;

@end

NSError * _Nullable startAudioSessionIfNotStarted(void);
