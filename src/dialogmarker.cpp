#include "dialogmarker.h"

#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTime>

DialogMarker::DialogMarker(QSet<qint64> markers, QWidget *parent)
    : QDialog(parent)
{
    vLayout = new QVBoxLayout(this);

    timeEdit = new QPlainTextEdit(this);
    for (const qint64 marker : markers){
        timeEdit->appendPlainText(QTime(0, 0, 0).addMSecs(marker).toString("hh:mm:ss"));
    }

    btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

    vLayout->addWidget(timeEdit);
    vLayout->addWidget(btnBox);
    setLayout(vLayout);

    connect(btnBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(btnBox, SIGNAL(rejected()), this, SLOT(reject()));
}

DialogMarker::~DialogMarker()
{
    delete timeEdit;
    delete btnBox;
    delete vLayout;
}

QSet<qint64> DialogMarker::getMarkers()
{
    QSet<qint64> ret;
    const QString markerText = timeEdit->toPlainText();
    QStringList markers = markerText.split("\n");
    for (const QString marker : markers){
        QTime m = QTime::fromString(marker, "hh:mm:ss");
        if (m.isValid()){
            ret.insert(QTime(0, 0, 0).msecsTo(m));
        }
    }
    return ret;
}
