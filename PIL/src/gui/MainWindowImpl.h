#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
#include <QMainWindow>
#include <queue>

class MainWindowImpl : public QMainWindow
{
    Q_OBJECT
public:
    MainWindowImpl(QWidget *parent = 0);

    virtual ~MainWindowImpl(){}

//    virtual int setupLayout(void);

    void    call(std::string cmd);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void timerEvent(QTimerEvent *event);

signals:
    void call_signal();

protected slots:
    void call_slot();

public slots:
    void action_SvarEdit(void);

private:

    std::queue<std::string>       cmds;
};

#endif // MAINWINDOWIMPL_H
