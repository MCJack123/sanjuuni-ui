#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sanjuuni/src/sanjuuni.hpp"

#include <chrono>

#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

extern int sanjuuni_main(int argc, const char * argv[]);

bool externalStop = false;
static std::thread* processThread = NULL;
static MainWindow* processWindow = NULL;
extern std::mutex exitLock;
extern std::condition_variable exitNotify;

enum class OutputTypeUI {
    Lua = 0,
    BIMG,
    Raw,
    Vid32,
    HTTP,
    WSServer,
    WSClient
};

static const char * typeExtensions[] = {
    "Lua file (*.lua)",
    "Blit image file (*.bimg)",
    "Raw mode frame (*.ccraw)",
    "32vid video (*.32v)"
};

static std::string typeArgs[] = {
    "--lua",
    "--blit-image",
    "--raw",
    "--32vid",
    "--http",
    "--websocket",
    "--websocket-client"
};

static const std::vector<Vec3b> defaultPalette = {
    {0xf0, 0xf0, 0xf0},
    {0x33, 0xb2, 0xf2},
    {0xd8, 0x7f, 0xe5},
    {0xf2, 0xb2, 0x99},
    {0x6c, 0xde, 0xde},
    {0x19, 0xcc, 0x7f},
    {0xcc, 0xb2, 0xf2},
    {0x4c, 0x4c, 0x4c},
    {0x99, 0x99, 0x99},
    {0xb2, 0x99, 0x4c},
    {0xe5, 0x66, 0xb2},
    {0xcc, 0x66, 0x33},
    {0x4c, 0x66, 0x7f},
    {0x4e, 0xa6, 0x57},
    {0x4c, 0x4c, 0xcc},
    {0x11, 0x11, 0x11}
};

static std::string avErrorString(int err) {
    char errstr[256];
    av_strerror(err, errstr, 256);
    return std::string(errstr);
}

template<class _Rep, class _Period>
std::string makeDurationString(const std::chrono::duration<_Rep, _Period>& duration) {
    char tmp[9];
    if (duration >= std::chrono::hours(1)) snprintf(tmp, 9, "%02d:%02d:%02d", std::chrono::duration_cast<std::chrono::hours>(duration).count(), std::chrono::duration_cast<std::chrono::minutes>(duration).count() % 60, std::chrono::duration_cast<std::chrono::seconds>(duration).count() % 60);
    else snprintf(tmp, 9, "%02d:%02d", std::chrono::duration_cast<std::chrono::minutes>(duration).count() % 60, std::chrono::duration_cast<std::chrono::seconds>(duration).count() % 60);
    return std::string(tmp);
}

void STATUS_FUNCTION(int nframe, int totalFrames, std::chrono::milliseconds elapsed, std::chrono::milliseconds remaining, int fps) {
    if (processWindow == NULL) return;
    QMetaObject::invokeMethod(processWindow, "updateStatus", Qt::AutoConnection, Q_ARG(int, nframe), Q_ARG(int, totalFrames), Q_ARG(long, elapsed.count()), Q_ARG(long, remaining.count()), Q_ARG(int, fps));
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), regenThread(std::bind(&MainWindow::regeneratePreview_thread, this)) {
    ui->setupUi(this);
    ui->progressGroup->hide();
}

MainWindow::~MainWindow() {
    runningRegen = false;
    regeneratePreview();
    regenThread.join();
    if (processThread != NULL) processThread->join();
    if (processWindow == this) processWindow = NULL;
    delete ui;
}

