#include "actions.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <iostream>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QTextStream>
#include <QMediaContent>

Actions::Actions()
{
    
}

Actions::~Actions()
{

}

QString Actions::getCommandOnVideo(Actions::enumActions action, QString name, QTime start, QTime end)
{
    QString path = QDir::currentPath()+"/../preview/";
    QString str;
    QString videoName(path+name);
    QString nameStart;
    QString nameEnd;
    QString nameMid;
    QString nameVideos;
    switch(action) {
        case Actions::enumActions::DELETE_ZONE:
            if (end.isNull()) {
                return "";
            }
            // Récupération du début
            nameStart += path+"start_"+name;
            str += "ffmpeg -i "+videoName;
            str += " -ss 00:00:00";
            str += " -to "+start.toString();
            str += " -c copy "+nameStart+" && ";
            // Récupération de la fin
            nameEnd += path+"end_"+name;
            str += "ffmpeg -i "+videoName;
            str += " -ss "+end.toString();
            str += " -c copy "+nameEnd+" && ";
            // Concaténation des deux parties
            nameVideos += nameStart+"|"+nameEnd;
            str += Actions::fusionVideos(videoName,nameVideos.split("|"));
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameStart+"|"+nameEnd;
            break;
        case Actions::enumActions::DELETE_BEGIN:
            str += "ffmpeg -i "+videoName;
            str += " -ss "+start.toString();
            str += " -c copy "+videoName;
            break;
        case Actions::enumActions::DELETE_END:
            str += "ffmpeg -i "+videoName;
            str += " -ss 00:00:00";
            str += " -to "+start.toString();
            str += " -c copy "+videoName;
            break;
        case Actions::enumActions::MUT:
            if (end.isNull()) {
                return "";
            }
            // Récupération du début
            nameStart += path+"start_"+name;
            str += "ffmpeg -i "+videoName;
            str += " -ss 00:00:00";
            str += " -to "+start.toString();
            str += " -c copy "+nameStart+" && ";
            // Récupération de la zone
            nameMid += path+"mid_"+name;
            str += "ffmpeg -i "+videoName;
            str += " -ss "+start.toString();
            str += " -to "+end.toString();
            str += " -c copy "+nameMid+" && ";
            // Récupération de la fin
            nameEnd += path+"end_"+name;
            str += "ffmpeg -i "+videoName;
            str += " -ss "+end.toString();
            str += " -c copy "+nameEnd+" && ";
            // Suppression du son sur la zone du milieu
            str += "ffmpeg -i "+nameMid;
            str += " -an -y ";
            str += " -c copy "+nameMid+" && ";
            // Concaténation des deux parties
            nameVideos += nameStart+"|"+nameEnd;
            str += Actions::fusionVideos(path+"Alice.mkv",nameVideos.split("|"));
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameStart+"|"+nameMid+"|"+nameEnd;
            break;
        case Actions::enumActions::SPLIT:
            // Récupération du la première partie
            str += "ffmpeg -i "+videoName;
            str += " -ss 00:00:00";
            str += " -to "+start.toString();
            str += " -c copy "+videoName+" && ";
            // Récupération de la deuxième partie
            nameEnd += path+"part_"+name;
            str += "ffmpeg -i "+videoName;
            str += " -ss "+start.toString();
            str += " -c copy "+nameEnd;
            break;
        default: 
            return "";
    }
    return str;
}

QString Actions::fusionVideos(QString finalName, QStringList nameOfVideos)
{
    QString liste = QDir::currentPath()+"/../preview/liste.txt";
    QString str;
    for (QString name : nameOfVideos) {
        str += "&& echo \"file '"+name+"'\" >> "+liste;
    }
    str += "&& ffmpeg -f concat -i "+liste+" -c copy "+finalName;
    return str;
}

bool Actions::removeFile(QStringList nameOfVideos)
{
    bool success = true;
    for (QString name : nameOfVideos) {        
        if (!QFile::remove(name)) {
            success = false;
        }
    }
    return success;
}

bool Actions::executeCommand(QString command)
{
    bool success = true;

    // Découpage de la commande si présence du mot DELETE
    QStringList nameVideosDelete;
    if (int index = command.indexOf("DELETE:")) {
        QStringList nameVideos = command.mid(index+6).split("|");
        command = command.left(index);
    }

    // Lancement des commandes
    QProcess cmd;
    QStringList allCommand = command.split("&&");
    for (auto command : allCommand) {
        if (int index = command.indexOf("<<")) {
            std::cout << command.mid(index+1).toStdString() << std::endl;
            cmd.setStandardOutputFile(command.mid(index+1));
            command = command.left(index);
        } else {
            cmd.setStandardOutputFile(QProcess::nullDevice());
        }
        int returnStat = cmd.execute(command);
        std::cout << command.toStdString() << " : " << returnStat << std::endl;
        if (returnStat != QProcess::NormalExit) {
            success = false;
        }
    }    
    // Suppression des fichiers si besoin
    if (!nameVideosDelete.isEmpty()) {
        removeFile(nameVideosDelete);
    }
    return success;    
}
