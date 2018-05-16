#include "actions.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QTextStream>
#include <QMediaContent>
#include <iostream>
#include <fstream>

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
            str += "ffmpeg -y -i "+videoName;
            str += " -ss 00:00:00.00";
            str += " -to "+start.toString();
            str += " -c copy "+nameStart+" && ";
            // Récupération de la fin
            nameEnd += path+"end_"+name;
            str += "ffmpeg -y -i "+videoName;
            str += " -ss "+end.toString();
            str += " -c copy "+nameEnd+" && ";
            // Concaténation des deux parties
            nameVideos += nameStart+"|"+nameEnd;
            str += Actions::fusionVideos(videoName,nameVideos.split("|"));
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameStart+"|"+nameEnd;
            break;
        case Actions::enumActions::DELETE_BEGIN:
            str += "ffmpeg -y -i "+videoName;
            str += " -ss "+start.toString();
            str += " -c copy "+videoName;
            break;
        case Actions::enumActions::DELETE_END:
            str += "ffmpeg -y -i "+videoName;
            str += " -ss 00:00:00.00";
            str += " -to "+start.toString();
            str += " -c copy "+videoName;
            break;
        case Actions::enumActions::MUT:
            if (end.isNull()) {
                return "";
            }
            // Récupération du début
            nameStart += path+"start_"+name;
            str += "ffmpeg -y -i "+videoName;
            str += " -ss 00:00:00.00";
            str += " -to "+start.toString();
            str += " -c copy "+nameStart+" && ";
            // Récupération de la zone
            nameMid += path+"mid_"+name;
            str += "ffmpeg -y -i "+videoName;
            str += " -ss "+start.toString();
            str += " -to "+end.toString();
            str += " -c copy "+nameMid+" && ";
            // Récupération de la fin
            nameEnd += path+"end_"+name;
            str += "ffmpeg -y -i "+videoName;
            str += " -ss "+end.toString();
            str += " -c copy "+nameEnd+" && ";
            // Suppression du son sur la zone du milieu
            str += "ffmpeg -y -i "+nameMid;
            str += " -an -y ";
            str += " -c copy "+nameMid+" && ";
            // Concaténation des parties
            nameVideos += nameStart+"|"+nameMid+"|"+nameEnd;
            str += Actions::fusionVideos(videoName,nameVideos.split("|"));
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameStart+"|"+nameMid+"|"+nameEnd;
            break;
        case Actions::enumActions::SPLIT:
            // Récupération du la première partie
            str += "ffmpeg -y -i "+videoName;
            str += " -ss 00:00:00.00";
            str += " -to "+start.toString();
            str += " -c copy "+videoName+" && ";
            // Récupération de la deuxième partie
            nameEnd += path+"part_"+name;
            str += "ffmpeg -y -i "+videoName;
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
    QFileInfo infoOutput(QDir::currentPath()+"/../preview/liste.txt");
    QString str;
    for (QString name : nameOfVideos) {
        QFileInfo info(name);
        str += "&& file "+info.absoluteFilePath()+" >> "+infoOutput.absoluteFilePath();
    }
    str += "&& ffmpeg -y -f concat -safe 0 -i "+infoOutput.absoluteFilePath()+" -c copy "+finalName;
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
    int index = command.indexOf("DELETE:");
    if (index != -1) {
        nameVideosDelete = command.mid(index+7).split("|");
        command = command.left(index);
    }

    // Lancement des commandes
    QProcess cmd;
    int returnStat;
    QString nameFile;
    QStringList allCommand = command.split("&&");
    for (auto command : allCommand) {
        index = command.indexOf(">>");
        if (index != -1) {
            nameFile = command.mid(index+3);
            QFile file(nameFile.toStdString().c_str());
            if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                std::cout << "Erreur" << std::endl;
            } else {
                nameVideosDelete.append(nameFile);
                QTextStream out(&file);
                out << command.left(index).toStdString().c_str() << "\n";
            }
        } else {
            returnStat = cmd.execute(command);
            if (returnStat != QProcess::NormalExit) {
                success = false;
            }
        }
    }    
    // Suppression des fichiers
    removeFile(nameVideosDelete);
    return success;    
}
