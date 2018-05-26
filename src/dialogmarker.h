#ifndef DIALOGMARKER_H
#define DIALOGMARKER_H

#include <QDialog>

class QVBoxLayout;
class QPlainTextEdit;
class QDialogButtonBox;

class DialogMarker : public QDialog
{
public:
    DialogMarker(QSet<qint64> markers, QWidget *parent = 0);
    ~DialogMarker();
    QSet<qint64> getMarkers();

private:
    QVBoxLayout *vLayout;

    QDialogButtonBox *btnBox;
    QPlainTextEdit *timeEdit;
};

#endif // DIALOGMARKER_H
