#include "settingeditor.h"
#include "mainwindow.h"

#include <QDialogButtonBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>

SettingEditor::SettingEditor(QWidget *parent)
    : QDialog(parent)
{
    vLayout = new QVBoxLayout(this);

    QStringList keys = MainWindow::settings->allKeys();
    twSettings = new QTableWidget(keys.size(), 2, this);
    twSettings->setHorizontalHeaderLabels({tr("Key"), tr("Value")});
    twSettings->horizontalHeader()->setStretchLastSection(true);
    int idx = 0;
    for (const QString key : keys){
        QTableWidgetItem *item_key = twSettings->item(idx, KEY);
        if (!item_key){
            item_key = new QTableWidgetItem(key);
            twSettings->setItem(idx, KEY, item_key);
        } else {
            item_key->setText(key);
        }
        item_key->setFlags(item_key->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *item_value = twSettings->item(idx, VALUE);
        if (!item_value){
            item_value = new QTableWidgetItem(MainWindow::settings->value(key).toString());
            twSettings->setItem(idx, VALUE, item_value);
        } else {
            item_value->setText(MainWindow::settings->value(key).toString());
        }
        idx++;
    }

    btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

    vLayout->addWidget(twSettings);
    vLayout->addWidget(btnBox);
    setLayout(vLayout);

    connect(btnBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(btnBox, SIGNAL(rejected()), this, SLOT(reject()));
}

SettingEditor::~SettingEditor()
{
    delete twSettings;
    delete btnBox;
    delete vLayout;
}

void SettingEditor::saveSettings()
{
    for (int i=0; i<twSettings->rowCount(); i++){
        const QString key = twSettings->item(i, KEY)->text();
        const QString value = twSettings->item(i, VALUE)->text();
        if (key.contains("ffmpeg"))
            Actions::ffmpeg = value;
        if (key.contains("sox"))
            Actions::sox = value;
        MainWindow::settings->setValue(key, value);
    }
    MainWindow::settings->sync();
}
