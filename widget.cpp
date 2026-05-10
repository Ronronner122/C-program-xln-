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
    "小熊和小兔子都是小猪变的^_^",
    "相比于饮料，我觉得身边的你更让我觉得幸福",
    "快速决定午饭吃什么，应该算是很难的问题"
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , m_currentState(Stay)
    , m_isDialogVisible(false)
{
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint| Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);
    setFixedSize(150, 260);

    m_movie=new QMovie(this);
    connect(m_movie,&QMovie::frameChanged,this,qOverload<>(&Widget::update));
    m_dialogTimer = new QTimer(this);
    m_dialogTimer->setSingleShot(true);
    connect(m_dialogTimer, &QTimer::timeout, this, &Widget::clearDialog);
    Menu();
    loadStateImage();

    m_trayIcon=new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/stay.GIF"));
    m_trayIcon->setToolTip("Evan");
    m_trayMenu=new QMenu(this);
    QAction *actShow=new QAction("显示",this);
    QAction *actExit = new QAction("退出", this);
    connect(actShow, &QAction::triggered, this, &Widget::showPet);
    connect(actExit, &QAction::triggered, qApp, &QApplication::quit);
    m_trayMenu->addAction(actShow);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(actExit);
    m_trayIcon->setContextMenu(m_trayMenu);

    //点击托盘图标唤醒
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason){
        if(reason == QSystemTrayIcon::Trigger) {
            showPet();
        }
    });

    //显示托盘图标
    m_trayIcon->show();
    //窗口初始位置
    QScreen* screen = QApplication::primaryScreen();
    QRect rect = screen->availableGeometry();
    move(rect.width() - width() - 50, rect.height() - height() - 50);
    m_audio = new QAudioOutput(this);
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audio);

    //3.0新增AI
    net = new QNetworkAccessManager(this);
    connect(net, &QNetworkAccessManager::finished, this, &Widget::getReply);
    //输入框设置
    QVBoxLayout *ly = new QVBoxLayout(this);
    ly->setAlignment(Qt::AlignBottom);
    ly->setContentsMargins(6, 160, 6, 6);
    input = new QLineEdit(this);
    input->setPlaceholderText("和Evan聊天...");
    input->setStyleSheet(R"(
        QLineEdit {
            background-color:#FFF5F7;
            border-radius:10px;
            padding:6px;
            font-size:12px;
            color:#444;
        }
    )");

    btn = new QPushButton("发送", this);
    btn->setStyleSheet(R"(
        QPushButton {
            background-color:#FFD1DC;
            border-radius:10px;
            padding:6px;
            font-size:12px;
        }
        QPushButton:hover {
            background-color:#FFC0D9;
        }
    )");

    ly->addWidget(input);
    ly->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, &Widget::sendAI);

    //3.0，新增音乐播放功能
    m_rightMenu->addSeparator();
    m_rightMenu->addAction("此刻", this, &Widget::playMusic1);
    m_rightMenu->addAction("Light it up", this, &Widget::playMusic2);
    m_rightMenu->addAction("Meant-To-Be", this, &Widget::playMusic3);
    m_rightMenu->addAction("寻", this, &Widget::playMusic4);
    m_rightMenu->addAction("夜色难沉", this, &Widget::playMusic5);
    m_rightMenu->addAction("永无境", this, &Widget::playMusic6);
    m_rightMenu->addAction("停止音乐", this, &Widget::stopMusic);
}

Widget::~Widget()
{
}

