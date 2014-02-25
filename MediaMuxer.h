#ifndef MEDIA_MUXER_H_
#define MEDIA_MUXER_H_
#include "ffmpeg.h"
#include "MediaEncode.h"
#include "AudioEncode.h"
#include "VideoEncode.h"

class MediaMuxer
{
    public:
        MediaMuxer();
        virtual ~MediaMuxer();

        int open(const char * filename, bool show = false);
        int open(const char * filename, const AVFormatContext * fmt_ctx, bool show = false);
        int write(uint8_t ** data, int size, AVMediaType type);
        int write(AVPacket * pkt, AVRational time_base, AVMediaType type);
        void close();


        void setChannelLayout(uint64_t channel_layout);
        void setSampleFormat(AVSampleFormat sample_fmt);
        void setSampleRate(int sample_rate);

        uint64_t getChannelLayout();
        AVSampleFormat getSampleFormat();
        int getSampleRate();

        void setWidth(int width);
        void setHeight(int height);
        void setPixFmt(AVPixelFormat pix_fmt);

        int getWidth();
        int getHeight();
        AVPixelFormat getPixFmt();

    private:
        int writeAudio(uint8_t ** data, int size);
        int writeAudio(AVPacket * pkt, AVRational time_base);
        int writeVideo(uint8_t ** data, int size);
        int writeVideo(AVPacket * pkt, AVRational time_base);

    private:
        AVFormatContext * mFmtCtx;
        AVOutputFormat * mOutFmt;
        int mAudioStreamIndex;
        int mVideoStreamIndex;
        AVStream * mAudioStream;
        AVStream * mVideoStream;
        MediaEncode * mAudioEncoder;
        MediaEncode * mVideoEncoder;
};



#endif  /* MEDIA_MUXER_H_ */
