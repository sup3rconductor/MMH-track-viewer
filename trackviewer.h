#ifndef TRACKVIEWER_H
#define TRACKVIEWER_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>

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
    QGraphicsScene *XZ_scene;
    QGraphicsScene *YZ_scene;

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void draw_projection(QGraphicsScene* sc, int A, int B, int C, int D, bool flag);
    void correspondence_table(int x0, int y0, int start_num);

private slots:
    void event_switch();
    void draw_dots();
    void draw_reconstructed_track();

};
#endif // TRACKVIEWER_H
