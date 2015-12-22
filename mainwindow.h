#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "common.h"
#include "sequence.h"
#include "globalscreen.h"
#include "facescreen.h"

namespace Ui {
class MainWindow;
}

const QEvent::Type
    OpenSequence = QEvent::Type(QEvent::User + 1),
    SaveSequence = QEvent::Type(QEvent::User + 2),
    PlayVideo = QEvent::Type(QEvent::User + 3),
    StopVideo = QEvent::Type(QEvent::User + 4);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    enum State { Empty, Normal, Play };

    void handleKeyEvent(QKeyEvent *e);
    void handlePaintEvent(QPaintEvent *e);
    void handleSequenceEvent(QEvent *e);
    void handleVideoEvent(QEvent *e);

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_actionExit_triggered();
    void on_playButton_clicked();
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_slider_valueChanged(int value);

    void frame_come();
    void refresh();

private:
    Ui::MainWindow *ui;
    GlobalScreen *gs;
    FaceScreen *fs;
    Sequence *seq;
    QString image_folder;

    QAction* separator[2];
    QVector<QAction*> recent;
    QString recent_path;

    State state;
    QTimer *timer_frame;
};

#endif // MAINWINDOW_H
