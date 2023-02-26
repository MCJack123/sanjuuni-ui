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
        bool compress = false;
        bool compressDeflate = false;
        bool compressDFPWM = false;
        bool binary = false;
        bool streamed = false;
        bool disableOpenCL = false;
        QString subtitle = "";
    };

    explicit AdvancedDialog(Settings& settings, QWidget *parent = nullptr);
    ~AdvancedDialog();

protected slots:
    void on_compress_stateChanged(int arg1);
    void on_deflate_stateChanged(int arg1);
    void on_dfpwm_stateChanged(int arg1);
    void on_binary_stateChanged(int arg1);
    void on_streamed_stateChanged(int arg1);
    void on_disableOpenCL_stateChanged(int arg1);
    void on_subtitle_textChanged(const QString& str);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::AdvancedDialog *ui;
    Settings& origsettings;
    Settings settings;
};

#endif // ADVANCEDDIALOG_H