void MainWindow::regeneratePreview_thread() {
    while (runningRegen) {
        if (!shouldRegen) {
            QMetaObject::invokeMethod(this, "showLoadingPreview", Qt::AutoConnection, Q_ARG(bool, false));
            std::unique_lock<std::mutex> lock(regenThreadLock);
            regenThreadNotify.wait(lock);
        }
        if (!runningRegen) break;
        shouldRegen = false;
        if (inputPath.isEmpty()) continue;
        QMetaObject::invokeMethod(this, "showLoadingPreview", Qt::AutoConnection, Q_ARG(bool, true));
        QImage image = this->originalImage.toImage();
        Mat input(image.width(), image.height());
        for (int y = 0; y < image.height(); y++) {
            for (int x = 0; x < image.width(); x++) {
                QRgb rgb = image.pixel(x, y);
                input[y][x] = {qRed(rgb), qGreen(rgb), qBlue(rgb)};
            }
        }
        std::vector<Vec3b> palette;
        switch (ui->quality->value()) {
            case 0: palette = defaultPalette; break;
            case 1: palette = reducePalette_medianCut(input, 16); break;
            case 3: palette = reducePalette_octree(input, 16); break;
            case 2: palette = reducePalette_kMeans(input, 16); break;
        }
        Mat reduced;
        if (ui->dither->isChecked()) reduced = ditherImage(input, palette);
        else reduced = thresholdImage(input, palette);
        Mat1b palimg = rgbToPaletteImage(reduced, palette);
        uchar * chars = NULL, * cols = NULL;
        makeCCImage(palimg, palette, &chars, &cols);
        int width = palimg.width / 2, height = palimg.height / 3;
        QPixmap res(width * 6, height * 9);
        QPainter painter(&res);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                uchar c = chars[y*width+x], fg = cols[y*width+x] & 0x0F, bg = cols[y*width+x] >> 4;
                QColor fgc(palette[fg][0], palette[fg][1], palette[fg][2]), bgc(palette[bg][0], palette[bg][1], palette[bg][2]);
                painter.fillRect(x * 6,     y * 9,     3, 3, c & 1 ? fgc : bgc);
                painter.fillRect(x * 6 + 3, y * 9,     3, 3, c & 2 ? fgc : bgc);
                painter.fillRect(x * 6,     y * 9 + 3, 3, 3, c & 4 ? fgc : bgc);
                painter.fillRect(x * 6 + 3, y * 9 + 3, 3, 3, c & 8 ? fgc : bgc);
                painter.fillRect(x * 6,     y * 9 + 6, 3, 3, c & 16 ? fgc : bgc);
                painter.fillRect(x * 6 + 3, y * 9 + 6, 3, 3, bgc);
            }
        }
        painter.end();
        delete[] chars;
        delete[] cols;
        previewImage = res;
        QMetaObject::invokeMethod(this, "setPreviewPixmap", Qt::AutoConnection);
    }
}

void MainWindow::setPreviewPixmap() {
    ui->newImage->setPixmap(previewImage);
}

void MainWindow::regeneratePreview() {
    shouldRegen = true;
    std::unique_lock<std::mutex> lock(regenThreadLock);
    regenThreadNotify.notify_all();
}

void MainWindow::updateStatus(int nframe, int totalFrames, long elapsed, long remaining, int fps) {
    ui->frameNumber->setText(QString::number(nframe));
    ui->totalFrames->setText(QString::number(totalFrames));
    ui->timeRemaining->setText(QString::fromStdString(makeDurationString(std::chrono::milliseconds(remaining))));
    ui->fps->setText(QString::number(fps));
    if (totalFrames > 0) ui->progressBar->setValue((float)nframe / (float)totalFrames * 100);
}

void MainWindow::processComplete(int retval, std::exception *e) {
    if (processThread == NULL) return;
    if (processThread->joinable()) processThread->join();
    delete processThread;
    processThread = NULL;
    if (!externalStop) {
        QApplication::beep();
        if (e != NULL) {
            QMessageBox::critical(this, "Conversion failed", tr("The file failed to convert with an exception: ") + QString(e->what()) + ". Check the console for more information.");
            delete e;
        } else if (retval) QMessageBox::critical(this, "Conversion failed", tr("The file failed to convert with error code ") + QString::number(retval) + ". Check the console for more information.");
        else QMessageBox::information(this, "Conversion complete", "The file has successfully been converted.");
    }
    ui->progressGroup->hide();
    ui->startButton->setText("Start");
}

void MainWindow::showLoadingPreview(bool show) {
    if (show) ui->loadingPreview->show();
    else ui->loadingPreview->hide();
}

