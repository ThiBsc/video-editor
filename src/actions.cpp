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

QString Actions::ffmpeg = "ffmpeg";
QString Actions::sox = "";

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
                str += Actions::ffmpeg+" -y -i "+videoName;
                str += " -t "+start.toString("hh:mm:ss.zz");
                str += " -c copy "+nameStart+" && ";
                // Récupération de la fin
                nameEnd += path+"end_"+name;
                str += Actions::ffmpeg+" -y -i "+videoName;
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
            str += Actions::ffmpeg+" -y -i "+videoName;
            str += " -ss "+start.toString("hh:mm:ss.zz");
            str += " -c copy "+nameTmp+" && ";
            // Renommage
            str += Actions::ffmpeg+" -y -i "+nameTmp;
            str += " -c copy "+videoName+" && ";
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameTmp;
            break;
        case Actions::enumActions::DELETE_END:
            if (!end.isNull()) {
                nameTmp += path+"tmp_"+name;
                str += Actions::ffmpeg+" -y -i "+videoName;
                str += " -t "+end.toString("hh:mm:ss.zz");
                str += " -c copy "+nameTmp+" && ";
                // Renommage
                str += Actions::ffmpeg+" -y -i "+nameTmp;
                str += " -c copy "+videoName+" && ";
                // Suppression des vidéos temporaires
                str += "DELETE:"+nameTmp;
            }
            break;
        case Actions::enumActions::TRIM:
            nameTmp += path+"tmp_"+name;
            str += Actions::ffmpeg+" -y -i "+videoName;
            str += " -ss "+start.toString("hh:mm:ss.zz");
            str += " -to "+end.toString("hh:mm:ss.zz");
            str += " -c copy "+nameTmp+" && ";
            str += Actions::ffmpeg+" -y -i "+nameTmp;
            str += " -c copy "+videoName+" && ";
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameTmp;
            break;
        case Actions::enumActions::MUT:
            if (!end.isNull()) {
                // Récupération du début
                nameStart += path+"start_"+name;
                str += Actions::ffmpeg+" -y -i "+videoName;
                str += " -t "+start.toString("hh:mm:ss.zz");
                str += " -c copy "+nameStart+" && ";
                // Récupération de la zone sans son
                nameMid += path+"mid_"+name;
                str += Actions::ffmpeg+" -y -i "+videoName;
                str += " -ss "+start.toString("hh:mm:ss.zz");
                str += " -to "+end.toString("hh:mm:ss.zz");
                str += " -c copy "+nameMid+" && ";
                nameTmp += path+"tmp_"+name;
                str += Actions::ffmpeg+" -y -i "+nameMid;
                str += " -filter:a \"volume=0\" -c:a aac -strict -2 "+nameTmp+" && ";
                str += Actions::ffmpeg+" -y -i "+nameTmp;
                str += " -c copy "+nameMid+" && ";
                // Récupération de la fin
                nameEnd += path+"end_"+name;
                str += Actions::ffmpeg+" -y -i "+videoName;
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
            str += Actions::ffmpeg+" -y -i "+videoName;
            str += " -t "+start.toString("hh:mm:ss.zz");
            str += " -c copy "+nameTmp+" && ";
            // Récupération de la deuxième partie
            nameEnd += path+"part_"+name;
            str += Actions::ffmpeg+" -y -i "+videoName;
            str += " -ss "+start.toString("hh:mm:ss.zz");
            str += " -c copy "+nameEnd+" && ";
            // Renommage
            str += Actions::ffmpeg+" -y -i "+nameTmp;
            str += " -c copy "+videoName+" && ";
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameTmp;
            break;
        case Actions::enumActions::NOISE:
            str += Actions::createProfileNoise(name, start, end);
            str += Actions::getCommandApplyFilterNoise(name);
            break;
        default:
            str = "";
            break;
    }
    return str;
}

