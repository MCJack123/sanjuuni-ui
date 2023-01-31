#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void setPreviewPixmap();
    void updateStatus(int nframe, int totalFrames, long elapsed, long remaining, int fps);
    void processComplete(int retval, std::exception *e);
    void showLoadingPreview(bool show);
    void on_openInputButton_clicked();
    void on_scriptType_currentIndexChanged(int index);
    void on_browseButton_clicked();
    void on_startButton_clicked();
    void on_dither_stateChanged(int arg1);
    void on_quality_sliderReleased();
    void on_outputPath_textChanged(const QString &arg1);

private:
    Ui::MainWindow* ui;
    QString inputPath;
    QPixmap originalImage;
    QPixmap previewImage;
    bool shouldRegen = false;
    bool runningRegen = true;
    std::thread regenThread;
    std::mutex regenThreadLock;
    std::condition_variable regenThreadNotify;
    void regeneratePreview_thread();
    void regeneratePreview();
};
#endif // MAINWINDOW_H
