#ifndef SETTINGEDITOR_H
#define SETTINGEDITOR_H

#include <QDialog>

class QVBoxLayout;
class QTableWidget;
class QDialogButtonBox;

class SettingEditor : public QDialog
{
public:
    SettingEditor(QWidget *parent = 0);
    ~SettingEditor();
    void saveSettings();

private:
    enum Col { KEY=0, VALUE };
    QVBoxLayout *vLayout;

    QDialogButtonBox *btnBox;
    QTableWidget *twSettings;

};

#endif // SETTINGEDITOR_H
