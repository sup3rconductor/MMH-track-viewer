#include "trackviewer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TrackViewer w;
    w.show();
    return a.exec();
}
