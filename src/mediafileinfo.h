#ifndef MEDIAFILEINFO_H
#define MEDIAFILEINFO_H

extern "C" {
#include <libavformat/avformat.h>
}

/**
 * @brief The MediaFileInfo class
 * Class to get the meta data of a media file
 * Actually only use to get media file duration
 */
class MediaFileInfo
{
public:
    MediaFileInfo();
    ~MediaFileInfo();
    void find_meta_data(const char* filename);
    int getMS();
    int getUSecond();
    int getSecond();
    int getMinute();
    int getHour();

private:
    AVFormatContext *pFormatCtx;
    int64_t duration;

};

#endif // MEDIAFILEINFO_H
