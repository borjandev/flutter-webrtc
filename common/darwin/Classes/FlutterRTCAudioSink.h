#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>
#import <WebRTC/WebRTC.h>

@interface FlutterRTCAudioSink : NSObject

@property (nonatomic, copy) void (^bufferCallback)(CMSampleBufferRef);
@property (nonatomic) CMAudioFormatDescriptionRef format;
@property (nonatomic) CMTime firstAudioSampleTime;
@property (nonatomic) CMTime referenceSampleTime;

- (instancetype) initWithAudioTrack:(RTCAudioTrack*)audio;

- (void) close;

@end
