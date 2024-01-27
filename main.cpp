#include "trackviewer.h"

#include <QApplication>

#include <vector>
#include <fstream>


void read_coordintes_data(int num)
{
    std::ifstream data;
    char* path = new char[70];
    char* title = new char[100];
    std::vector<int> part_name;
    std::vector<int> coord[3];

    sprintf(path, "E:\\MMH_reconst_data\\CHARGED_MATRIX\\M03\\ChargedMatrix%d05.dat", num);
    data.open(path, std::ios_base::in);
    data.getline(title, 100);
}





int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TrackViewer w;
    w.show();
    return a.exec();
}
