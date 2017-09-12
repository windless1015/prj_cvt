#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <QMessageBox>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <QMouseEvent>
#include <QPoint>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m.resize(4,4);
    target_img = new QImage(ui->srcDisplay_2->width(), ui->srcDisplay_2->height(), QImage::Format_Indexed8);
    QVector<QRgb> table( 256 );
    for ( int i = 0 ; i < 256 ; ++ i ){
        table[i] = qRgb(i,i,i);
    }
    target_img->setColorTable( table );
    target_img->fill(255);
    //显示显示控件的位置
//    qDebug("%d",ui->srcDisplay_1->pos().x());
//    qDebug("%d",ui->srcDisplay_1->pos().y());
}

MainWindow::~MainWindow()
{
    delete target_img;
    delete ui;
}

/*显示原始图像*/
void MainWindow::on_display_src_btn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开图像"),"c:/User/st/Desktop/",
                                                       tr("Image (*.jpg *.jpeg *.bmp *.png)"));
    if(fileName.isEmpty())
        return;
    else{
        if(!(src_img.load(fileName))){
            QMessageBox::information(this, QString::fromLocal8Bit("打开图像失败"), QString::fromLocal8Bit("打开图像失败!"));
            return;
            }

        ui->srcDisplay_1->setPixmap(QPixmap::fromImage(src_img));
        ui->srcDisplay_1->setAlignment(Qt::AlignCenter);
        }
}

/*图像配准*/
void MainWindow::on_img_match_btn_clicked()
{
    src_img = CvtColorToGray(src_img);
    //先检测4对配准点的坐标是否有输入,如果有一个没有输入都提示出错
    bool flag[4];
    flag[0] = traversalControl(*(ui->verticalLayout_src_x));
    flag[1] = traversalControl(*(ui->verticalLayout_src_y));
    flag[2] = traversalControl(*(ui->verticalLayout_target_x));
    flag[3] = traversalControl(*(ui->verticalLayout_target_y));
    //bool ok = flag[0] && flag[1] && flag[2] && flag[3];
    bool ok = true;
    if(!ok)
         QMessageBox::information(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("请输入完整对应点坐标!"));
    else
    {
        bool ok;
        src_point_vector.push_back(QPoint(ui->lineEdit_src_x_1->text().toInt(&ok, 10), ui->lineEdit_src_y_1->text().toInt(&ok, 10)));
        src_point_vector.push_back(QPoint(ui->lineEdit_src_x_2->text().toInt(&ok, 10), ui->lineEdit_src_y_2->text().toInt(&ok, 10)));
        src_point_vector.push_back(QPoint(ui->lineEdit_src_x_3->text().toInt(&ok, 10), ui->lineEdit_src_y_3->text().toInt(&ok, 10)));
        src_point_vector.push_back(QPoint(ui->lineEdit_src_x_4->text().toInt(&ok, 10), ui->lineEdit_src_y_4->text().toInt(&ok, 10)));

        target_point_vector.push_back(QPoint(ui->lineEdit_target_x_1->text().toInt(&ok, 10), ui->lineEdit_target_y_1->text().toInt(&ok, 10)));
        target_point_vector.push_back(QPoint(ui->lineEdit_target_x_2->text().toInt(&ok, 10), ui->lineEdit_target_y_2->text().toInt(&ok, 10)));
        target_point_vector.push_back(QPoint(ui->lineEdit_target_x_3->text().toInt(&ok, 10), ui->lineEdit_target_y_3->text().toInt(&ok, 10)));
        target_point_vector.push_back(QPoint(ui->lineEdit_target_x_4->text().toInt(&ok, 10), ui->lineEdit_target_y_4->text().toInt(&ok, 10)));

//        src_point_vector.push_back(QPoint(41,118));
//        src_point_vector.push_back(QPoint(387,66));
//        src_point_vector.push_back(QPoint(83, 264));
//        src_point_vector.push_back(QPoint(317,218));

//        target_point_vector.push_back(QPoint(53,48));
//        target_point_vector.push_back(QPoint(253,48));
//        target_point_vector.push_back(QPoint(53,248));
//        target_point_vector.push_back(QPoint(253,248));

        computeMatrix();
        computeParam(1);
        computeParam(2);
        std::cout << c1_4;
        std::cout << c5_8;
    }
}

Mat MainWindow::PerspectiveTrans(Mat src, Point2f* scrPoints, Point2f* dstPoints)
{
    Mat dst;
    Mat Trans = getPerspectiveTransform(scrPoints, dstPoints);
    warpPerspective(src, dst, Trans, Size(src.cols, src.rows), CV_INTER_CUBIC);
    return dst;
}

