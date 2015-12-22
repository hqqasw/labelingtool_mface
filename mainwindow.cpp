#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gs(NULL), fs(NULL),
    seq(NULL),
    state(Empty),
    timer_frame(new QTimer())
{
    ui->setupUi(this);
    setWindowTitle(Title);
    ui->slider->setEnabled(false);

    separator[0] = ui->menu_File->insertSeparator(ui->actionExit);
    separator[1] = ui->menu_File->insertSeparator(ui->actionExit);

    connect(timer_frame, SIGNAL(timeout()), this, SLOT(frame_come()));
    installEventFilter(this);
}

MainWindow::~MainWindow()
{
    //lazy
}

void MainWindow::handleKeyEvent(QKeyEvent *e)
{
    if (state == Empty)
        return;
    int delta = 0;
    switch (e->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        delta = -ui->slider->pageStep();
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        delta = -ui->slider->singleStep();
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        delta = ui->slider->pageStep();
        break;
    case Qt::Key_Space:
        if (state == Play) {
            QEvent e(StopVideo);
            handleVideoEvent(&e);
        }else
            delta = ui->slider->singleStep();
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        delta = ui->slider->singleStep();
        break;
    }
    if (delta != 0) {
        int value = ui->slider->value() + delta;
        ui->slider->setValue(value);
    }
}

void MainWindow::handlePaintEvent(QPaintEvent *e)
{
    QMainWindow::paintEvent(e);
    if (state != Empty) {
        gs->update();
        fs->update();
    }
}

void MainWindow::handleSequenceEvent(QEvent *e)
{
    if (state == Play)
    {
        QEvent e(StopVideo);
        handleVideoEvent(&e);
    }

    if (e->type() == OpenSequence)
    {
        QString path;
        path = QFileDialog::getOpenFileName(this, "Open", "", " sequence file.(*.avi *.mp4)");
        if (path.isEmpty())
            return;
        path = QFileInfo(path).absoluteFilePath();
        QString filename = QFileInfo(path).fileName();

        ui->statusBar->showMessage("Loading " + filename);

        QString res_path = path;
        res_path.remove(res_path.size()-4, 4);
        res_path.append("_res.txt");
        QFileInfo fi_res(res_path);

        Sequence* new_seq;
        if(fi_res.isFile())
            new_seq = new Sequence(path, res_path);
        else
            new_seq = new Sequence(path, NULL);
        if (seq != NULL)
            delete seq;
        QGridLayout *g = ui->screen;
        if (state == Empty) {
            g->removeWidget(ui->frame);
            delete ui->frame;
        }else {
            g->removeWidget(gs);
            g->removeWidget(fs);
            delete gs;
            delete fs;
        }

        seq = new_seq;
        seq->set_path(path);
        seq->set_name(filename);
        state = Normal;
        gs = new GlobalScreen(seq, this);
        fs = new FaceScreen(seq, this);

        qreal scale = qScale(seq->get_size(), QSize(360, 360));
        if (scale > 1)
            scale = 1;
        gs->setFixedSize(seq->get_size() * scale);
        fs->setFixedSize(360, 360);
        g->addWidget(fs, 0, 0, 1, 1, Qt::AlignCenter);
        g->addWidget(gs, 0, 1, 1, 1, Qt::AlignCenter);

        ui->slider->setEnabled(true);
        ui->slider->setMinimum(0);
        ui->slider->setMaximum(seq->get_frame_N() - 1);
        ui->slider->setSingleStep(1);
        ui->slider->setPageStep(qRound(seq->get_frame_rate()));
        QString text = QString::number(0) + "/" + QString::number(seq->get_frame_N() - 1);
        ui->progress->setText(text);
        ui->slider->setValue(0);

        QSettings settings(Author, Title);

        setWindowTitle(path + " - " + Title);
    }
    else if (e->type() == SaveSequence)
    {
        QString save_path = seq->get_path();
        save_path.remove(save_path.size()-4, 4);
        save_path.append("_res.txt");

        std::string fout_name = Qstr2stdstr(save_path);
        FILE *fp;
        fp = fopen(fout_name.c_str(),"w");
        if(fp == NULL) return;

        fprintf(fp, "%s\n", Qstr2stdstr(seq->get_name()).c_str());
        fprintf(fp, "%d\t%d\t%d\t%f\n", seq->get_frame_N(), seq->get_frame_width(), seq->get_frame_height(), seq->get_frame_rate());;
        for(int i = 0; i < seq->res.size(); i++)
        {
             fprintf(fp, "%d\n", seq->res[i].size());
            for(int j = 0; j < seq->res[i].size(); j++)
            {
                fprintf(fp, "%f\t%f\t%f\t%f\n", seq->res[i][j].rect.x , seq->res[i][j].rect.y, seq->res[i][j].rect.width, seq->res[i][j].rect.height);
                for(int k = 0; k < ALI_POINTS_NUM; k++)
                {
                    fprintf(fp, "%f\t%f\t",seq->res[i][j].alignments[k].x, seq->res[i][j].alignments[k].y);
                }
                fprintf(fp, "\n");
            }
        }
        fclose(fp);
    }
}

void MainWindow::handleVideoEvent(QEvent *e)
{
    if (state == Empty)
        return;
    if (state == Normal && e->type() == PlayVideo)
    {
        state = Play;
        timer_frame->start(qRound(1000 / seq->get_frame_rate()));
        ui->statusBar->showMessage("Start Play");
        gs->play();
        fs->play();
        ui->playButton->setText("Pause");
    }
    if (state == Play && e->type() == StopVideo)
    {
        state = Normal;
        timer_frame->stop();
        gs->stop();
        fs->stop();
        ui->playButton->setText("Play");
    }
    update();
}



void MainWindow::on_action_Open_triggered()
{
    QCoreApplication::postEvent(this, new QEvent(OpenSequence));
}

void MainWindow::on_action_Save_triggered()
{
    QCoreApplication::postEvent(this, new QEvent(SaveSequence));
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_playButton_clicked()
{
    if (state == Normal)
        QCoreApplication::postEvent(this, new QEvent(PlayVideo));
    else
        QCoreApplication::postEvent(this, new QEvent(StopVideo));
}

void MainWindow::on_addButton_clicked()
{

}

void MainWindow::on_deleteButton_clicked()
{

}

void MainWindow::on_slider_valueChanged(int value)
{
    int max = ui->slider->maximum();
    QString text = QString::number(value) + "/" + QString::number(max);
    ui->progress->setText(text);

    seq->set_n(value);

    update();
}


void MainWindow::frame_come()
{
    int value = ui->slider->value() + ui->slider->singleStep();
    if (value > ui->slider->maximum()) {
        QEvent e(StopVideo);
        handleVideoEvent(&e);
    }else
        ui->slider->setValue(value);
}

void MainWindow::refresh()
{
    update();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (obj != this)
        return false;

    switch (e->type())
    {
    case QEvent::KeyPress:
        if (static_cast<QKeyEvent*>(e)->modifiers() == Qt::NoModifier)
        {
            handleKeyEvent(static_cast<QKeyEvent*>(e));
            return true;
        }
        break;
    case QEvent::Paint:
        handlePaintEvent(static_cast<QPaintEvent*>(e));
        return true;
        break;
    case OpenSequence:
    case SaveSequence:
        handleSequenceEvent(e);
        return true;
        break;
    case PlayVideo:
    case StopVideo:
        handleVideoEvent(e);
        return true;
        break;
    }

    return false;
}
