#include "advanceddialog.h"
#include "ui_advanceddialog.h"

AdvancedDialog::AdvancedDialog(Settings& settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvancedDialog),
    origsettings(settings),
    settings(settings) {
    ui->setupUi(this);
    ui->compress->setCheckState(settings.compress ? Qt::Checked : Qt::Unchecked);
    ui->deflate->setCheckState(settings.compressDeflate ? Qt::Checked : Qt::Unchecked);
    ui->dfpwm->setCheckState(settings.compressDFPWM ? Qt::Checked : Qt::Unchecked);
    ui->binary->setCheckState(settings.binary ? Qt::Checked : Qt::Unchecked);
    ui->streamed->setCheckState(settings.streamed ? Qt::Checked : Qt::Unchecked);
    ui->disableOpenCL->setCheckState(settings.disableOpenCL ? Qt::Checked : Qt::Unchecked);
    ui->subtitle->setText(settings.subtitle);
    ui->deflate->setEnabled(settings.compress);
    ui->dfpwm->setEnabled(settings.compress);
}

AdvancedDialog::~AdvancedDialog() {
    delete ui;
}

void AdvancedDialog::on_compress_stateChanged(int arg1) {
    settings.compress = arg1 == Qt::Checked;
    ui->deflate->setEnabled(settings.compress);
    ui->dfpwm->setEnabled(settings.compress);
}

void AdvancedDialog::on_deflate_stateChanged(int arg1) {
    settings.compressDeflate = arg1 == Qt::Checked;
}

void AdvancedDialog::on_dfpwm_stateChanged(int arg1) {
    settings.compressDFPWM = arg1 == Qt::Checked;
}

void AdvancedDialog::on_binary_stateChanged(int arg1) {
    settings.binary = arg1 == Qt::Checked;
}

void AdvancedDialog::on_streamed_stateChanged(int arg1) {
    settings.streamed = arg1 == Qt::Checked;
}

void AdvancedDialog::on_disableOpenCL_stateChanged(int arg1) {
    settings.disableOpenCL = arg1 == Qt::Checked;
}

void AdvancedDialog::on_subtitle_textChanged(const QString& str) {
    settings.subtitle = str;
}

void AdvancedDialog::on_buttonBox_accepted() {
    origsettings = settings;
    close();
}

void AdvancedDialog::on_buttonBox_rejected() {
    close();
}
