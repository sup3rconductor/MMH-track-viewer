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
QVector<double> XZcoordinates[2]; //0 - x, 1 - z
QVector<double> YZcoordinates[2]; //0 - y, 1 - z
QVector<int> copyNumber, copyNumXZ, copyNumYZ;

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
    ui->graph_1->xAxis->setRange(-492, 492);
    ui->graph_1->yAxis->setRange(-500, 500);
    ui->graph_2->xAxis->setRange(-492, 492);
    ui->graph_2->yAxis->setRange(-500, 500);

    /*//Убирает оси (дезигнерское решение)
    ui->graph_1->xAxis->setVisible(false);
    ui->graph_1->yAxis->setVisible(false);
    ui->graph_2->xAxis->setVisible(false);
    ui->graph_2->yAxis->setVisible(false);*/

    //Adding scenes
    XZ_scene = new QGraphicsScene();
    YZ_scene = new QGraphicsScene();
    XZ_new_scene = new QGraphicsScene();
    YZ_new_scene = new QGraphicsScene();
    //strp = new QGraphicsRectItem [20];

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
    XZ_scene->setSceneRect(-540, -238, 1080, 476);
    YZ_scene->setSceneRect(-540, -238, 1080, 476);

    XZ_new_scene->setSceneRect(-540, -238, 1080, 476);
    YZ_new_scene->setSceneRect(-540, -238, 1080, 476);

    //Drawing MMH
    draw_projection(XZ_scene, -540, -530, 209, 216, true);
    draw_projection(YZ_scene, -540, -530, 224, 231, false);
    strip_number_to_coords();
    //test_draw();
    //test_calibration();
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
    data.open("G:\\MMH-track-viewer-ver-2.0\\strip_coords.dat", std::ios_base::in);

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

//Test block - calibrating drawing
void TrackViewer::test_calibration()
{
    QPolygon strip[16][2];
    QBrush brush;
    QPen pen;

    brush.setColor(Qt::gray);
    brush.setStyle(Qt::SolidPattern);
    pen.setColor(Qt::black);

    //int copies[24] = {192, 193, 288, 289, 576, 577, 672, 673, 960, 961, 1056, 1057, 0, 1, 96, 97, 384, 385, 480, 481, 768, 769, 864, 865};

    //int k = 0;
    //int l;

    /*for(auto i = 0; i < 16; i++)
    {
        l = copies[k];
        strip[i][0] << QPoint(Xvertices[l], Yvertices[l]) << QPoint(Xvertices[l] + 10, Yvertices[l]) <<
            QPoint(Xvertices[l] + 10, Yvertices[l] + 7) << QPoint(Xvertices[l], Yvertices[l] + 7);
        XZ_scene->addPolygon(strip[i][0], pen, brush);
        k++;

    }

    for(auto i = 0; i < 12; i++)
    {
        l = copies[k];
        strip[i][1] << QPoint(Xvertices[l], Yvertices[l]) << QPoint(Xvertices[l] + 10, Yvertices[l]) <<
            QPoint(Xvertices[l] + 10, Yvertices[l] + 7) << QPoint(Xvertices[l], Yvertices[l] + 7);
        YZ_scene->addPolygon(strip[i][1], pen, brush);
        k++;
    } */

    /*int x, y;

    //Try to paint all of them
    for(auto i = 0; i < 1152; i++)
    {
        x = Xvertices[i];
        y = Yvertices[i];

        strip[i] << QPoint(x, y) << QPoint(x + 10, y) << QPoint(x + 10, y + 7) << QPoint(x, y + 7);

        //XZ
        if((i >= 192 && i <= 383) || (i >= 576 && i <= 767) || (i >= 960 && i <= 1151))
        {
            XZ_scene->addPolygon(strip[i], pen, brush);
        }


        //YZ
        if((i >= 0 && i <= 191) || (i >= 384 && i <= 575) || (i >= 768 && i <= 959))
        {
            YZ_scene->addPolygon(strip[i], pen, brush);
        }
    }*/

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

//Function to read data from file
void ReadEventData(int num)
{
    //Variables
    std::ifstream data, event;
    char* title = new char[100];
    std::ostringstream Path;
    std::string name;
    double energy, x, y, z, Time, Copy;

    //Opening file
    Path << "E:\\MMH_reconst_data\\CHARGED_MATRIX\\M03\\ChargedMatrix" << std::setfill('0') << std::setw(5) << num << ".dat";
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

                //Getting particles coordinates to draw triggered strips
                if((z >= -480 && z <= -473) || (z >= -472 && z <= -465) || (z >= 3 && z <= 10) || (z >= 11 && z <= 18) || (z >= 485 && z <= 492) || (z >= 493 && z <= 500))
                {
                    XZcoordinates[0].push_back(x);
                    XZcoordinates[1].push_back(z);
                }

                else if((z >= -500 && z <= -493) || (z >= -492 && z <= -485) || (z >= -17 && z <= -10) || (z >= -9 && z <= -2) || (z >= 465 && z <= 472) || (z >= 473 && z <= 480))
                {
                    YZcoordinates[0].push_back(y);
                    YZcoordinates[1].push_back(z);
                }
            }

            //If it's muon - add coordinates to charged vectors
            if (name == "mu+" || name == "mu-")
            {
                coord_mu[0].push_back(x);
                coord_mu[1].push_back(y);
                coord_mu[2].push_back(z);

                //Getting particles coordinates to draw triggered strips
                if((z >= -480 && z <= -473) || (z >= -472 && z <= -465) || (z >= 3 && z <= 10) || (z >= 11 && z <= 18) || (z >= 485 && z <= 492) || (z >= 493 && z <= 500))
                {
                    XZcoordinates[0].push_back(x);
                    XZcoordinates[1].push_back(z);
                }

                else if((z >= -500 && z <= -493) || (z >= -492 && z <= -485) || (z >= -17 && z <= -10) || (z >= -9 && z <= -2) || (z >= 465 && z <= 472) || (z >= 473 && z <= 480))
                {
                    YZcoordinates[0].push_back(y);
                    YZcoordinates[1].push_back(z);
                }
            }
        }
    }

    else std::cout << "Error!";
    data.close();
    Path.str("");
    Path.clear();

    QVector<int> buffer;
    Path << "E:\\MMH_reconst_data\\EVENTS_DATA\\M03\\Event" << std::setfill('0') << std::setw(5) << num << ".dat";
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

    int value_to_remove;
    while (!buffer.empty())
    {
        value_to_remove = buffer[0];
        copyNumber.push_back(value_to_remove);
        buffer.erase(std::remove(buffer.begin(), buffer.end(), value_to_remove), buffer.end());
    }

}

