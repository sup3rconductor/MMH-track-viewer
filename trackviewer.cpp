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

//Global variables: event number, coordinates of muons and charged particles, track parameters ()
int EventNum = -1;
QVector<double> coord_mu[3];
QVector<double> coord_charged[3];
QVector<int> copyNumber;

int Xvertices[1152] = {0};
int Yvertices[1152] = {0};

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
    //ui->graph_1->xAxis->setVisible(false);
    //ui->graph_1->yAxis->setVisible(false);
    //ui->graph_2->xAxis->setVisible(false);
    //ui->graph_2->yAxis->setVisible(false);

    //Adding scenes
    XZ_scene = new QGraphicsScene();
    YZ_scene = new QGraphicsScene();
    XZ_new_scene = new QGraphicsScene();
    YZ_new_scene = new QGraphicsScene();

    //Setting scenes
    ui->XZ_proj->setScene(XZ_scene);
    ui->YZ_proj->setScene(YZ_scene);
    ui->XZ_proj->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->XZ_proj->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->YZ_proj->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->YZ_proj->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->XZ_trig->setScene(XZ_new_scene);
    ui->YZ_trig->setScene(YZ_new_scene);
    ui->XZ_trig->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->XZ_trig->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->YZ_trig->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->YZ_trig->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //Limiting scene size
    XZ_scene->setSceneRect(-540, -238, 1080, 500);
    YZ_scene->setSceneRect(-540, -238, 1080, 500);

    XZ_new_scene->setSceneRect(-540, -238, 1080, 500);
    YZ_new_scene->setSceneRect(-540, -238, 1080, 500);

    //Drawing MMH
    draw_projection(XZ_scene, -540, -530, 209, 213, true);
    draw_projection(YZ_scene, -540, -530, 224, 228, false);
    strip_number_to_coords();
}

TrackViewer::~TrackViewer()
{
    delete ui;
}

//Function to connect strip copy nubers with it's coordinates
void TrackViewer::strip_number_to_coords()
{
    int number, x, y;
    std::ifstream data;
    data.open("C:\\Users\\HYPERPC\\Documents\\MMH-track-viewer-ver-2.1\\strip_coords.dat", std::ios_base::in);

    if(data.is_open())
    {
        for (int i = 0; i < 1152; i++)
        {
            data >> number >> x >> y;
            Xvertices[i] = x;
            Yvertices[i] = y;
        }
    }

    else std::cout << "Error...";
    data.close();
}


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
    else                y1 = C + 3, y2 = D + 3;

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
            y1 -= 5;            //Next layer in plane
            y2 -= 5;            //
            x1 = A + 5;         //
            x2 = B + 5;         //
        }

        x1 = A;                 //Next plane
        x2 = B;                 //
        y1 -= 202;              //
        y2 -= 202;              //
    }

    //Drawing wide rectangles
    int length = 965;  //strip length in pixels
    x1 = A;
    x2 = x1 + length;

    //True == XZ projection, False == YZ projection
    if(flag == true)    y1 = C + 11, y2 = D + 11;
    else                y1 = C - 9, y2 = D - 9;

    for(int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            layer[i][j] << QPoint(x1, y1) << QPoint(x2, y1) << QPoint(x2, y2) << QPoint(x1, y2);
            sc->addPolygon(layer[i][j]);
            y1 -= 5;
            y2 -= 5;
        }

        y1 -= 202;
        y2 -= 202;
    }
}

//Function to read data from file
void ReadEventData(int num)
{
    //Variables
    std::ifstream data, event;
    char* title = new char[100];
    std::ostringstream Path;
    std::string name;
    QVector<double> buffer_mu[3], buffer_ch[3];
    double energy, x, y, z, Time, Copy;

    //Opening file
    Path << "E:\\MMH_reconst_data\\CHARGED_MATRIX\\M04\\ChargedMatrix" << std::setfill('0') << std::setw(5) << num << ".dat";
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
            }

            //If it's muon - add coordinates to charged vectors
            if (name == "mu+" || name == "mu-")
            {
                coord_mu[0].push_back(x);
                coord_mu[1].push_back(y);
                coord_mu[2].push_back(z);
            }
        }
    }

    else std::cout << "Error!";
    data.close();
    Path.str("");
    Path.clear();


    QVector<int> buffer;
    Path << "E:\\MMH_reconst_data\\EVENTS_DATA\\M04\\Event" << std::setfill('0') << std::setw(5) << num << ".dat";
    event.open(Path.str(), std::ios_base::in);

    if (event.is_open())
    {
        event.getline(title, 100);

        //Reading data
        while (!event.eof())
        {
            event >> energy >> Time >> Copy;
            buffer.push_back(Copy);
        }
    }

    else std::cout << "Error!";
    event.close();
    delete[] title;

    int remove_val;
    while (!buffer.empty())
    {
        remove_val = buffer[0];
        copyNumber.push_back(remove_val);
        buffer.erase(std::remove(buffer.begin(), buffer.end(), remove_val), buffer.end());
    }

}


