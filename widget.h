#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QTimer>
#include <QPixmap>
#include <QPoint>
#include <QPainter>
#include <QMovie>
#include <QSystemTrayIcon>
#include <QMenu>
//3.0新增
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include<QJsonArray>
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots://功能定义
    void sayHello();
    void feed();
    void setStateStay();
    void setStateHappy();
    void setStateQuesting();
    void setStateStudying();
    void setStateSleep();
    void setStateTiring();
    void hidePet();
    void showPet();
    void showDialog(const QString &text);
    void clearDialog();
    //3.0
    void playMusic1();
    void playMusic2();
    void playMusic3();
    void playMusic4();
    void playMusic5();
    void playMusic6();
    void stopMusic();
    void sendAI();
    void getReply(QNetworkReply *reply);

private:
    void Menu();          //右键菜单
    void loadStateImage();

    enum PetState { Stay, Happy, Questing, Studying, Sleeping, Tired };
    PetState m_currentState;

    bool m_isDialogVisible;
    QString m_dialogText;
    QPoint m_dragPos;
    QTimer *m_dialogTimer;
    QMenu *m_rightMenu;
    QMovie *m_movie;
    QPixmap m_currentPixmap;
    QSystemTrayIcon *m_trayIcon;//托盘
    QMenu *m_trayMenu;
    //3.0
    //音乐
    QMediaPlayer *m_player;
    QAudioOutput *m_audio;

    //AI
    QNetworkAccessManager *net;
    QLineEdit *input;
    QPushButton *btn;
};

#endif // WIDGET_H