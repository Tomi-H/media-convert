#include "ffmpeg.h"
#include "MediaSource.h"
#include "AudioResample.h"
#include "VideoScale.h"
#include <stdio.h>

void pgm_save(unsigned char * buf, int wrap, int xsize, int ysize, char * filename)
{
    FILE * f = fopen(filename, "w");

    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (int i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}


int main(int argc, char ** argv)
{
    av_register_all();
    MediaSource ms;
    AVPacket pkt;
    AVFrame * frame = avcodec_alloc_frame();
   // AVFrame * dst_frame = avcodec_alloc_frame();
    AVPicture picture;
    int index = 1;
    char str[100] = {0};

    ms.open(argv[1], 1);
    VideoScale sc;
    sc.init(ms.getWidth(), ms.getHeight(), ms.getPixFormat(), 352, 288,
            ms.getPixFormat(), 0, NULL, NULL, NULL);

    avpicture_alloc(&picture, ms.getPixFormat(), 352, 288);

    while (ms.read(&pkt) == 0) {
        int ret = ms.decode(frame, &pkt);
        if (ret > 0) {
            if (ms.isVideoPacket(&pkt)) {
                if (index > 30)
                    continue;
                snprintf(str, 100, "%02d.pgm", index++);
                sc.scale(frame->data, frame->linesize, 0, ms.getHeight(),
                         picture.data, picture.linesize);
                pgm_save(picture.data[0], picture.linesize[0], 352, 288, str);
            }

            if (ms.isAudioPacket(&pkt)) {
                /*
                static AudioResample rsp;
                rsp.init(ms.getChannelLayout(), ms.getSampleFormat(), ms.getSampleRate(),
                         ms.getChannelLayout(), ms.getSampleFormat(), ms.getSampleRate());
                         */

            }
        }
    }

    ms.close();

    return 0;
}
