#include "trackviewer.h"
#include "ui_trackviewer.h"
#include <qmath.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <QVector>

TrackViewer::TrackViewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TrackViewer)
{
    //Connecting buttons
    ui->setupUi(this);
    connect(ui->Prev_ev, SIGNAL(clicked()), this, SLOT(event_switch()));
    connect(ui->Next_ev, SIGNAL(clicked()), this, SLOT(event_switch()));
    connect(ui->show_pts, SIGNAL(clicked()), this, SLOT(draw_dots()));
    connect(ui->show_reconst, SIGNAL(clicked()), this, SLOT(draw_reconstructed_track()));

    ui->Prev_ev->setCheckable(true);
    ui->Next_ev->setCheckable(true);
    ui->show_pts->setCheckable(true);
    ui->show_reconst->setCheckable(true);

    //Setting plots
    ui->graph_1->xAxis->setRange(-500, 500);
    ui->graph_1->yAxis->setRange(-500, 500);
    ui->graph_2->xAxis->setRange(-500, 500);
    ui->graph_2->yAxis->setRange(-500, 500);

    //Убирает оси (дезигнерское решение)
    ui->graph_1->xAxis->setVisible(false);
    ui->graph_1->yAxis->setVisible(false);
    ui->graph_2->xAxis->setVisible(false);
    ui->graph_2->yAxis->setVisible(false);

    //Adding scenes
    XZ_scene = new QGraphicsScene();
    YZ_scene = new QGraphicsScene();

    //Setting scenes
    ui->XZ_proj->setScene(XZ_scene);
    ui->YZ_proj->setScene(YZ_scene);
    ui->XZ_proj->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->XZ_proj->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->YZ_proj->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->YZ_proj->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //Limiting scene size
    XZ_scene->setSceneRect(-540, -238, 1080, 476);
    YZ_scene->setSceneRect(-540, -238, 1080, 476);

    //Drawing MMH
    draw_projection(XZ_scene, -540, -530, 209, 216, true);
    draw_projection(YZ_scene, -540, -530, 224, 231, false);
    correspondence_table(-540, 209, 0);
    correspondence_table(-540, 224, 192);
}

TrackViewer::~TrackViewer()
{
    delete ui;
}


//Global variables: event number, coordinates of muons and charged particles, track parameters ()
int EventNum = -1;
QVector<double> coord_mu[3];
QVector<double> coord_charged[3];
QVector<double> coordinates[3];
QVector<double> XZ_track_parameters;
QVector<double> YZ_track_parameters;
int Xvertices[1152];
int Yvertices[1152];

//Function to draw projections of MMH
//sc - grapics scene to draw on
//A, B, C, D - vertices of first rectangle (strip cross-section)
//flag - boolean variable to distinguish XZ and YZ projetions
void TrackViewer::draw_projection(QGraphicsScene* sc, int A, int B, int C, int D, bool flag)
{
    QPolygon strip[3][2][96], layer[3][2];   //Initializing rectangles to draw
    int x1, x2, y1, y2;                      //Vertices of rectangles
    x1 = A;
    x2 = B;


    //True == XZ projection, False == YZ projection
    if(flag == true)    y1 = C, y2 = D;
    else                y1 = C + 5, y2 = D + 5;

    //Drawing narrow rectangles
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 96; k++)
            {
                strip[i][j][k] << QPoint(x1, y1) << QPoint(x2, y1) << QPoint(x2, y2) << QPoint(x1, y2);
                sc->addPolygon(strip[i][j][k]);
                x1 += 10;       //Next rectangle
                x2 += 10;       //
            }
            y1 -= 8;            //Next layer in plane
            y2 -= 8;            //
            x1 = A + 5;         //
            x2 = B + 5;         //
        }

        x1 = A;                 //Next plane
        x2 = B;                 //
        y1 -= 185;              //
        y2 -= 185;              //
    }

    //Drawing wide rectangles
    int length = 965;  //strip length in pixels
    x1 = A;
    x2 = x1 + length;

    //True == XZ projection, False == YZ projection
    if(flag == true)    y1 = C + 20, y2 = D + 20;
    else                y1 = C - 15, y2 = D - 15;

    for(int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            layer[i][j] << QPoint(x1, y1) << QPoint(x2, y1) << QPoint(x2, y2) << QPoint(x1, y2);
            sc->addPolygon(layer[i][j]);
            y1 -= 8;
            y2 -= 8;
        }

        y1 -= 185;
        y2 -= 185;
    }
}