QString Actions::getCommandApplyFilterNoise(QString videoName)
{
    QString path = MainWindow::settings->value("General/dir_preview").toString()+"/";
    QString profileNoise = MainWindow::settings->value("General/dir_preview").toString()+"/noise.prof";
    QString audioCleanFile(path+"best_audio.wav");
    QString audiFile(path+"audio.wav");
    QString videoTmp(path+"tmp_"+videoName);
    QString str;
    // Séparation de l'audio de la vidéo
    str += Actions::ffmpeg+" -y -i "+path+videoName+" -c copy -an "+videoTmp+" && ";
    str += Actions::ffmpeg+" -y -i "+path+videoName+" -vn "+audiFile+" && ";
    // Création de l'audio sans bruit ambiant avec SoXs
    str += Actions::sox+" "+audiFile+" "+audioCleanFile+" noisered "+profileNoise+" "+MainWindow::settings->value("General/sensibility").toString()+" && ";
    // Assemblage de l'audio avec la vidéo sans son
    str += Actions::ffmpeg+" -y -i "+audioCleanFile+" -i "+videoTmp+" -c copy "+path+videoName+" && ";
    // Suppression des vidéos temporaires
    str += "DELETE:"+videoTmp+"|"+audiFile+"|"+audioCleanFile+" && ";
    return str;
}

QString Actions::createProfileNoise(QString videoName, QTime start, QTime end)
{
    QString str;
    if (!Actions::sox.isEmpty() && !Actions::ffmpeg.isEmpty() && !start.isNull() && !end.isNull()) {
        QString path = MainWindow::settings->value("General/dir_preview").toString()+"/";
        QString profileNoise = MainWindow::settings->value("General/dir_preview").toString()+"/noise.prof";
        QString noiseAudio(path+"noise_audio.wav");
        str += Actions::ffmpeg+" -y -i "+path+videoName+" -vn ";
        str += " -ss "+start.toString("hh:mm:ss.zz");
        str += " -to "+end.toString("hh:mm:ss.zz");
        str += " "+path+"noise_audio.wav && ";
        // SoX profile
        str += Actions::sox+" "+noiseAudio+" -n noiseprof "+profileNoise+" && ";
        str += "DELETE:"+noiseAudio+" && ";
    } else {
        QString msg;
        if (start.isNull() || end.isNull()) {
            msg = tr("Invalid area");
        } else if (Actions::sox.isEmpty()) {
            msg = tr("You must configure SoX in setting");
        } else if () {
            msg = tr("You must configure ffmpeg in setting");
        }
        // Erreur pas de zone de bruit sélectionnée ou pas sox de paramétré
        QMessageBox::critical(NULL, tr("Error"), msg, QMessageBox::OK);
    }
    return str;
}

QString Actions::fusionVideos(QString finalName, QStringList nameOfVideos)
{
    QFileInfo infoOutput(MainWindow::settings->value("General/dir_preview").toString()+"/liste.txt");
    QString str;
    for (QString name : nameOfVideos) {
        QFileInfo info(name);
        str += " && file "+info.absoluteFilePath()+" >> "+infoOutput.absoluteFilePath();
    }
    str += " && "+Actions::ffmpeg+" -y -f concat -safe 0 -i "+infoOutput.absoluteFilePath()+" -c copy "+finalName+" && ";
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
    QUrl origin(src);
    QString fileDest;
    if(dir.exists()){        
        fileDest = dest+"/"+origin.fileName();
    } else {
        fileDest = dest;
    }
    if (QFile::exists(fileDest)) {
        QFile::remove(fileDest);
    }
    return QFile::copy(src, fileDest);
}

bool Actions::renameFile(QString src, QString newname)
{
    return QFile::rename(src, newname);
}

bool Actions::executeCommand(QString command)
{
    bool success = true;
    QProcess cmd;
    int returnStat;
    int index;
    QStringList nameVideosDelete;
    QString nameFile;
    QStringList allCommand = command.split("&&");
    for (auto command : allCommand) {
        if (!command.trimmed().isEmpty()) {
            index = command.indexOf(">>");
            if (index != -1) {
                nameFile = command.mid(index+3);
                QFile file(nameFile.trimmed());
                if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                    nameVideosDelete.append(nameFile);
                    QTextStream out(&file);
                    out << command.left(index) << "\n";
                    file.close();
                }
            } else {
                index = command.indexOf("DELETE:");
                if (index != -1) {
                    nameVideosDelete.append(command.mid(index+7).split("|"));
                } else {
                    returnStat = cmd.execute(command);
                    if (returnStat == -1) {

                        success = false;
                    }
                }
                
            }
        }
    }
    // Suppression des fichiers
    Actions::removeFile(nameVideosDelete);
    return success;
}
