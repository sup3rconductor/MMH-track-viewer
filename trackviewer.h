#ifndef TRACKVIEWER_H
#define TRACKVIEWER_H

#include <QMainWindow>
#include <QKeyEvent>

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

protected:
    void keyPressEvent(QKeyEvent *e) override;

private slots:
    void event_switch();
    void draw_dots();
    void draw_reconstructed_track();

};
#endif // TRACKVIEWER_H