//Одноразовая функция: заполняет таблицу соответствия
//x0, y0 - start positions, start_num - Ncopy of first strip
void TrackViewer::correspondence_table(int x0, int y0, int start_num)
{
    int l = start_num;
    int x = x0, y = y0;

    //Filling XZ
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 96; k++)
            {
                Xvertices[l] = x;
                Yvertices[l] = y;
                x += 10;
                l++;
            }

            x = x0 + 5;
            y -= 8;
        }

        l += 193;
        x = x0;
        y -= 185;
    }
}

//Function to read data from file
void ReadEventData(int num)
{
    //Variables
    std::ifstream data;
    char* title = new char[100];
    std::ostringstream Path;
    std::string name;
    double energy, x, y, z;

    //Opening file
    Path << "G:\\MMH_reconst_data\\CHARGED_MATRIX\\M03\\ChargedMatrix" << std::setfill('0') << std::setw(5) << num << ".dat";
    data.open(Path.str(), std::ios_base::in);

    if (data.is_open())
    {
        data.getline(title, 100);

        //Reading data
        while (!data.eof())
        {
            data >> name >> energy >> x >> y >> z;

            //If it's charged - add coordinates to charged vectors
            if (name == "e+" || name == "e-" || name == "proton" || name == "pi+" || name == "pi-")
            {
                coord_charged[0].push_back(x);
                coord_charged[1].push_back(y);
                coord_charged[2].push_back(z);
                coordinates[0].push_back(x);
                coordinates[1].push_back(y);
                coordinates[2].push_back(z);
            }

            //If it's muon - add coordinates to charged vectors
            if (name == "mu+" || name == "mu-")
            {
                coord_mu[0].push_back(x);
                coord_mu[1].push_back(y);
                coord_mu[2].push_back(z);
                coordinates[0].push_back(x);
                coordinates[1].push_back(y);
                coordinates[2].push_back(z);
            }
        }
    }

    else std::cout << "Error!";
    data.close();
    delete[] title;
}