void TrackViewer::draw_strips()
{
    QPolygon STRP[1152];

    int x, y, N;

    QBrush brush;
    QPen pen;

    brush.setColor(Qt::gray);
    brush.setStyle(Qt::NoBrush);
    pen.setColor(Qt::green);

        for(auto i = 0; i < copyNumber.size(); i++)
        {
            N = copyNumber[i];
            x = Xvertices[N];
            y = Yvertices[N];

            STRP[i] << QPoint(x, y) << QPoint(x + 10, y) << QPoint(x + 10, y + 4) << QPoint(x, y + 4);

            //XZ
            if((N >= 192 && N <= 383) || (N >= 576 && N <= 767) || (N >= 960 && N <= 1151))
            {
                XZ_new_scene->addPolygon(STRP[i], pen, brush);
            }


            //YZ
            else if((N >= 0 && i <= 191) || (N >= 384 && N <= 575) || (N >= 768 && N <= 959))
            {
                YZ_new_scene->addPolygon(STRP[i], pen, brush);
            }
        }

}

//Show next or previous event
void TrackViewer::event_switch()
{
    QString newEventNum;
    if(ui->Prev_ev->isChecked() && EventNum > 0)
    {
        XZ_new_scene->clear();
        YZ_new_scene->clear();

        EventNum--;
        newEventNum = QString::number(EventNum, 'g', 5);
        ui->event_num->setText(newEventNum);
        for(int i = 0; i < 3; i++)
        {
            coord_mu[i].clear();
            coord_charged[i].clear();
        }
        copyNumber.clear();

        ReadEventData(EventNum);
        draw_dots();
        draw_strips();
        ui->Prev_ev->setChecked(true);
    }

    if(ui->Next_ev->isChecked() && EventNum < 10000)
    {
        XZ_new_scene->clear();
        YZ_new_scene->clear();

        EventNum++;
        newEventNum = QString::number(EventNum, 'g', 5);
        ui->event_num->setText(newEventNum);
        for(int i = 0; i < 3; i++)
        {
            coord_mu[i].clear();
            coord_charged[i].clear();
        }
        copyNumber.clear();

        ReadEventData(EventNum);
        draw_dots();
        draw_strips();
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
    //Clear everything
    ui->graph_1->clearGraphs();
    ui->graph_2->clearGraphs();

    //Add muon points (upper graph)
    ui->graph_1->addGraph();
    ui->graph_1->graph(0)->setData(coord_mu[0], coord_mu[2]);
    ui->graph_1->graph(0)->setPen(QPen(Qt::red));
    ui->graph_1->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->graph_1->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

    //Add charged points (upper graph)
    ui->graph_1->addGraph();
    ui->graph_1->graph(1)->setData(coord_charged[0], coord_charged[2]);
    ui->graph_1->graph(1)->setPen(QPen(Qt::blue));
    ui->graph_1->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->graph_1->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

    ui->graph_1->replot();

    //Add muon points (lower graph)
    ui->graph_2->addGraph();
    ui->graph_2->graph(0)->setData(coord_mu[1], coord_mu[2]);
    ui->graph_2->graph(0)->setPen(QPen(Qt::red));
    ui->graph_2->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->graph_2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

    //Add charged points (lower graph)
    ui->graph_2->addGraph();
    ui->graph_2->graph(1)->setData(coord_charged[1], coord_charged[2]);
    ui->graph_2->graph(1)->setPen(QPen(Qt::blue));
    ui->graph_2->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->graph_2->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

    ui->graph_2->replot();
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
