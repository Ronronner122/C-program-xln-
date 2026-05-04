#include "widget.h"
#include <QApplication>
#include <ctime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置随机数种子（对话随机）
    srand(time(0));
    Widget w;
    w.show();
    return a.exec();
}