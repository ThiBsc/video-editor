#include "mediafileinfo.h"

MediaFileInfo::MediaFileInfo()
{
    // init
    av_register_all();
    pFormatCtx = avformat_alloc_context();
}

MediaFileInfo::~MediaFileInfo()
{
    // clear
    avformat_free_context(pFormatCtx);
}

/**
 * @brief MediaFileInfo::find_meta_data
 * @param filename
 * Find the metadata of the filename
 */
void MediaFileInfo::find_meta_data(const char *filename)
{
    this->filename = filename;
    avformat_open_input(&pFormatCtx, filename, NULL, NULL);
    if (pFormatCtx->duration == 0)
        avformat_find_stream_info(pFormatCtx, NULL);
    duration = pFormatCtx->duration;
    // Find the index of the first audio stream
    int audio_index = -1, video_index = -1;
    for (uint i=0; i<pFormatCtx->nb_streams; i++) {
        if (audio_index == -1 && pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audio_index = i;
        }
        if (video_index == -1 && pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            video_index = i;
        }
        if (audio_index != -1 && video_index != -1)
            break;
    }
    if (audio_index != -1){
        AVStream* stream = pFormatCtx->streams[audio_index];
        AVCodec *a_codec = avcodec_find_decoder(stream->codec->codec_id);
        audio_codec = a_codec->name;
    }
    if (video_index != -1){
        AVStream* stream = pFormatCtx->streams[video_index];
        AVCodec *v_codec = avcodec_find_decoder(stream->codec->codec_id);
        video_codec = v_codec->name;
    }
    avformat_close_input(&pFormatCtx);
}

/*
 int hours, mins, secs, us;
 secs = ic->duration / AV_TIME_BASE;
 us = ic->duration % AV_TIME_BASE;
 mins = secs / 60;
 secs %= 60;
 hours = mins / 60;
 mins %= 60;
*/

/**
 * @brief MediaFileInfo::getMS
 * @return The total time in millisecond of the media file
 */
qint64 MediaFileInfo::getMS()
{
    return duration;
}

/**
 * @brief MediaFileInfo::getUSecond
 * @return The microsecond of the media file
 */
int MediaFileInfo::getUSecond()
{
    return duration % 60;
}

/**
 * @brief MediaFileInfo::getSecond
 * @return The second of the media file
 */
int MediaFileInfo::getSecond()
{
    return (duration / AV_TIME_BASE) % 60;
}

/**
 * @brief MediaFileInfo::getMinute
 * @return The minute of the media file
 */
int MediaFileInfo::getMinute()
{
    return ((duration / AV_TIME_BASE) / 60) % 60;
}

/**
 * @brief MediaFileInfo::getHour
 * @return The hour of the media file
 */
int MediaFileInfo::getHour()
{
    return ((duration / AV_TIME_BASE) / 60) / 60;
}

std::string MediaFileInfo::getAudioCodec()
{
    return audio_codec;
}

std::string MediaFileInfo::getVideoCodec()
{
    return video_codec;
}