void MainWindow::on_openInputButton_clicked() {
    inputPath = QFileDialog::getOpenFileName(this, tr("Select Input File"), "", tr("Images (*.png *.jpg *.jpeg *.gif *.bmp *.webp *.heic);;Videos (*.mp4 *.m4v *.mov *.avi *.webm *.mkv *.flv);;All files (*)"));
    if (!inputPath.isEmpty()) {
        AVFormatContext * format_ctx = NULL;
        AVCodecContext * video_codec_ctx = NULL;
        const AVCodec * video_codec = NULL;
        int error, video_stream = -1;
        std::string errmsg;
        // Open video file
        do {
            if ((error = avformat_open_input(&format_ctx, inputPath.toStdString().c_str(), NULL, NULL)) < 0) {
                errmsg = "Could not open input file: " + avErrorString(error);
                break;
            }
            if ((error = avformat_find_stream_info(format_ctx, NULL)) < 0) {
                errmsg = "Could not read stream info: " + avErrorString(error);
                avformat_close_input(&format_ctx);
                break;
            }
            // Search for video (and audio?) stream indexes
            for (int i = 0; i < format_ctx->nb_streams && video_stream < 0; i++) {
                if (video_stream < 0 && format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) video_stream = i;
            }
            if (video_stream < 0) {
                errmsg = "Could not find any video streams";
                avformat_close_input(&format_ctx);
                break;
            }
            // Open the video decoder
            if (!(video_codec = avcodec_find_decoder(format_ctx->streams[video_stream]->codecpar->codec_id))) {
                errmsg = "Could not find video codec";
                avformat_close_input(&format_ctx);
                break;
            }
            if (!(video_codec_ctx = avcodec_alloc_context3(video_codec))) {
                errmsg = "Could not allocate video codec context";
                avformat_close_input(&format_ctx);
                break;
            }
            if ((error = avcodec_parameters_to_context(video_codec_ctx, format_ctx->streams[video_stream]->codecpar)) < 0) {
                errmsg = "Could not initialize video codec parameters: " + avErrorString(error);
                avcodec_free_context(&video_codec_ctx);
                avformat_close_input(&format_ctx);
                break;
            }
            if ((error = avcodec_open2(video_codec_ctx, video_codec, NULL)) < 0) {
                errmsg = "Could not open video codec: " + avErrorString(error);
                avcodec_free_context(&video_codec_ctx);
                avformat_close_input(&format_ctx);
                break;
            }
            AVPacket * packet = av_packet_alloc();
            AVFrame * frame = av_frame_alloc();
            while (av_read_frame(format_ctx, packet) >= 0) {
                if (packet->stream_index == video_stream) {
                    avcodec_send_packet(video_codec_ctx, packet);
                    if ((error = avcodec_receive_frame(video_codec_ctx, frame)) == 0) {
                        SwsContext * resize_ctx = sws_getContext(frame->width, frame->height, (AVPixelFormat)frame->format, frame->width, frame->height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);
                        QImage image(frame->width, frame->height + 1, QImage::Format::Format_RGBA8888);
                        int stride[4] = {image.bytesPerLine(), image.bytesPerLine(), image.bytesPerLine(), image.bytesPerLine()};
                        uint8_t * ptrs[4] = {image.bits(), image.bits() + 1, image.bits() + 2, image.bits() + 3};
                        sws_scale(resize_ctx, frame->data, frame->linesize, 0, frame->height, ptrs, stride);
                        originalImage = QPixmap::fromImage(image.copy(0, 0, frame->width, frame->height));
                        sws_freeContext(resize_ctx);
                        break;
                    }
                    if (error != AVERROR_EOF && error != AVERROR(EAGAIN)) {
                        errmsg = "Failed to grab video frame: " + avErrorString(error);
                        break;
                    }
                }
            }
            av_frame_free(&frame);
            av_packet_free(&packet);
            avcodec_free_context(&video_codec_ctx);
            avformat_close_input(&format_ctx);
        } while (false);
        if (!errmsg.empty()) {
            QMessageBox::critical(this, "Error opening file", QString::fromStdString("Could not open file: " + errmsg));
            inputPath = "";
        } else {
            ui->originalImage->setPixmap(originalImage);
            regeneratePreview();
        }
    }
    ui->startButton->setEnabled(!inputPath.isEmpty() && (!ui->outputPath->text().isEmpty() || ui->scriptType->currentIndex() == (int)OutputTypeUI::HTTP || ui->scriptType->currentIndex() == (int)OutputTypeUI::WSServer));
}

void MainWindow::on_scriptType_currentIndexChanged(int index) {
    OutputTypeUI tt = (OutputTypeUI)index;
    ui->browseButton->setEnabled(tt == OutputTypeUI::Lua || tt == OutputTypeUI::BIMG || tt == OutputTypeUI::Raw || tt == OutputTypeUI::Vid32);
    ui->port->setEnabled(tt == OutputTypeUI::HTTP || tt == OutputTypeUI::WSServer);
    ui->outputPath->setEnabled(tt != OutputTypeUI::HTTP && tt != OutputTypeUI::WSServer);
    ui->compress->setEnabled(tt == OutputTypeUI::Vid32);
    ui->dfpwm->setEnabled(tt == OutputTypeUI::Vid32);
    ui->startButton->setEnabled(!inputPath.isEmpty() && (!ui->outputPath->text().isEmpty() || ui->scriptType->currentIndex() == (int)OutputTypeUI::HTTP || ui->scriptType->currentIndex() == (int)OutputTypeUI::WSServer));
}

