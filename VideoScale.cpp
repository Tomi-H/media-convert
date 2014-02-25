/*
 * VideoScale.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: hemiao
 */

#include "VideoScale.h"

VideoScale::VideoScale()
{
    mSwsCtx = NULL;
}

VideoScale::~VideoScale()
{
}

int VideoScale::init(int srcW, int srcH, AVPixelFormat srcFormat,
                     int dstW, int dstH, AVPixelFormat dstFormat,
                     int flags,SwsFilter * srcFilter,
                     SwsFilter * dstFilter,const double * param)
{
    if (!mSwsCtx)
        mSwsCtx = sws_getCachedContext(mSwsCtx,srcW, srcH, srcFormat, dstW, dstH,
                                       dstFormat, flags, srcFilter, dstFilter, param);
    if (!mSwsCtx) {
        fprintf(stderr, "get scale context failed.\n");
        return -1;
    }

    return 0;
}


int VideoScale::scale(const uint8_t * const srcSlice[], const int srcStride[],
                      const int srcSliceY, int srcSliceH, uint8_t * const dst[],
                      const int dstStride[])
{
    return sws_scale(mSwsCtx, srcSlice, srcStride, srcSliceY, srcSliceH, dst, dstStride);
}

void VideoScale::release()
{
    sws_freeContext(mSwsCtx);
}
