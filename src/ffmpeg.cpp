#include "ffmpeg.h"
#include <QFile>
#include <QTextStream>
#include <QMediaContent>

Ffmpeg::Ffmpeg()
{

}

QString Ffmpeg::getCommandOnVideo(Ffmpeg::Actions action, QString nameVideo, QTime start, QTime duration)
{
    QString str;
    str.append("ffmpeg -i "+nameVideo);
    switch(action) {
        case Ffmpeg::Actions::CUT:
            // Prise en compte de la zone sélectionnée
            str.append("-ss "+start.toString());
            str.append("-t "+duration.toString());

            str.append("-c:v copy -c:a copy");
            break;
    }
    str.append("cut_"+nameVideo);
    return str;
}

void Ffmpeg::executeCommand(QString command)
{
    start(command);
}

void Ffmpeg::fusionVideos(QVector<Media> videos)
{
    // Création fichier contenant toutes les vidéos
    QFile file("fic.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&file);
    for (Media video : videos) {
        flux << video.content.canonicalUrl().toString();
    }
    file.close();
    // Commande concaténant les vidéos du fichier
    QString str("ffmpeg -f concat -i fic.txt -c copy projet.ogg");
    executeCommand(str);
}

Ffmpeg::~Ffmpeg()
{
    terminate();
}

