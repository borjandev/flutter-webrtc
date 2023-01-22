#import <AVFoundation/AVFoundation.h>
#import "FlutterRTCAudioSink.h"
#import "RTCAudioSource+Private.h"
#include "media_stream_interface.h"
#include "audio_sink_bridge.cpp"
#import <mach/mach_time.h>

@implementation FlutterRTCAudioSink {
    AudioSinkBridge *_bridge;
    webrtc::AudioSourceInterface* _audioSource;
}

@synthesize firstAudioSampleTime;
@synthesize referenceSampleTime;

- (instancetype) initWithAudioTrack:(RTCAudioTrack* )audio {
    self = [super init];
    rtc::scoped_refptr<webrtc::AudioSourceInterface> audioSourcePtr = audio.source.nativeAudioSource;
    _audioSource = audioSourcePtr.get();
    _bridge = new AudioSinkBridge((void*)CFBridgingRetain(self));
    _audioSource->AddSink(_bridge);
    firstAudioSampleTime = CMTimeMake(1,1);
    referenceSampleTime = CMTimeMake(1,1);
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
            AudioBufferList audioBufferList;
               AudioBuffer audioBuffer;
               audioBuffer.mData = (void*) audio_data;
               audioBuffer.mDataByteSize = bits_per_sample / 8 * UInt32(number_of_channels) * UInt32(number_of_frames);
               audioBuffer.mNumberChannels = UInt32(number_of_channels);
               audioBufferList.mNumberBuffers = 1;
               audioBufferList.mBuffers[0] = audioBuffer;
               AudioStreamBasicDescription audioDescription;
               audioDescription.mBytesPerFrame = bits_per_sample / 8 * UInt32(number_of_channels);
               audioDescription.mBitsPerChannel = bits_per_sample;
               audioDescription.mBytesPerPacket = bits_per_sample / 8 * UInt32(number_of_channels);
               audioDescription.mChannelsPerFrame = UInt32(number_of_channels);
               audioDescription.mFormatID = kAudioFormatLinearPCM;
               audioDescription.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;
               audioDescription.mFramesPerPacket = 1;
               audioDescription.mReserved = 0;
               audioDescription.mSampleRate = sample_rate;
               CMAudioFormatDescriptionRef formatDesc;
               CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &audioDescription, 0, nil, 0, nil, nil, &formatDesc);
            if (formatDesc != NULL) {
                sink.format =formatDesc;
            } else {
                // Handle the error
                NSLog(@"Error creating audio format description:");
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
        
        timingInfo.decodeTimeStamp = kCMTimeInvalid;
        if (sink.firstAudioSampleTime.value == 1) {
            mach_timebase_info_data_t timeInfo;
            mach_timebase_info(&timeInfo);
            CMTime time = CMTimeMake(mach_absolute_time() * timeInfo.numer / timeInfo.denom, 1000000000);
            sink.firstAudioSampleTime = time;
            timingInfo.presentationTimeStamp = time;
        } 
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
