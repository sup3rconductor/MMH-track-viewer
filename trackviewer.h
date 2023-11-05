#ifndef TRACKVIEWER_H
#define TRACKVIEWER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class TrackViewer; }
QT_END_NAMESPACE

class TrackViewer : public QMainWindow
{
    Q_OBJECT

public:
    TrackViewer(QWidget *parent = nullptr);
    ~TrackViewer();

private:
    Ui::TrackViewer *ui;
};
#endif // TRACKVIEWER_H