void TrackViewer::test_draw()
{
    int x, y;
    int copies[16] = {90, 91, 186, 213, 309, 414, 539, 540, 623, 624, 719, 749, 798, 893, 1034, 1130};
    QPolygon str[16];

    QBrush brush;
    QPen pen;

    brush.setColor(Qt::gray);
    brush.setStyle(Qt::SolidPattern);
    pen.setColor(Qt::black);

    for(auto i = 0; i < 16; i++)
    {
        x = Xvertices[copies[i]];
        y = Yvertices[copies[i]];

        str[i] << QPoint(x, y) << QPoint(x + 10, y) << QPoint(x + 10, y + 7) << QPoint(x, y + 7);

        //XZ
        if((copies[i] >= 192 && copies[i] <= 383) || (copies[i] >= 576 && copies[i] <= 767) || (copies[i] >= 960 && copies[i] <= 1151))
        {
            XZ_scene->addPolygon(str[i], pen, brush);
        }


        //YZ
        else if((copies[i] >= 0 && copies[i] <= 191) || (copies[i] >= 384 && copies[i] <= 575) || (copies[i] >= 768 && copies[i] <= 959))
        {
            YZ_scene->addPolygon(str[i], pen, brush);
        }
    }

}

//Function to draw triggered strips
void TrackViewer::draw_strips()
{
    QPolygon STRP[1152];

    int x, y, N;

    QBrush brush;
    QPen pen;

    brush.setColor(Qt::gray);
    brush.setStyle(Qt::NoBrush);
    pen.setColor(Qt::green);

        for(auto i = 0; i < copyNumXZ.size(); i++)
        {
            N = copyNumXZ[i];
            x = Xvertices[N];
            y = Yvertices[N];

            STRP[i] << QPoint(x, y) << QPoint(x + 10, y) << QPoint(x + 10, y + 7) << QPoint(x, y + 7);
            XZ_new_scene->addPolygon(STRP[i], pen, brush);
        }

        for(auto i = 0; i < copyNumYZ.size(); i++)
        {
            N = copyNumYZ[i];
            x = Xvertices[N];
            y = Yvertices[N];

            STRP[i] << QPoint(x, y) << QPoint(x + 10, y) << QPoint(x + 10, y + 7) << QPoint(x, y + 7);
            YZ_new_scene->addPolygon(STRP[i], pen, brush);
        }

}

