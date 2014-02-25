/*
 * VideoScale.h
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#ifndef VIDEOSCALE_H_
#define VIDEOSCALE_H_
#include "ffmpeg.h"

class VideoScale
{
    public:
        VideoScale();
        virtual ~VideoScale();

        int init(int srcW, int srcH, AVPixelFormat srcFormat,
                 int dstW, int dstH, AVPixelFormat dstFormat,
                 int flags,  SwsFilter * srcFilter,
                 SwsFilter * dstFilter, const double * param);
        int scale(const uint8_t * const srcSlice[], const int srcStride[],
                  const int srcSliceY, int srcSliceH, uint8_t * const dst[],
                  const int dstStride[]);
        void release();
    private:
        struct SwsContext * mSwsCtx;
       /*
        int mSrcWidth;
        int mSrcHeight;
        AVPixelFormat mSrcPixFmt;
        int mDstWidth;
        int mDstHeight;
        AVPixelFormat mDstPixFmt;
        */
};

#endif /* VIDEOSCALE_H_ */
