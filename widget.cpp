#include "widget.h"
#include <QApplication>
#include <QScreen>
#include <QFont>
#include <cstdlib>
#include <ctime>

const QStringList helloTexts = {
    "我想和你一起生活，在某个小镇，共享无尽的黄昏",
    "愿我的小姑娘所得皆所愿，所行化坦途",
    "一杯美式，一杯拿铁，温度刚刚好，只等你的到来",
    "无论何时都可以呼唤温度名字，他就是我为你定制的咒语"
};

const QStringList feedTexts = {
    "小熊和小兔子都是小猪变得^_^",
    "相比于饮料，我觉得身边的你更让我觉得幸福",
    "快速决定午饭吃什么，应该算是很难的问题"
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , m_currentState(Stay)
    , m_isDialogVisible(false)
{
    setWindowFlags(Qt::FramelessWindowHint
                   | Qt::WindowStaysOnTopHint
                   | Qt::Tool);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);
    setFixedSize(150, 150);

    m_dialogTimer = new QTimer(this);
    connect(m_dialogTimer, &QTimer::timeout, this, &Widget::clearDialog);

    initMenu();
    loadStateImage();

    QScreen* screen = QApplication::primaryScreen();
    QRect rect = screen->availableGeometry();
    move(rect.width() - width() - 50, rect.height() - height() - 50);
}

Widget::~Widget()
{
}

void Widget::initMenu()
{
    m_rightMenu = new QMenu(this);

    QAction* actHello = new QAction("打招呼", this);
    QAction* actFeed = new QAction("喂食", this);
    QAction* actStay = new QAction("待机", this);
    QAction* actHappy = new QAction("开心", this);
    QAction* actQuest = new QAction("探索", this);
    QAction* actStudy = new QAction("学习", this);
    QAction* actSleep = new QAction("睡觉", this);
    QAction* actTire = new QAction("疲惫", this);
    QAction* actHide = new QAction("隐藏", this);
    QAction* actShow = new QAction("显示", this);

    connect(actHello, &QAction::triggered, this, &Widget::sayHello);
    connect(actFeed, &QAction::triggered, this, &Widget::feed);
    connect(actStay, &QAction::triggered, this, &Widget::setStateStay);
    connect(actHappy, &QAction::triggered, this, &Widget::setStateHappy);
    connect(actQuest, &QAction::triggered, this, &Widget::setStateQuesting);
    connect(actStudy, &QAction::triggered, this, &Widget::setStateStudying);
    connect(actSleep, &QAction::triggered, this, &Widget::setStateSleep);
    connect(actTire, &QAction::triggered, this, &Widget::setStateTiring);
    connect(actHide, &QAction::triggered, this, &Widget::hidePet);
    connect(actShow, &QAction::triggered, this, &Widget::showPet);

    m_rightMenu->addAction(actHello);
    m_rightMenu->addAction(actFeed);
    m_rightMenu->addSeparator();
    m_rightMenu->addAction(actStay);
    m_rightMenu->addAction(actHappy);
    m_rightMenu->addAction(actQuest);
    m_rightMenu->addAction(actStudy);
    m_rightMenu->addAction(actSleep);
    m_rightMenu->addAction(actTire);
    m_rightMenu->addSeparator();
    m_rightMenu->addAction(actHide);
    m_rightMenu->addAction(actShow);
}

void Widget::loadStateImage()
{
    QString path;
    switch (m_currentState) {
    case Stay: path = ":/stay.png"; break;
    case Happy: path = ":/happy.png"; break;
    case Questing: path = ":/questing.png"; break;
    case Studying: path = ":/studying.png"; break;
    case Sleep: path = ":/sleep.png"; break;
    case Tiring: path = ":/tiring.png"; break;
    default: path = ":/stay.png";
    }
    m_currentPixmap.load(path);
    m_currentPixmap = m_currentPixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    update();
}

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 清空为完全透明
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // 绘制图片
    painter.drawPixmap(0, 0, m_currentPixmap);

    // 绘制对话气泡
    if (m_isDialogVisible && !m_dialogText.isEmpty()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 230));
        painter.drawRoundedRect(10, 10, 130, 40, 8, 8);

        QFont font;
        font.setPointSize(8);
        painter.setFont(font);
        painter.setPen(QColor(50, 50, 50));
        painter.drawText(QRect(15, 15, 120, 30), Qt::AlignLeft | Qt::TextWordWrap, m_dialogText);
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    } else if (event->button() == Qt::RightButton) {
        m_rightMenu->exec(event->globalPosition().toPoint());
        event->accept();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPos);
        event->accept();
    }
}

void Widget::showDialog(const QString &text)
{
    m_dialogText = text;
    m_isDialogVisible = true;
    m_dialogTimer->start(3000);
    update();
}

void Widget::clearDialog()
{
    m_isDialogVisible = false;
    m_dialogText.clear();
    update();
}

void Widget::setStateStay() { m_currentState = Stay; loadStateImage(); }
void Widget::setStateHappy() { m_currentState = Happy; loadStateImage(); showDialog("我会陪伴在你身边，一起见证晴雨交替。"); }
void Widget::setStateQuesting() { m_currentState = Questing; loadStateImage(); showDialog("熊的天，这个问题有点难哦"); }
void Widget::setStateStudying() { m_currentState = Studying; loadStateImage(); showDialog("在思考的间隙，总是忍不住想你"); }
void Widget::setStateSleep() { m_currentState = Sleep; loadStateImage(); showDialog("你的晚安吻，才是驱散暗云与梦魇的月光"); }
void Widget::setStateTiring() { m_currentState = Tiring; loadStateImage(); showDialog("只要你在我身边，就是我源源不断的能量源泉"); }

void Widget::sayHello()
{
    int idx = rand() % helloTexts.size();
    showDialog(helloTexts[idx]);
}

void Widget::feed()
{
    setStateHappy();
    int idx = rand() % feedTexts.size();
    showDialog(feedTexts[idx]);
}

void Widget::hidePet() { hide(); }
void Widget::showPet() { show(); }