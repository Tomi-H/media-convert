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
    MediaMuxer muxer;

    ms.open(argv[1], 0);
    muxer.open(argv[2], ms.getFmtCtx(), true);

    AVPacket pkt;
    AVFrame * frame = NULL;

    while (!ms.read(&pkt)) {
        AVMediaType type = ms.getMediaType(&pkt);
        int ret = ms.decode(frame, &pkt);
        if (ret < 0) {
            fprintf(stderr, "decode error.\n");
            exit(1);
        }

        if (ret == 0)
            continue;

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