/*图像校正*/
void MainWindow::on_img_correct_btn_clicked()
{

//    bool is_use_interp = false; //是否使用双线性差值
//    for(int i=0; i<src_img.height(); i++)
//        for(int j=0; j < src_img.width(); j++)
//        {
//            QPoint ptProj = ProjTrans(QPoint(i,j));
//            if(is_use_interp)
//            {
//                int nGray = InterpBilinear(ptProj.x(), ptProj.y());
//                if(nGray >= 0)
//                    target_img->setPixel(i,j, nGray);

//            }
//            else  //使用的是四舍五入的最邻近插值
//            {
//                int ii = ptProj.x() + 0.5;
//                int jj = ptProj.y() + 0.5;
//                if(ii > 0 && ii < src_img.width() && jj >0
//                   && jj < src_img.height()){
//                    QRgb pixel = src_img.pixel(ii,jj);
//                    target_img->setPixel(i,j, qGray(pixel));
//                }

//            }
//        }

    cv::Mat src_mat = QImage2Mat(src_img);
    Point2f src_point[4] = { Point2f(41, 118), Point2f(387, 65), Point2f(83, 264), Point2f(315, 218) };
    Point2f target_point[4] = { Point2f(10, 10), Point2f(210, 10), Point2f(10, 210), Point2f(210, 210) };
    Mat dst_perspective = PerspectiveTrans(src_mat, src_point, target_point);
    imshow("perspective", dst_perspective);
    waitKey();
    *target_img = Mat2QImage(dst_perspective);



    ui->srcDisplay_2->setPixmap(QPixmap::fromImage(*target_img));
    ui->srcDisplay_2->setAlignment(Qt::AlignCenter);
}


bool MainWindow::traversalControl(QVBoxLayout& q)
{
    bool flag = true;
    for(int i=0;i<q.count();i++)
    {
        QWidget* w = q.itemAt(i)->widget();
        if(w->inherits("QLineEdit"))
        {
            QLineEdit* b = qobject_cast<QLineEdit*>(w);
            if(b->text().isEmpty())
                flag = false;
        }

    }
    return flag;
}

/*线性方程求解 Ax =B;
    Matrix4d A;
    A << 2,-1,-1,1,
         1,1,-2,1,
         4,-6,2,-2,
         3,6,-9,7;
    Vector4d B(2,4,4,9);
    Vector4d x = A.colPivHouseholderQr().solve(B);
*/

void MainWindow::computeParam(int type)
{
    double arr[4] = {0};
    for(int j=0; j < 4; j++)
    {
        QPoint p = target_point_vector.at(j);
        if(type == 1)
          arr[j] = p.x();
        if(type == 2)
          arr[j] = p.y();
    }

    Vector4f B(arr[0], arr[1], arr[2], arr[3]);

    if(type == 1)
        c1_4 = m.colPivHouseholderQr().solve(B);
    if (type == 2)
        c5_8 = m.colPivHouseholderQr().solve(B);
}

void MainWindow::computeMatrix()
{
    double arr[16]  = {0};
    for (int i=0; i< 16; i = i + 4)
    {
        int aa = i / 4;
        QPoint p = src_point_vector.at(aa);
        arr[i] = p.x();
        arr[i+1] = p.y();
        arr[i+2] = p.x() * p.y();
        arr[i+3] = 1;
    }

    m << arr[0], arr[1], arr[2], arr[3],
         arr[4], arr[5],arr[6], arr[7],
         arr[8], arr[9], arr[10], arr[11],
         arr[12], arr[13],arr[14], arr[15];

    /*m << 41 ,  118 , 4838 ,    1,
         387 ,   66 ,25542   ,  1,
         83  , 264, 21912    , 1,
          317  , 218, 69106    , 1;*/

}

/*灰度变换,返回灰度图像*/
QImage MainWindow::CvtColorToGray(QImage& src_image)
{
    int height = src_image.height();
    int width = src_image.width();
    QImage ret(width, height, QImage::Format_Indexed8);
    ret.setColorCount(256);
    for(int i = 0; i < 256; i++)
    {
        ret.setColor(i, qRgb(i, i, i));
    }
    switch(src_image.format())
    {
        case QImage::Format_Indexed8:
            for(int i = 0; i < height; i ++)
            {
                const uchar *pSrc = (uchar *)src_image.constScanLine(i);
                uchar *pDest = (uchar *)ret.scanLine(i);
                memcpy(pDest, pSrc, width);
            }
            break;
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            for(int i = 0; i < height; i ++)
            {
                const QRgb *pSrc = (QRgb *)src_image.constScanLine(i);
                uchar *pDest = (uchar *)ret.scanLine(i);
                for( int j = 0; j < width; j ++)
                {
                     pDest[j] = qGray(pSrc[j]);
                }
            }
            break;
        }
        return ret;
}

/*原始点投影到目标点*/
QPoint MainWindow::ProjTrans(QPoint src)
{
    int x = int(c1_4[0]*src.x() + c1_4[1]*src.y()+ c1_4[2]*src.x()*src.y() + c1_4[3]);
    int y = int (c5_8[0]*src.x() + c5_8[1]*src.y()+ c5_8[2]*src.x()*src.y() + c5_8[3]);
    QPoint retPt(x, y);
    return retPt;
}


