#include "trackviewer.h"
#include "ui_trackviewer.h"

TrackViewer::TrackViewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TrackViewer)
{
    ui->setupUi(this);
}

TrackViewer::~TrackViewer()
{
    delete ui;
}

