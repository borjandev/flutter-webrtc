#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>
#import <WebRTC/WebRTC.h>

@interface FlutterRTCAudioSink : NSObject

@property (nonatomic, copy) void (^bufferCallback)(CMSampleBufferRef);
@property (nonatomic) CMAudioFormatDescriptionRef format;
@property (nonatomic) CMTime externalPresentationTimestamp;
@property (nonatomic) CMTime externalDecodeTimestamp;
@property (nonatomic) BOOL alreadySetTimestamps;
@property (nonatomic) int currentBufferIteration;

- (instancetype) initWithAudioTrack:(RTCAudioTrack*)audio;

- (void) close;

@end