int MainWindow::InterpBilinear(double x, double y)
{
    //if(int(y)==300)
      //  int cc = 1;

    // 四个最临近象素的坐标(i1, j1), (i2, j1), (i1, j2), (i2, j2)
    int	x1, x2;
    int	y1, y2;

    // 四个最临近象素值
    unsigned char	f1, f2, f3, f4;

    // 二个插值中间值
    unsigned char	f12, f34;

    double	epsilon = 0.0001;

    // 计算四个最临近象素的坐标
    x1 = (int) x;
    x2 = x1 + 1;
    y1 = (int) y;
    y2 = y1 + 1;


    int nHeight = src_img.height();
    int nWidth = src_img.width();
    if( (x < 0) || (x > nWidth - 1) || (y < 0) || (y > nHeight - 1))
    {
        // 要计算的点不在源图范围内，返回-1
        return -1;
    }
    else
    {


        if (fabs(x - nWidth + 1) <= epsilon)
        {
            // 要计算的点在图像右边缘上
            if (fabs(y - nHeight + 1) <= epsilon)
            {
                // 要计算的点正好是图像最右下角那一个象素，直接返回该点象素值
                QRgb pixel = src_img.pixel(x1,y1);
                f1 = (unsigned char)qGray(pixel);
                //f1 = (unsigned char)GetGray( x1, y1 );
                return f1;
            }
            else
            {
                // 在图像右边缘上且不是最后一点，直接一次插值即可
                QRgb pixel = src_img.pixel(x1,y1);
                f1 = (unsigned char)qGray(pixel);
                pixel = src_img.pixel(x1,y2);
                f3 = (unsigned char)qGray(pixel);
                //f1 = (unsigned char)GetGray(x1, y1 );
                //f3 = (unsigned char)GetGray( x1, y2 );

                // 返回插值结果
                return ((int) (f1 + (y -y1) * (f3 - f1)));
            }
        }
        else if (fabs(y - nHeight + 1) <= epsilon)
        {
            // 要计算的点在图像下边缘上且不是最后一点，直接一次插值即可
            QRgb pixel = src_img.pixel(x1,y1);
            f1 = (unsigned char)qGray(pixel);
            pixel = src_img.pixel(x2,y1);
            f2 = (unsigned char)qGray(pixel);
            //f1 = (unsigned char)GetGray( x1, y1 );
            //f2 = (unsigned char)GetGray( x2, y1 );

            // 返回插值结果
            return ((int) (f1 + (x -x1) * (f2 - f1)));
        }
        else
        {
            // 计算四个最临近象素值
            QRgb pixel = src_img.pixel(x1,y1);
            f1 = (unsigned char)qGray(pixel);
            pixel = src_img.pixel(x2,y1);
            f2 = (unsigned char)qGray(pixel);
            pixel = src_img.pixel(x1,y2);
            f3 = (unsigned char)qGray(pixel);
            pixel = src_img.pixel(x2,y2);
            f4 = (unsigned char)qGray(pixel);
           /*
            f1 = (unsigned char)GetGray( x1, y1 );
            f2 = (unsigned char)GetGray( x2, y1 );
            f3 = (unsigned char)GetGray( x1, y2 );
            f4 = (unsigned char)GetGray( x2, y2 );
            */

            // 插值1
            f12 = (unsigned char) (f1 + (x - x1) * (f2 - f1));

            // 插值2
            f34 = (unsigned char) (f3 + (x - x1) * (f4 - f3));

            // 插值3
            return ((int) (f12 + (y -y1) * (f34 - f12)));
        }
    }
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int disp1_x = event->x() - ui->srcDisplay_1->pos().x(); //显示控件1左上顶点的横坐标
    int disp1_y = event->y() - ui->srcDisplay_1->pos().y();//显示控件1左上顶点的纵坐标
    int disp2_x = event->x() - ui->srcDisplay_2->pos().x(); //显示控件2左上顶点的横坐标
    int disp2_y = event->y() - ui->srcDisplay_2->pos().y();//显示控件2左上顶点的纵坐标

    int display_control_w = ui->srcDisplay_1->width();
    int display_control_h = ui->srcDisplay_1->height();

    if (event->button() == Qt::LeftButton  ) {
        //在第一个显示区域里面
        if(disp1_x >=0 && disp1_x <= display_control_w && disp1_y >=0 && disp1_y <= display_control_h)
        {
            QString temp = "x=" + QString::number(disp1_x) + ", y=" + QString::number(disp1_y);
            QMessageBox::information(this, QString::fromLocal8Bit("获取原始图像坐标"), temp);
        }
        else if(disp2_x >=0 && disp2_x <= display_control_w && disp2_y >=0 && disp2_y <= display_control_h)
        {
            QString temp = "x=" + QString::number(disp2_x) + ", y=" + QString::number(disp2_y);
            QMessageBox::information(this, QString::fromLocal8Bit("获取投影图像坐标"), temp);
        }


    }
}


QImage MainWindow::Mat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {

        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {

        return QImage();
    }
}


cv::Mat MainWindow::QImage2Mat(QImage image)
{
    cv::Mat mat;

    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

















