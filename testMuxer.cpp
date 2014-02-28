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

        switch(type) {
            case AVMEDIA_TYPE_AUDIO:

                break;
            case AVMEDIA_TYPE_VIDEO:

                break;
            default:
                av_log(NULL, AV_LOG_INFO, "Unsupported packet type.\n");
                break;
        }
        muxer.write(frame->data, frame->nb_samples, type);
        if (pkt.data)
            av_free_packet(&pkt);
    }

    muxer.close();
    ms.close();

    return 0;
}
