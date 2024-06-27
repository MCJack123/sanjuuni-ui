#ifndef ADVANCEDDIALOG_H
#define ADVANCEDDIALOG_H

#include <QDialog>

namespace Ui {
    class AdvancedDialog;
}

class AdvancedDialog : public QDialog {
    Q_OBJECT

public:
    struct Settings {
        int compressionType = 0;
        bool compressDFPWM = false;
        bool binary = false;
        bool streamed = false;
        bool disableOpenCL = false;
        bool trimBorders = true;
        bool separateStreams = false;
        bool forcePalette = false;
        QString subtitle = "";
        int monitorWidth = 8;
        int monitorHeight = 6;
        double monitorScale = 0.5;
        int32_t palette[16] = {-1};
    };

    explicit AdvancedDialog(Settings& settings, QWidget *parent = nullptr);
    ~AdvancedDialog();

protected slots:
    void on_compressionType_currentIndexChanged(int arg1);
    void on_separateStreams_stateChanged(int arg1);
    void on_dfpwm_stateChanged(int arg1);
    void on_binary_stateChanged(int arg1);
    void on_trimBorders_stateChanged(int arg1);
    void on_streamed_stateChanged(int arg1);
    void on_disableOpenCL_stateChanged(int arg1);
    void on_subtitle_textChanged(const QString& str);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_monitorWidth_valueChanged(int arg1);
    void on_monitorHeight_valueChanged(int arg1);
    void on_monitorScale_valueChanged(double arg1);
    void on_forcePalette_stateChanged(int arg1);

private:
    Ui::AdvancedDialog *ui;
    Settings& origsettings;
    Settings settings;
};

#endif // ADVANCEDDIALOG_H
