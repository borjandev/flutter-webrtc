#import <AVFoundation/AVFoundation.h>
#import "FlutterRTCAudioSink.h"
#import "RTCAudioSource+Private.h"
#include "media_stream_interface.h"
#include "audio_sink_bridge.cpp"
#import <mach/mach_time.h>

@implementation FlutterRTCAudioSink {
    AudioSinkBridge *_bridge;
    webrtc::AudioSourceInterface* _audioSource;
    bool _isFirstAudioSample;
}

- (instancetype) initWithAudioTrack:(RTCAudioTrack* )audio {
    self = [super init];
    rtc::scoped_refptr<webrtc::AudioSourceInterface> audioSourcePtr = audio.source.nativeAudioSource;
    _audioSource = audioSourcePtr.get();
    _bridge = new AudioSinkBridge((void*)CFBridgingRetain(self));
    _audioSource->AddSink(_bridge);
    _isFirstAudioSample = true;
    return self;
}

- (void) close {
    _audioSource->RemoveSink(_bridge);
    delete _bridge;
    _bridge = nil;
    _audioSource = nil;
}

void RTCAudioSinkCallback (void *object, const void *audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels, size_t number_of_frames)
{
    FlutterRTCAudioSink* sink = (__bridge FlutterRTCAudioSink*)(object);
    if (sink.bufferCallback) {
        if (!sink.format) {
            // Create the audio format description
            AudioStreamBasicDescription asbd;
            asbd.mSampleRate = sample_rate;
            asbd.mFormatID = kAudioFormatLinearPCM;
            asbd.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
            asbd.mBitsPerChannel = bits_per_sample;
            asbd.mChannelsPerFrame = (UInt32)number_of_channels;
            asbd.mFramesPerPacket = 1;
            asbd.mBytesPerFrame = asbd.mBitsPerChannel * asbd.mChannelsPerFrame / 8;
            asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;
            CMAudioFormatDescriptionRef format = NULL;
            OSStatus status = CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &asbd, 0, NULL, 0, NULL, NULL, &format);
            if (status == noErr) {
                sink.format = format;
            } else {
                // Handle the error
                NSLog(@"Error creating audio format description: %d", (int)status);
                return;
            }
        }
        CMBlockBufferRef blockBuffer;
        OSStatus status = CMBlockBufferCreateWithMemoryBlock(NULL, (void*)audio_data, number_of_frames * CMAudioFormatDescriptionGetStreamBasicDescription(sink.format)->mBytesPerFrame, kCFAllocatorNull, NULL, 0, number_of_frames * CMAudioFormatDescriptionGetStreamBasicDescription(sink.format)->mBytesPerFrame, 0, &blockBuffer);
        if (status != noErr) {
            // Handle the error
            NSLog(@"Error creating block buffer: %d", (int)status);
            return;
        }
        
        
        CMSampleTimingInfo timingInfo;
        timingInfo.duration = CMTimeMake(number_of_frames, sample_rate);
        mach_timebase_info_data_t timeInfo;
        mach_timebase_info(&timeInfo);
        CMTime time = CMTimeMake(mach_absolute_time() * timeInfo.numer / timeInfo.denom, 1000000000);
        timingInfo.presentationTimeStamp = time;
        timingInfo.decodeTimeStamp = kCMTimeInvalid;
        CMSampleBufferRef sampleBuffer;
        status = CMSampleBufferCreate(kCFAllocatorDefault, blockBuffer, true, NULL, NULL, sink.format, number_of_frames, 1, &timingInfo, 0, NULL, &sampleBuffer);
        if (status != noErr) {
            // Handle the error
            NSLog(@"Error creating sample buffer: %d", (int)status);
            CFRelease(blockBuffer);
            return;
        }
        sink.bufferCallback(sampleBuffer);
        CFRelease(blockBuffer);
        CFRelease(sampleBuffer);
    }
   
}

@end
