#if TARGET_OS_IPHONE
#import <Flutter/Flutter.h>
#elif TARGET_OS_OSX
#import <FlutterMacOS/FlutterMacOS.h>
#endif
#import <WebRTC/WebRTC.h>
#import <ReplayKit/ReplayKit.h>
#import <FlutterRTCAudioSink.h>

@import Foundation;
@import AVFoundation;

@interface FlutterRTCMediaRecorder : NSObject<RTCVideoRenderer>

@property (nonatomic, strong) RTCVideoTrack * _Nullable videoTrack;
@property (nonatomic, strong) NSURL * _Nonnull output;
@property (nonatomic, strong) AVAssetWriter * _Nullable assetWriter;
@property (nonatomic, strong) AVAssetWriterInput * _Nullable writerInput;

- (instancetype _Nonnull) initWithVideoTrack:(RTCVideoTrack * _Nullable)video
                             rotationDegrees:(NSNumber * _Nonnull)rotation
                                  audioTrack:(RTCAudioTrack * _Nullable)audio
                                  outputFile:(NSURL * _Nonnull)out;

- (void) changeVideoTrack:(RTCVideoTrack * _Nonnull) track;

- (void) stop:(_Nonnull FlutterResult) result;

@end

@interface InkAudioRecorder : NSObject <AVAudioRecorderDelegate>

@property (nonatomic, strong) AVAudioRecorder * _Nullable audioRecorder;
@property (nonatomic, strong) NSURL * _Nonnull filename;
@property (nonatomic, strong) NSDictionary * _Nullable settings;

- (instancetype _Nonnull )initWithURL:(NSURL *_Nonnull)url;
- (void)startRecording;
- (void)stopRecording;

@end

@interface AppRecorder : NSObject

@property (nonatomic, strong) AVAssetWriter * _Nullable inkAppVideoRecordingWriter;
@property (nonatomic, strong) AVAssetWriter * _Nullable inkAppAudioRecordingWriter;
@property (nonatomic, strong) AVAssetWriterInput * _Nonnull inkAppVideoInput;

@property (nonatomic, assign) BOOL hasVideoWriterStartedWriting;
@property (nonatomic, assign) BOOL hasAudioWriterStartedWriting;

@property (nonatomic, assign) BOOL isRecordingVideoToFile;
@property (nonatomic, assign) BOOL isRecordingAudioToFile;
@property (nonatomic, assign) BOOL shouldSkipFrame;
@property (nonatomic, strong) AVAssetWriterInput * _Nonnull inkAppAudioInput;
@property (nonatomic, strong) NSURL * _Nullable inkAppRecordingFileURL;
@property (nonatomic, strong) NSURL * _Nullable inkAppVideoRecordingFileURL;
@property (nonatomic, strong) NSURL * _Nullable inkAppRemoteAudioRecordingFileURL;
@property (nonatomic, strong) NSURL * _Nullable inkAppLocalAudioRecordingFileURL;
@property (nonatomic, strong) NSURL * _Nullable inkAppSPECIALocalAudioRecordingFileURL;
@property (nonatomic, strong) RPScreenRecorder * _Nullable replayKitRecorder;
@property (nonatomic, strong) InkAudioRecorder * _Nullable audioCapture;
@property (nonatomic, strong) FlutterRTCAudioSink * _Nullable audioSink;


- (instancetype _Nonnull )init;
- (void)startWithFileName:(NSString *_Nonnull)fileName recordingHandler:(void (^_Nonnull)(NSError * _Nullable))recordingHandler;
- (void)stopRecordingWithHandler:(void (^_Nullable)(NSError * _Nullable))handler;

@end

@interface InkAppRecorder : NSObject

@property (nonatomic, strong) AppRecorder * _Nonnull appRecorder;
@property (nonatomic, copy) void (^ _Nullable done)(NSError * _Nullable);

- (instancetype _Nonnull )init;
- (void)startWithFileName:(NSString *_Nonnull)fileName recordingHandler:(void (^_Nonnull)(NSError * _Nullable))recordingHandler onCompletion:(void (^_Nonnull)(NSError * _Nullable))onCompletion;
- (void)resume;

@end

NSError * _Nullable startAudioSessionIfNotStarted(void);



