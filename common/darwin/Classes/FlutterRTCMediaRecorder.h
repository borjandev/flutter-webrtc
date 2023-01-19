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


@interface AudioCapture : NSObject

@property (nonatomic, strong) AVAudioEngine * _Nonnull audioEngine;
@property (nonatomic, strong) AVAssetWriter * _Nonnull assetWriter;
@property (nonatomic, strong) AVAudioFormat * _Nullable audioFormat;
@property (nonatomic, strong) AVAssetWriterInput * _Nonnull audioInput;
@property (nonatomic, assign) BOOL isAssetWriterRecordingToFile;

- (instancetype _Nonnull )initWithAssetWriter:(AVAssetWriter *_Nonnull)assetWriter;
- (void)startRecording;
- (void)stopRecording;

@end


@interface AppRecorder : NSObject

@property (nonatomic, strong) AVAssetWriter * _Nullable inkAppRecordingWriter;
@property (nonatomic, strong) AVAssetWriterInput * _Nonnull inkAppVideoInput;
@property (nonatomic, assign) BOOL isRecordingToFile;
@property (nonatomic, assign) BOOL shouldSkipFrame;
@property (nonatomic, strong) AVAssetWriterInput * _Nonnull inkAppAudioInput;
@property (nonatomic, strong) NSURL * _Nullable inkAppRecordingFileURL;
@property (nonatomic, strong) RPScreenRecorder * _Nullable replayKitRecorder;
@property (nonatomic, strong) AudioCapture * _Nullable audioCapture;
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

