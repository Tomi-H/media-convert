#include "MediaMuxer.h"
#include <assert.h>

MediaMuxer::MediaMuxer()
{
    mFmtCtx = NULL;
    mOutFmt = NULL;
    mAudioStream = NULL;
    mVideoStream = NULL;
    mAudioStreamIndex = -1;
    mVideoStreamIndex = -1;
    mAudioEncoder = NULL;
    mVideoEncoder = NULL;
}

MediaMuxer::~MediaMuxer()
{
}

int MediaMuxer::open(const char * filename, bool show)
{
    mOutFmt = av_guess_format(NULL, filename, NULL);
    if (!mOutFmt) {
        fprintf(stderr, "Could not find correct output format.\n");
        exit(1);
    }

    int flags = 0;
    if (mOutFmt->flags & AVFMT_GLOBALHEADER)
        flags |= CODEC_FLAG_GLOBAL_HEADER;

    mFmtCtx = avformat_alloc_context();
    mFmtCtx->oformat = mOutFmt;
    if (mOutFmt->audio_codec) {
        mAudioEncoder = new AudioEncode;
        mAudioEncoder->open(mOutFmt->audio_codec, flags);
        mAudioStream = avformat_new_stream(mFmtCtx, mAudioEncoder->getCodec());
        if (!mAudioStream) {
            fprintf(stderr, "create encode audio stream failed.\n");
            return -1;
        }

        mAudioStreamIndex = mFmtCtx->nb_streams - 1;
        mAudioStream->id = mAudioStreamIndex;
    }

    if (mOutFmt->video_codec) {
        mVideoEncoder = new VideoEncode;
        mVideoEncoder->open(mOutFmt->video_codec, flags);
        mVideoStream = avformat_new_stream(mFmtCtx, mVideoEncoder->getCodec());
        if (!mVideoStream) {
            fprintf(stderr, "create encode video stream failed.\n");
            return -1;
        }
        mVideoStreamIndex = mFmtCtx->nb_streams - 1;
        mVideoStream->id = mVideoStreamIndex;
    }

    if (show)
        av_dump_format(mFmtCtx, 0, filename, 1);

    if (!(mOutFmt->flags & AVFMT_NOFILE)) {
        int ret = avio_open(&mFmtCtx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Couldn't open input file: %s.\n", filename);
            return -1;
        }
    }

    int ret = avformat_write_header(mFmtCtx, NULL);
    if (ret < 0) {
        fprintf(stderr, "write file header failed.\n");
        return -1;
    }

    return 0;
}

