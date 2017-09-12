#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <vector>
#include <Eigen/Eigen>
#include <QVBoxLayout>
#include <QGroupBox>
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <vector>

using namespace std;
using namespace cv;
using namespace Eigen;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_display_src_btn_clicked();

    void on_img_match_btn_clicked();

    void on_img_correct_btn_clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;
    QImage src_img;
    QImage *target_img;
    std::vector<QPoint> src_point_vector;
    std::vector<QPoint> target_point_vector;
    std::vector<double> param_vec;
    bool traversalControl(QVBoxLayout& q);
    //计算c1, c2, c3, c4, c5,c6,c7,c8
    void computeParam(int type);
    void computeMatrix();
    Matrix4f  m;

    Vector4f c1_4;
    Vector4f c5_8;
    QImage CvtColorToGray(QImage& src_image);
    QPoint ProjTrans(QPoint);
    int InterpBilinear(double x, double y);

    Mat PerspectiveTrans(Mat src, Point2f* scrPoints, Point2f* dstPoints);
    cv::Mat MainWindow::QImage2Mat(QImage image);
    QImage MainWindow::Mat2QImage(const cv::Mat& mat);

};

#endif // MAINWINDOW_H
