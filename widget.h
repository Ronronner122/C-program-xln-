#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QMenu>
#include <QPixmap>
#include <QPoint>

class Widget : public QWidget
{
    Q_OBJECT

public:
    // 六种状态
    enum PetState {
        Stay,       // 待机
        Happy,      // 开心
        Questing,   // 探索
        Studying,   // 学习
        Sleep,      // 睡觉
        Tiring      // 疲惫
    };

    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    // 重写事件
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    // 状态切换
    void setStateStay();
    void setStateHappy();
    void setStateQuesting();
    void setStateStudying();
    void setStateSleep();
    void setStateTiring();

    // 互动功能
    void sayHello();    // 打招呼
    void feed();        // 喂食
    void hidePet();     // 隐藏桌宠
    void showPet();     // 显示桌宠
    void clearDialog(); // 清除对话气泡

private:
    // 初始化右键菜单
    void initMenu();
    // 加载当前状态图片
    void loadStateImage();
    // 显示对话语录
    void showDialog(const QString& text);


    // 变量
    PetState m_currentState;    // 当前状态
    QPixmap m_currentPixmap;    // 当前状态图片
    QPoint m_dragPos;           // 拖拽位置
    QMenu* m_rightMenu;         // 右键菜单

    QString m_dialogText;       // 对话内容
    QTimer* m_dialogTimer;      // 对话消失计时器
    bool m_isDialogVisible;     // 对话显示标记
};

#endif // WIDGET_H