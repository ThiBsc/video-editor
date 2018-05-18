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
