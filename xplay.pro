#-------------------------------------------------
#
# Project created by QtCreator 2017-05-18T09:03:18
#
#-------------------------------------------------

QT       += core gui opengl multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = xplay
TEMPLATE = app


SOURCES += main.cpp\
        xplay.cpp \
    xffmpeg.cpp \
    videowidget.cpp \
    xreadthread.cpp \
    videoplay.cpp \
    audioplay.cpp \
    audiowidget.cpp \
    myslider.cpp

HEADERS  += xplay.h \
    include/libavformat/avformat.h \
    include/libavformat/avio.h \
    include/libavformat/version.h \
    include/libavutil/adler32.h \
    include/libavutil/aes.h \
    include/libavutil/aes_ctr.h \
    include/libavutil/attributes.h \
    include/libavutil/audio_fifo.h \
    include/libavutil/avassert.h \
    include/libavutil/avconfig.h \
    include/libavutil/avstring.h \
    include/libavutil/avutil.h \
    include/libavutil/base64.h \
    include/libavutil/blowfish.h \
    include/libavutil/bprint.h \
    include/libavutil/bswap.h \
    include/libavutil/buffer.h \
    include/libavutil/camellia.h \
    include/libavutil/cast5.h \
    include/libavutil/channel_layout.h \
    include/libavutil/common.h \
    include/libavutil/cpu.h \
    include/libavutil/crc.h \
    include/libavutil/des.h \
    include/libavutil/dict.h \
    include/libavutil/display.h \
    include/libavutil/downmix_info.h \
    include/libavutil/error.h \
    include/libavutil/eval.h \
    include/libavutil/ffversion.h \
    include/libavutil/fifo.h \
    include/libavutil/file.h \
    include/libavutil/frame.h \
    include/libavutil/hash.h \
    include/libavutil/hmac.h \
    include/libavutil/hwcontext.h \
    include/libavutil/hwcontext_cuda.h \
    include/libavutil/hwcontext_dxva2.h \
    include/libavutil/hwcontext_qsv.h \
    include/libavutil/hwcontext_vaapi.h \
    include/libavutil/hwcontext_vdpau.h \
    include/libavutil/imgutils.h \
    include/libavutil/intfloat.h \
    include/libavutil/intreadwrite.h \
    include/libavutil/lfg.h \
    include/libavutil/log.h \
    include/libavutil/lzo.h \
    include/libavutil/macros.h \
    include/libavutil/mastering_display_metadata.h \
    include/libavutil/mathematics.h \
    include/libavutil/md5.h \
    include/libavutil/mem.h \
    include/libavutil/motion_vector.h \
    include/libavutil/murmur3.h \
    include/libavutil/opt.h \
    include/libavutil/parseutils.h \
    include/libavutil/pixdesc.h \
    include/libavutil/pixelutils.h \
    include/libavutil/pixfmt.h \
    include/libavutil/random_seed.h \
    include/libavutil/rational.h \
    include/libavutil/rc4.h \
    include/libavutil/replaygain.h \
    include/libavutil/ripemd.h \
    include/libavutil/samplefmt.h \
    include/libavutil/sha.h \
    include/libavutil/sha512.h \
    include/libavutil/spherical.h \
    include/libavutil/stereo3d.h \
    include/libavutil/tea.h \
    include/libavutil/threadmessage.h \
    include/libavutil/time.h \
    include/libavutil/timecode.h \
    include/libavutil/timestamp.h \
    include/libavutil/tree.h \
    include/libavutil/twofish.h \
    include/libavutil/version.h \
    include/libavutil/xtea.h \
    include/libavcodec/avcodec.h \
    include/libavcodec/avdct.h \
    include/libavcodec/avfft.h \
    include/libavcodec/d3d11va.h \
    include/libavcodec/dirac.h \
    include/libavcodec/dv_profile.h \
    include/libavcodec/dxva2.h \
    include/libavcodec/jni.h \
    include/libavcodec/mediacodec.h \
    include/libavcodec/qsv.h \
    include/libavcodec/vaapi.h \
    include/libavcodec/vda.h \
    include/libavcodec/vdpau.h \
    include/libavcodec/version.h \
    include/libavcodec/videotoolbox.h \
    include/libavcodec/vorbis_parser.h \
    include/libavcodec/xvmc.h \
    include/libswresample/swresample.h \
    include/libswresample/version.h \
    include/libswscale/swscale.h \
    include/libswscale/version.h \
    xffmpeg.h \
    videowidget.h \
    xreadthread.h \
    videoplay.h \
    audioplay.h \
    audiowidget.h \
    include/libavfilter/avfilter.h \
    myslider.h

FORMS    += xplay.ui

INCLUDEPATH += $$PWD/ \
               $$PWD/lib \
               $$PWD/include/\
               $$PWD/include/libavformat\
               $$PWD/include/libavutil \
               $$PWD/include/libavcodec \
               $$PWD/include/libswresample \
               $$PWD/include/libswscale \
               $$PWD/include/libavfilter

DEPENDPATH += $$PWD/ \
              $$PWD/include\
              $$PWD/lib



LIBS += -L/$$PWD/lib/ -lavformat -lavutil -lavcodec -lswscale -lswresample