//Get copy numbers of triggered strips
void TrackViewer::GetCopyNumber()
{
        double Zpt, Xpt;
        double x_left, x_right;
        int plain_num = 0, cp;
        double x_min = 0, x_max = 0;
        int MinCopy[12] = {  0, 96, 192, 288, 384, 480, 576, 672, 768, 864, 960, 1056};
        int MaxCopy[12] = {95, 191, 287, 383, 479, 575, 671, 767, 863, 959, 1055, 1151};
        double Xmin[12] = {-492., -487., -492., -487., -492., -487., -492., -487., -492., -487., -492., -487.};
        double Xmax[12] = {468., 473., 468., 473., 468., 473., 468., 473., 468., 473., 468., 473.};

        //Filling for XZ
        for(auto i = 0; i < XZcoordinates[1].size(); i++)
        {
            Zpt = XZcoordinates[1][i];
            Xpt = XZcoordinates[0][i];

            if(Zpt >= -500 && Zpt <= -493) plain_num = 0;         //1
            else if(Zpt >= -492 && Zpt <= -485) plain_num = 1;    //2
            else if(Zpt >= -17 && Zpt <= -10) plain_num = 4;      //5
            else if(Zpt >= -9 && Zpt <= -2) plain_num = 5;        //6
            else if(Zpt >= 465 && Zpt <= 472) plain_num = 8;      //9
            else if(Zpt >= 473 && Zpt <= 480) plain_num = 9;      //10

            cp = MinCopy[plain_num];
            x_min = Xmin[plain_num], x_max = Xmax[plain_num];
            x_left = x_min, x_right = x_min + 10;
            if(Xpt > x_min && Xpt < x_max)
            {
                while(!(Xpt >= x_left && Xpt <= x_right)  && (cp <= MaxCopy[plain_num]))
                {
                    x_left += 10;
                    x_right += 10;
                    cp++;
                }
                copyNumXZ.push_back(cp);
            }
        }

        //Filling for YZ
        for(auto i = 0; i < YZcoordinates[1].size(); i++)
        {
            Zpt = YZcoordinates[1][i];
            Xpt = YZcoordinates[0][i];

            if(Zpt >= -480 && Zpt <= -473) plain_num = 2;         //3
            else if(Zpt >= -472 && Zpt <= -465) plain_num = 3;    //4
            else if(Zpt >= 3 && Zpt <= 10) plain_num = 6;         //7
            else if(Zpt >= 11 && Zpt <= 18) plain_num = 7;        //8
            else if(Zpt >= 485 && Zpt <= 492) plain_num = 10;     //11
            else if(Zpt >= 493 && Zpt <= 500) plain_num = 11;     //12

            cp = MinCopy[plain_num];
            x_min = Xmin[plain_num], x_max = Xmax[plain_num];
            x_left = x_min, x_right = x_min + 10;
            if(Xpt > x_min && Xpt < x_max)
            {
                while(!(Xpt >= x_left && Xpt <= x_right) && (cp <= MaxCopy[plain_num]))
                {
                    x_left += 10;
                    x_right += 10;
                    cp++;
                }
                copyNumYZ.push_back(cp);
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
        XZcoordinates[0].clear();
        XZcoordinates[1].clear();
        copyNumber.clear();
        copyNumXZ.clear();
        copyNumYZ.clear();

        ReadEventData(EventNum);
        draw_dots();
        GetCopyNumber();
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
        XZcoordinates[0].clear();
        XZcoordinates[1].clear();
        copyNumber.clear();
        copyNumXZ.clear();
        copyNumYZ.clear();

        ReadEventData(EventNum);
        draw_dots();
        GetCopyNumber();
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
    ui->graph_1->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));

    //Add charged points (upper graph)
    ui->graph_1->addGraph();
    ui->graph_1->graph(1)->setData(coord_charged[0], coord_charged[2]);
    ui->graph_1->graph(1)->setPen(QPen(Qt::blue));
    ui->graph_1->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->graph_1->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));

    ui->graph_1->replot();

    //Add muon points (lower graph)
    ui->graph_2->addGraph();
    ui->graph_2->graph(0)->setData(coord_mu[1], coord_mu[2]);
    ui->graph_2->graph(0)->setPen(QPen(Qt::red));
    ui->graph_2->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->graph_2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));

    //Add charged points (lower graph)
    ui->graph_2->addGraph();
    ui->graph_2->graph(1)->setData(coord_charged[1], coord_charged[2]);
    ui->graph_2->graph(1)->setPen(QPen(Qt::blue));
    ui->graph_2->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->graph_2->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));

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
