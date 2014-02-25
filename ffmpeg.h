#ifndef _FFMPEG_H_
#define _FFMPEG_H_

extern "C"
{
#ifdef __cplusplus
	#define __STDC_CONSTANT_MACROS
	#ifdef _STDINT_H
		#undef _STDINT_H
	#endif
	# include <stdint.h>
#endif

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/fifo.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/opt.h>
#include <libavutil/log.h>
#include <libavutil/attributes.h>
}

#endif