int MediaMuxer::open(const char * filename, const AVFormatContext * fmt_ctx, bool show)
{
    assert(filename);
    assert(fmt_ctx);

    if (avformat_alloc_output_context2(&mFmtCtx, NULL, NULL, filename) < 0) {
        fprintf(stderr, "Allocate output context failed.\n");
        return -1;
    }
    mOutFmt = mFmtCtx->oformat;

    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        AVStream * in_st = fmt_ctx->streams[i];
        AVStream * out_st = avformat_new_stream(mFmtCtx, in_st->codec->codec);
        if (!out_st) {
            fprintf(stderr, "allocate new stream failed.\n");
            return -1;
        }

        int ret = avcodec_copy_context(out_st->codec, in_st->codec);
        if (ret < 0) {
            fprintf(stderr, "copty codec context failed: %s\n", av_err2str(ret));
            return -1;
        }

        if (mOutFmt->flags & AVFMT_GLOBALHEADER)
            out_st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    if (show)
        av_dump_format(mFmtCtx, 0, filename, 1);

    mAudioStreamIndex = av_find_best_stream(mFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL,0);
    if (mAudioStreamIndex < 0) {
        fprintf(stderr, "find audio stream failed.\n");
    } else {
        mAudioStream = mFmtCtx->streams[mAudioStreamIndex];
    }

    mVideoStreamIndex = av_find_best_stream(mFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (mVideoStreamIndex < 0) {
        fprintf(stderr, "find video stream failed.\n");
    } else {
        mVideoStream = mFmtCtx->streams[mVideoStreamIndex];
    }

    if (!(mOutFmt->flags & AVFMT_NOFILE)) {
        int ret = avio_open(&mFmtCtx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Couldn't open input file: %s.\n", filename);
            return -1;
        }
    }

    int ret = avformat_write_header(mFmtCtx, NULL);
    if (ret < 0) {
        fprintf(stderr, "write file header failed.\n");
        return -1;
    }

    return 0;
}

int MediaMuxer::write(uint8_t ** data, int size, AVMediaType type)
{
    // synchronization of video and audio is needed
    if (type == AVMEDIA_TYPE_AUDIO) {
        return writeAudio(data, size);
    } else if (type == AVMEDIA_TYPE_VIDEO) {
        return writeVideo(data, size);
    }

    return 0;
}

int MediaMuxer::writeAudio(uint8_t ** data, int size)
{

    return 0;
}

int MediaMuxer::writeVideo(uint8_t ** data, int size)
{

    return 0;
}

int MediaMuxer::writeAudio(AVPacket * pkt, AVRational time_base)
{
    pkt->pts = av_rescale_q(pkt->pts, time_base, mAudioStream->time_base);
    pkt->dts = av_rescale_q(pkt->dts, time_base, mAudioStream->time_base);
    pkt->duration = av_rescale_q(pkt->duration, time_base, mAudioStream->time_base);
    pkt->pos = -1;

    int ret = av_interleaved_write_frame(mFmtCtx, pkt);
    if (ret < 0) {
        fprintf(stderr, "remux audio packet failed: %s.\n", av_err2str(ret));
        return -1;
    }

    return 0;
}

int MediaMuxer::writeVideo(AVPacket * pkt, AVRational time_base)
{
    pkt->pts = av_rescale_q(pkt->pts, time_base, mVideoStream->time_base);
    pkt->dts = av_rescale_q(pkt->dts, time_base, mVideoStream->time_base);
    pkt->duration = av_rescale_q(pkt->duration, time_base, mVideoStream->time_base);
    pkt->pos = -1;

    int ret = av_interleaved_write_frame(mFmtCtx, pkt);
    if (ret < 0) {
        fprintf(stderr, "remux video packet failed: %s.\n", av_err2str(ret));
        return -1;
    }

    return 0;
}

int MediaMuxer::write(AVPacket * pkt, AVRational time_base, AVMediaType type)
{
    int ret = 0;

    switch(type) {
        case AVMEDIA_TYPE_AUDIO:
            ret = writeAudio(pkt, time_base);
            break;
        case AVMEDIA_TYPE_VIDEO:
            ret = writeVideo(pkt, time_base);
            break;
        default:
            break;
    }

    return ret;
}

void MediaMuxer::close()
{
    if (mFmtCtx && !(mFmtCtx->flags & AVFMT_NOFILE))
        avio_close(mFmtCtx->pb);
    avformat_free_context(mFmtCtx);

    if (mAudioEncoder) {
        mAudioEncoder->close();
        delete mAudioEncoder;
    }

    if (mVideoEncoder) {
        mVideoEncoder->close();
        delete mVideoEncoder;
    }
}

void MediaMuxer::setChannelLayout(uint64_t channel_layout)
{
    mAudioEncoder->setChannelLayout(channel_layout);
}

void MediaMuxer::setSampleFormat(AVSampleFormat sample_fmt)
{
    mAudioEncoder->setSampleFormat(sample_fmt);
}

void MediaMuxer::setSampleRate(int sample_rate)
{
    mAudioEncoder->setSampleRate(sample_rate);
}

void MediaMuxer::setWidth(int width)
{
    mVideoEncoder->setWidth(width);
}

void MediaMuxer::setHeight(int height)
{
    mVideoEncoder->setHeight(height);
}

void MediaMuxer::setPixFmt(AVPixelFormat pix_fmt)
{
    mVideoEncoder->setPixFmt(pix_fmt);
}

uint64_t MediaMuxer::getChannelLayout()
{
    return mAudioEncoder->getChannelLayout();
}

AVSampleFormat MediaMuxer::getSampleFormat()
{
    return mAudioEncoder->getSampleFormat();
}

int MediaMuxer::getSampleRate()
{
    return mAudioEncoder->getSampleRate();
}

int MediaMuxer::getWidth()
{
    return mVideoEncoder->getWidth();
}

int MediaMuxer::getHeight()
{
    return mVideoEncoder->getHeight();
}

AVPixelFormat MediaMuxer::getPixFmt()
{
    return mVideoEncoder->getPixFmt();
}