//Show next of previous event
void TrackViewer::event_switch()
{
    QString newEventNum;
    if(ui->Prev_ev->isChecked() && EventNum > 0)
    {
        EventNum--;
        newEventNum = QString::number(EventNum, 'g', 5);
        ui->event_num->setText(newEventNum);
        for(int i = 0; i < 3; i++)
        {
            coord_mu[i].clear();
            coord_charged[i].clear();
        }
        ReadEventData(EventNum);
        //Clear everything
        ui->graph_1->clearGraphs();
        ui->graph_2->clearGraphs();


        //Add muon points (upper graph)
        ui->graph_1->addGraph();
        ui->graph_1->graph(0)->setData(coord_mu[0], coord_mu[2]);
        ui->graph_1->graph(0)->setPen(QPen(Qt::red));
        ui->graph_1->graph(0)->setLineStyle(QCPGraph::lsNone);
        ui->graph_1->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        //Add charged points (upper graph)
        ui->graph_1->addGraph();
        ui->graph_1->graph(1)->setData(coord_charged[0], coord_charged[2]);
        ui->graph_1->graph(1)->setPen(QPen(Qt::blue));
        ui->graph_1->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->graph_1->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        ui->graph_1->replot();

        //Add muon points (lower graph)
        ui->graph_2->addGraph();
        ui->graph_2->graph(0)->setData(coord_mu[1], coord_mu[2]);
        ui->graph_2->graph(0)->setPen(QPen(Qt::red));
        ui->graph_2->graph(0)->setLineStyle(QCPGraph::lsNone);
        ui->graph_2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        //Add charged points (lower graph)
        ui->graph_2->addGraph();
        ui->graph_2->graph(1)->setData(coord_charged[1], coord_charged[2]);
        ui->graph_2->graph(1)->setPen(QPen(Qt::blue));
        ui->graph_2->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->graph_2->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        ui->graph_2->replot();
        ui->Prev_ev->setChecked(true);
    }

    if(ui->Next_ev->isChecked() && EventNum < 10000)
    {
        EventNum++;
        newEventNum = QString::number(EventNum, 'g', 5);
        ui->event_num->setText(newEventNum);
        for(int i = 0; i < 3; i++)
        {
            coord_mu[i].clear();
            coord_charged[i].clear();
        }
        ReadEventData(EventNum);
        //Clear everything
        ui->graph_1->clearGraphs();
        ui->graph_2->clearGraphs();


        //Add muon points (upper graph)
        ui->graph_1->addGraph();
        ui->graph_1->graph(0)->setData(coord_mu[0], coord_mu[2]);
        ui->graph_1->graph(0)->setPen(QPen(Qt::red));
        ui->graph_1->graph(0)->setLineStyle(QCPGraph::lsNone);
        ui->graph_1->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        //Add charged points (upper graph)
        ui->graph_1->addGraph();
        ui->graph_1->graph(1)->setData(coord_charged[0], coord_charged[2]);
        ui->graph_1->graph(1)->setPen(QPen(Qt::blue));
        ui->graph_1->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->graph_1->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        ui->graph_1->replot();

        //Add muon points (lower graph)
        ui->graph_2->addGraph();
        ui->graph_2->graph(0)->setData(coord_mu[1], coord_mu[2]);
        ui->graph_2->graph(0)->setPen(QPen(Qt::red));
        ui->graph_2->graph(0)->setLineStyle(QCPGraph::lsNone);
        ui->graph_2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        //Add charged points (lower graph)
        ui->graph_2->addGraph();
        ui->graph_2->graph(1)->setData(coord_charged[1], coord_charged[2]);
        ui->graph_2->graph(1)->setPen(QPen(Qt::blue));
        ui->graph_2->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->graph_2->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        ui->graph_2->replot();
        ui->Next_ev->setChecked(true);
    }

    //Сюда добавить функции стирающие точки, реконструкцию и чтение следующего файла
    ui->graph_1->clearGraphs();
    ui->graph_2->clearGraphs();
    ui->Prev_ev->setChecked(false);
    ui->Next_ev->setChecked(false);
}


//Drawing cooordinates of partices in MMH
void TrackViewer::draw_dots()
{
    //If button is pressed points should be revealed
    if(ui->show_pts->isChecked())
    {
        //Clear everything
       // ui->graph_1->clearGraphs();
        //ui->graph_2->clearGraphs();


        /* //Add muon points (upper graph)
        ui->graph_1->addGraph();
        ui->graph_1->graph(0)->setData(coord_mu[0], coord_mu[2]);
        ui->graph_1->graph(0)->setPen(QPen(Qt::red));
        ui->graph_1->graph(0)->setLineStyle(QCPGraph::lsNone);
        ui->graph_1->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        //Add charged points (upper graph)
        ui->graph_1->addGraph();
        ui->graph_1->graph(1)->setData(coord_charged[0], coord_charged[2]);
        ui->graph_1->graph(1)->setPen(QPen(Qt::blue));
        ui->graph_1->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->graph_1->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        ui->graph_1->replot(); */

        /*//Add muon points (lower graph)
        ui->graph_2->addGraph();
        ui->graph_2->graph(0)->setData(coord_mu[1], coord_mu[2]);
        ui->graph_2->graph(0)->setPen(QPen(Qt::blue));
        ui->graph_2->graph(0)->setLineStyle(QCPGraph::lsNone);
        ui->graph_2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        //Add charged points (lower graph)
        ui->graph_2->addGraph();
        ui->graph_2->graph(1)->setData(coord_charged[1], coord_charged[2]);
        ui->graph_2->graph(1)->setPen(QPen(Qt::red));
        ui->graph_2->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->graph_2->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        ui->graph_2->replot(); */

        ui->show_pts->setChecked(true);
    }

    ui->show_pts->setChecked(false);
}

//Reconstruction method
void KalmanFilterMethod(int a)
{

}

//Drawing reconstructed track with Kalman-filter method
void TrackViewer::draw_reconstructed_track()
{

}


//Enter event number with keyboard (realization will be later)
void TrackViewer::keyPressEvent(QKeyEvent *e)
{

}