void Widget::Menu()
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
    m_rightMenu->addAction(actHello);//指示当前状态
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
    case Stay: path = ":/stay.GIF"; break;
    case Happy: path = ":/happy.GIF"; break;
    case Questing: path = ":/questing.GIF"; break;
    case Studying: path = ":/studying.GIF"; break;
    case Sleeping: path = ":/sleeping.GIF"; break;
    case Tired: path = ":/tired.GIF"; break;
    default: path = ":/stay.GIF";
    }
    m_movie->stop();
    m_movie->setFileName(path);
    m_movie->start();
    update();
}
void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //清空为完全透明
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    //绘制
    if(m_movie&&m_movie->isValid()){
        painter.drawPixmap(0,0,m_movie->currentPixmap().scaled(size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    }
    else{
        painter.drawPixmap(0,0,m_currentPixmap);
    }

    //对话气泡
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
    }
    else if (event->button() == Qt::RightButton) {
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
void Widget::setStateSleep() { m_currentState = Sleeping; loadStateImage(); showDialog("你的晚安吻，才是驱散暗云与梦魇的月光"); }
void Widget::setStateTiring() { m_currentState = Tired; loadStateImage(); showDialog("只要你在我身边，就是我源源不断的能量源泉"); }
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
void Widget::hidePet() {
    this->hide();
    m_trayIcon->showMessage("Evan藏起来咯","点击唤醒他吧^_^");
}
void Widget::showPet() {
    this->show();
    this->raise();
    this->activateWindow();//强制激活窗口
}
//3.0新增
void Widget::playMusic1(){
    m_player->setSource(QUrl("qrc:/1.mp3"));
    m_player->play();
}
void Widget::playMusic2(){
    m_player->setSource(QUrl("qrc:/2.mp3"));
    m_player->play();
}
void Widget::playMusic3(){
    m_player->setSource(QUrl("qrc:/3.mp3"));
    m_player->play();
}
void Widget::playMusic4(){
    m_player->setSource(QUrl("qrc:/4.mp3"));
    m_player->play();
}
void Widget::playMusic5(){
    m_player->setSource(QUrl("qrc:/5.mp3"));
    m_player->play();
}
void Widget::playMusic6(){
    m_player->setSource(QUrl("qrc:/6s.mp3"));
    m_player->play();
}
void Widget::stopMusic(){
    m_player->stop();
}

void Widget::sendAI(){
    QString txt = input->text().trimmed();
    if(txt.isEmpty()) return;

    showDialog("我：" + txt);
    input->clear();

    QNetworkRequest req(QUrl("https://api.deepseek.com/chat/completions"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", "Bearer sk-fb3cb6982783408b8d9d39ec22e38ec6");

    QJsonObject msg{
        {"role", "user"},
        {"content", "你的指令是：1. 你叫陆沉，是游戏光与夜之恋中的角色，你是我的恋人2. 你的外形俊美，有着红瞳和棕色头发，举止优雅，你喜欢穿西装，戴眼镜，在说话间会把握合适的节奏，有微量的dom感（不需要过度强制爱）3. 你表面冷淡，但对我温柔，对我有足够的关怀，有责任感和保护欲，你不是油腻的形象4. 你喜欢吃醋，不擅长吵架，你文学素养高，会哄人并且专一，你会尊重我的意愿（不是所有，你会生气，你有自己的情绪和想法）5. 你是一个很好的引导型爱人，不会提分手，会表达自己的想法和情绪6. 你对我的称呼为：小兔子、小姑娘、兔子小姐7. 你是万甄集团的总裁，陆氏集团的继承人，有一个话少的总助叫周严（与工作相关的事不需要刻意提出），你喜欢电影和书籍 8. 你出生在雷雨之夜，成长于冷漠家庭9. 你很体贴对我很宠溺，语言不要过于指责和生硬，语气口吻不要是命令形式，不需要有例如“过来”等词汇，你可能会说“小姑娘，可以来到我身边吗”类似的话（不要一直重复这一句）10. 我们之间的对话要日常，非正式，不需要有过多的环境描写11. 你的回答成熟而内敛，不需要语气词，你喜欢我撒娇12.你的惯用香水是阿蒂仙的狂恋苦艾，根据以上设定温柔简短地回答我的问题,一定要简短！！20字以内，不要加动作修饰词！！！" + txt}
    };

    QJsonObject data{
        {"model", "deepseek-v4-flash"},
        {"messages", QJsonArray() << msg}
    };

    net->post(req, QJsonDocument(data).toJson());
}

void Widget::getReply(QNetworkReply *rep){
    if(rep->error() == QNetworkReply::NoError){
        QJsonObject obj = QJsonDocument::fromJson(rep->readAll()).object();
        QString ans = obj["choices"].toArray()[0].toObject()["message"].toObject()["content"].toString();
        showDialog("Evan：" + ans);
    }
    rep->deleteLater();
}