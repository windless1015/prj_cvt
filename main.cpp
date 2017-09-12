#include "mainwindow.h"
#include <QApplication>




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    Eigen::Vector2d v1, v2;     //Eigen中的变量
//        v1 << 5, 6;   //默认的向量为列向量
//        std::cout  << "v1 = " << std::endl << v1 << std::endl;
//        v2 << 4, 5 ;
//        Matrix2d result = v1*v2.transpose();

//        std::cout << "result: " << std::endl << result << std::endl;

    return a.exec();
}
