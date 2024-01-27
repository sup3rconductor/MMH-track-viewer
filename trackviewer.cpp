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
    ui->setupUi(this);
    connect(ui->Prev_ev, SIGNAL(clicked()), this, SLOT(event_switch()));
    connect(ui->Next_ev, SIGNAL(clicked()), this, SLOT(event_switch()));
    connect(ui->show_pts, SIGNAL(clicked()), this, SLOT(draw_dots()));
    connect(ui->show_reconst, SIGNAL(clicked()), this, SLOT(draw_reconstructed_track()));

    ui->Prev_ev->setCheckable(true);
    ui->Next_ev->setCheckable(true);
    ui->show_pts->setCheckable(true);
    ui->show_reconst->setCheckable(true);

    ui->graph_1->xAxis->setRange(-500, 500);
    ui->graph_1->yAxis->setRange(-500, 500);
    ui->graph_2->xAxis->setRange(-500, 500);
    ui->graph_2->yAxis->setRange(-500, 500);

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
    Path << "F:\\MMH_reconst_data\\CHARGED_MATRIX\\M03\\ChargedMatrix" << std::setfill('0') << std::setw(5) << num << ".dat";
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
        ui->graph_2->graph(0)->setPen(QPen(Qt::blue));
        ui->graph_2->graph(0)->setLineStyle(QCPGraph::lsNone);
        ui->graph_2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        //Add charged points (lower graph)
        ui->graph_2->addGraph();
        ui->graph_2->graph(1)->setData(coord_charged[1], coord_charged[2]);
        ui->graph_2->graph(1)->setPen(QPen(Qt::red));
        ui->graph_2->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->graph_2->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

        ui->graph_2->replot();

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
