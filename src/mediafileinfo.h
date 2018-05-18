#ifndef MEDIAFILEINFO_H
#define MEDIAFILEINFO_H

#include <string>
#include <QObject>

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
    qint64 getMS();
    int getUSecond();
    int getSecond();
    int getMinute();
    int getHour();

private:
    std::string filename;
    AVFormatContext *pFormatCtx;
    qint64 duration;

};

#endif // MEDIAFILEINFO_H
