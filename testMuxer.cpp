#include "MediaSource.h"
#include "MediaMuxer.h"

int main(int argc, char ** argv)
{
    if (argc < 3) {
        fprintf(stderr, "need at least 3 arguments.\n");
        exit(1);
    }

    av_register_all();
    MediaSource ms;
    ms.open(argv[1], 0);

    MediaMuxer muxer;
    muxer.open(argv[2], ms.getFmtCtx(), true);

    AVPacket pkt;

    while (!ms.read(&pkt)) {
        AVMediaType type = ms.getMediaType(&pkt);
        AVRational time_base;

        switch(type) {
            case AVMEDIA_TYPE_AUDIO:
                time_base = ms.getAudioStreamTimeBase();
                break;
            case AVMEDIA_TYPE_VIDEO:
                time_base = ms.getVideoStreamTimeBase();
                break;
            default:
                fprintf(stderr, "unsupported packet type.\n");
                return 0;
                break;
        }
        muxer.write(&pkt, time_base, ms.getMediaType(&pkt));
        if (pkt.data)
            av_free_packet(&pkt);
    }

    muxer.close();
    ms.close();

    return 0;
}
