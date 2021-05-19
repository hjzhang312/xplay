#ifndef XFFMPEG_H
#define XFFMPEG_H

#include<QMutex>
#include <string>
#include <QString>

#ifdef __cplusplus             //告诉编译器，这部分代码按C语言的格式进行编译，而不是C++的
extern "C"{
#endif

#include "avcodec.h"
#include "avformat.h"
#include "swscale.h"
#include "avutil.h"
#include "swresample.h"
#include "avfilter.h"

#ifdef __cplusplus
}
#endif

class XFFmpeg
{
public:
    static XFFmpeg *Get()
    {
        static XFFmpeg ff;
        return &ff;
    }

    bool Open(const char *fileName);
    void Close();
    AVFrame *Decode(AVPacket *pkt);
    bool send_packet_data(AVPacket *pkt);
    AVFrame *receiveFrame();


    AVPacket Read();
    bool ToRGB(char *out, uint outwidth, uint outheight );
    int ToPCM(char *out,AVFrame *frame);

    int GetPts(AVFrame *pkt);
    bool seek(float pos);

    std::string GetError();


protected:
    XFFmpeg();

public:
    QMutex mutex;
    int mAudioStream;
    int mVideoStream;
    uint mTotalTime;
    char mErrorBuffer[1024];
    AVFormatContext *ic;
    AVFrame *mFrame;
    int fps;
    int pts;
    int apts;
    struct SwsContext *mSwsCtx;
    struct SwrContext *mSwrCtx;
};

#endif // XFFMPEG_H
