#include "actions.h"
#include "mainwindow.h"
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

Actions::Actions(){}

Actions::~Actions(){}

QString Actions::getCommandOnVideo(Actions::enumActions action, QString name, QTime start, QTime end)
{
    if (start.isNull()) {
        return "";
    }

    QString path = MainWindow::settings->value("General/dir_preview").toString()+"/";
    QString videoName(path+name);
    QString str(""), nameStart, nameEnd, nameMid, nameTmp, nameVideos;
    switch(action) {
        case Actions::enumActions::DELETE_ZONE:
            if (!end.isNull()) {
                // Récupération du début
                nameStart += path+"start_"+name;
                str += "ffmpeg -y -i "+videoName;
                str += " -t "+start.toString("hh:mm:ss.zz");
                str += " -c copy "+nameStart+" && ";
                // Récupération de la fin
                nameEnd += path+"end_"+name;
                str += "ffmpeg -y -i "+videoName;
                str += " -ss "+end.toString("hh:mm:ss.zz");
                str += " -c copy "+nameEnd+" && ";
                // Concaténation des deux parties
                nameVideos += nameStart+"|"+nameEnd;
                str += Actions::fusionVideos(videoName,nameVideos.split("|"));
                // Suppression des vidéos temporaires
                str += "DELETE:"+nameStart+"|"+nameEnd;
            }
            break;
        case Actions::enumActions::DELETE_BEGIN:
            nameTmp += path+"tmp_"+name;
            str += "ffmpeg -y -i "+videoName;
            str += " -ss "+start.toString("hh:mm:ss.zz");
            str += " -c copy "+nameTmp+" && ";
            // Renommage
            str += "ffmpeg -y -i "+nameTmp;
            str += " -c copy "+videoName+" && ";
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameTmp;
            break;
        case Actions::enumActions::DELETE_END:
            nameTmp += path+"tmp_"+name;
            str += "ffmpeg -y -i "+videoName;
            str += " -t "+start.toString("hh:mm:ss.zz");
            str += " -c copy "+nameTmp+" && ";
            // Renommage
            str += "ffmpeg -y -i "+nameTmp;
            str += " -c copy "+videoName+" && ";
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameTmp;
            break;
        case Actions::enumActions::TRIM:
            nameTmp += path+"tmp_"+name;
            str += "ffmpeg -y -i "+videoName;
            str += " -ss "+start.toString("hh:mm:ss.zz");
            str += " -to "+end.toString("hh:mm:ss.zz");
            str += " -c copy "+nameTmp+" && ";
            str += "ffmpeg -y -i "+nameTmp;
            str += " -c copy "+videoName+" && ";
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameTmp;
            break;
        case Actions::enumActions::MUT:
            if (!end.isNull()) {
                // Récupération du début
                nameStart += path+"start_"+name;
                str += "ffmpeg -y -i "+videoName;
                str += " -t "+start.toString("hh:mm:ss.zz");
                str += " -c copy "+nameStart+" && ";
                // Récupération de la zone sans son
                nameMid += path+"mid_"+name;
                str += "ffmpeg -y -i "+videoName;
                str += " -ss "+start.toString("hh:mm:ss.zz");
                str += " -to "+end.toString("hh:mm:ss.zz");
                str += " -c copy "+nameMid+" && ";
                nameTmp += path+"tmp_"+name;
                str += "ffmpeg -y -i "+nameMid;
                str += " -filter:a \"volume=0\" -c:a aac -strict -2 "+nameTmp+" && ";
                str += "ffmpeg -y -i "+nameTmp;
                str += " -c copy "+nameMid+" && ";
                // Récupération de la fin
                nameEnd += path+"end_"+name;
                str += "ffmpeg -y -i "+videoName;
                str += " -ss "+end.toString("hh:mm:ss.zz");
                str += " -c copy "+nameEnd+" && ";
                // Concaténation des parties
                nameVideos += nameStart+"|"+nameMid+"|"+nameEnd;
                str += Actions::fusionVideos(videoName,nameVideos.split("|"));
                // Suppression des vidéos temporaires
                str += "DELETE:"+nameStart+"|"+nameMid+"|"+nameEnd+"|"+nameTmp;
            }
            break;
        case Actions::enumActions::SPLIT:
            // Récupération du la première partie
            nameTmp += path+"tmp_"+name;
            str += "ffmpeg -y -i "+videoName;
            str += " -t "+start.toString("hh:mm:ss.zz");
            str += " -c copy "+nameTmp+" && ";
            // Récupération de la deuxième partie
            nameEnd += path+"part_"+name;
            str += "ffmpeg -y -i "+videoName;
            str += " -ss "+start.toString("hh:mm:ss.zz");
            str += " -c copy "+nameEnd+" && ";
            // Renommage
            str += "ffmpeg -y -i "+nameTmp;
            str += " -c copy "+videoName+" && ";
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameTmp;
            break;
        default:
            str = "";
    }
    return str;
}

QString Actions::fusionVideos(QString finalName, QStringList nameOfVideos)
{
    QFileInfo infoOutput(MainWindow::settings->value("General/dir_preview").toString()+"/liste.txt");
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

/**
 *  Delete a directory along with all of its contents.
 *
 *  param dirName Path of directory to remove.
 *  return true on success; false on error.
 */
bool Actions::removeAllFileDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);
    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            result = QFile::remove(info.absoluteFilePath());
            if (!result) {
                return result;
            }
        }
    } 
    return result;
}

/**
 * @brief Actions::copyFile
 * Copy the media in dest directory
 */
bool Actions::copyFile(QString src, QString dest)
{
    QDir dir(dest);
    if(!dir.exists()){
        return false;
    }
    QUrl origin(src);
    QString fileDest(dest+"/"+origin.fileName());
    if (QFile::exists(fileDest)) {
        QFile::remove(fileDest);
    }
    return QFile::copy(src, fileDest);
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
            if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                nameVideosDelete.append(nameFile);
                QTextStream out(&file);
                out << command.left(index).toStdString().c_str() << "\n";
            }
        } else {
            returnStat = cmd.execute(command);
            if (returnStat == -1) {
                std::cout << "RETOUR STAT " << returnStat << std::endl;
                success = false;
            }
        }
    }
    // Suppression des fichiers
    Actions::removeFile(nameVideosDelete);
    return success;
}
