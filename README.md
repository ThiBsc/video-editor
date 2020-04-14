# video-editor
Projet annuel d'édition de video linéaire.

## Fonctionnalités:

- Ajout de fichiers
- Récupération de la vidéo finale
- Sauvegarde du projet (des vidéos dans leur état courant)
- Renommer une vidéo
- Supprimer des vidéos
- Fusionner des vidéos
- Supprimer des parties de la vidéo sélectionnée
- Mettre sous silence une zone de la vidéo sélectionnée
- Réduire le bruit ambiant grâce à une zone témoin (application globale ou locale)
- Scinder en deux une vidéo
- Ajout de marqueurs manuellement

## Requirements

- qt5-default
- qtmultimedia5-dev
- libqt5multimedia5
- libqt5multimedia5-plugins
- libavformat-dev
- ffmpeg

### Used libraries
[![QCustomPlot](http://www.qcustomplot.com/qcp-logo.png)](http://www.qcustomplot.com/)

### Optional

- SoX

## Compile

```shell
git clone https://github.com/thibsc/video-editor.git
cd video-editor
mkdir build
cd build
qmake ../video-editor.pro
make && make clean
```
