# video-editor
Projet annuel d'édition de video linéaire

## Requirements

- qt5-default
- qtmultimedia5-dev
- libqt5multimedia5
- libqt5multimedia5-plugins
- libavformat-dev

### Used libraries
[![QCustomPlot](http://www.qcustomplot.com/qcp-logo.png)](http://www.qcustomplot.com/)

## Compile

```shell
git clone https://github.com/thibDev/video-editor.git
cd video-editor
mkdir build
cd build
qmake ../video-editor.pro
make && make clean
```
