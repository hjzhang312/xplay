#include "audiowidget.h"
#include <QAudioFormat>
#include <QDebug>
#include <QIODevice>

AudioWidget::AudioWidget()
{
    out = NULL;
}

void AudioWidget::audioInit(int rate,int channels)
{
    QAudioFormat audioFormat;
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleRate(rate);
    audioFormat.setChannelCount(channels);
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    if(out)
    {
        delete out;
        out = NULL;
    }
    out = new QAudioOutput(audioFormat );
    in = out->start();
    in->open(QIODevice::ReadWrite);
    qDebug() << "buff size: " <<rate << channels;
}

bool AudioWidget::WriteData(char *data,int size)
{
    mutex.lock();
    int ret = in->write(data,size);
    bool v = true;

    if(ret != size)
    {
        v = false;
    }
    mutex.unlock();
    return v;
}

int AudioWidget::getBufFree()
{
    mutex.lock();
    int byte = out->bytesFree();
    mutex.unlock();
    return byte;

}

int AudioWidget::getBufSize()
{
    return out->bufferSize();

}