void MainWindow::on_browseButton_clicked() {
    QString str = QFileDialog::getSaveFileName(this, "Select output file", "", typeExtensions[ui->scriptType->currentIndex()]);
    if (!str.isEmpty()) ui->outputPath->setText(str);
    ui->startButton->setEnabled(!inputPath.isEmpty() && (!str.isEmpty() || ui->scriptType->currentIndex() == (int)OutputTypeUI::HTTP || ui->scriptType->currentIndex() == (int)OutputTypeUI::WSServer));
}

void MainWindow::on_startButton_clicked() {
    if (processThread != NULL) {
        externalStop = true;
        std::unique_lock<std::mutex> lock(exitLock);
        exitNotify.notify_all();
        return;
    }
    std::vector<std::string> arguments;
    arguments.push_back("sanjuuni");
    arguments.push_back("--input");
    arguments.push_back(inputPath.toStdString());
    OutputTypeUI tt = (OutputTypeUI)ui->scriptType->currentIndex();
    if (tt == OutputTypeUI::Lua || tt == OutputTypeUI::BIMG || tt == OutputTypeUI::Raw || tt == OutputTypeUI::Vid32) {
        arguments.push_back("--output");
        arguments.push_back(ui->outputPath->text().toStdString());
        arguments.push_back(typeArgs[ui->scriptType->currentIndex()]);
    } else if (tt == OutputTypeUI::HTTP || tt == OutputTypeUI::WSServer) {
        arguments.push_back(typeArgs[ui->scriptType->currentIndex()]);
        arguments.push_back(std::to_string(ui->port->value()));
    } else if (tt == OutputTypeUI::WSClient) {
        arguments.push_back(typeArgs[ui->scriptType->currentIndex()]);
        arguments.push_back(ui->outputPath->text().toStdString());
    }
    if (!ui->dither->isChecked()) arguments.push_back("--threshold");
    if (ui->compress->isChecked()) {
        arguments.push_back("--compression");
        arguments.push_back("custom");
    }
    if (ui->dfpwm->isChecked()) arguments.push_back("--dfpwm");
    if (ui->width->value() > 0) {
        arguments.push_back("--width");
        arguments.push_back(std::to_string(ui->width->value()));
    }
    if (ui->height->value() > 0) {
        arguments.push_back("--height");
        arguments.push_back(std::to_string(ui->height->value()));
    }
    switch (ui->quality->value()) {
        case 0: arguments.push_back("--default-palette"); break;
        case 3: arguments.push_back("--octree"); break;
        case 2: arguments.push_back("--kmeans"); break;
    }
    ui->progressBar->setValue(0);
    ui->frameNumber->setText("0");
    ui->totalFrames->setText("0");
    ui->timeRemaining->setText("00:00");
    processWindow = this;
    externalStop = false;
    processThread = new std::thread([this, arguments]() {
        const char ** argv = new const char*[arguments.size()];
        for (int i = 0; i < arguments.size(); i++) argv[i] = arguments[i].c_str();
        try {
            int retval = sanjuuni_main(arguments.size(), argv);
            QMetaObject::invokeMethod(this, "processComplete", Qt::AutoConnection, Q_ARG(int, retval), Q_ARG(std::exception*, NULL));
        } catch (std::exception &e) {
            QMetaObject::invokeMethod(this, "processComplete", Qt::AutoConnection, Q_ARG(int, 0), Q_ARG(std::exception*, new std::exception(e)));
        }
    });
    ui->startButton->setText("Stop");
    ui->progressGroup->show();
}

void MainWindow::on_dither_stateChanged(int arg1) {
    regeneratePreview();
}

void MainWindow::on_quality_sliderReleased() {
    regeneratePreview();
}

void MainWindow::on_outputPath_textChanged(const QString &arg1) {
    ui->startButton->setEnabled(!inputPath.isEmpty() && (!ui->outputPath->text().isEmpty() || ui->scriptType->currentIndex() == (int)OutputTypeUI::HTTP || ui->scriptType->currentIndex() == (int)OutputTypeUI::WSServer));
}

