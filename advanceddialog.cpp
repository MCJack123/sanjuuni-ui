#include "advanceddialog.h"
#include "ui_advanceddialog.h"
#include "palettechooser.h"

AdvancedDialog::AdvancedDialog(Settings& settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvancedDialog),
    origsettings(settings),
    settings(settings) {
    ui->setupUi(this);
    ui->compressionType->setCurrentIndex(settings.compressionType);
    ui->separateStreams->setCheckState(settings.separateStreams ? Qt::Checked : Qt::Unchecked);
    ui->dfpwm->setCheckState(settings.compressDFPWM ? Qt::Checked : Qt::Unchecked);
    ui->binary->setCheckState(settings.binary ? Qt::Checked : Qt::Unchecked);
    ui->trimBorders->setCheckState(settings.trimBorders ? Qt::Checked : Qt::Unchecked);
    ui->streamed->setCheckState(settings.streamed ? Qt::Checked : Qt::Unchecked);
    ui->disableOpenCL->setCheckState(settings.disableOpenCL ? Qt::Checked : Qt::Unchecked);
    ui->subtitle->setText(settings.subtitle);
    ui->monitorWidth->setValue(settings.monitorWidth);
    ui->monitorHeight->setValue(settings.monitorHeight);
    ui->monitorScale->setValue(settings.monitorScale);
    ui->forcePalette->setCheckState(settings.forcePalette ? Qt::Checked : Qt::Unchecked);
    ui->paletteEdit->setEnabled(settings.forcePalette);
}

AdvancedDialog::~AdvancedDialog() {
    delete ui;
}

void AdvancedDialog::on_compressionType_currentIndexChanged(int arg1) {
    settings.compressionType = arg1;
    if (arg1 == 4) {
        // ANS not supported in separate streams
        ui->separateStreams->setEnabled(false);
        ui->separateStreams->setCheckState(Qt::Unchecked);
    } else {
        ui->separateStreams->setEnabled(true);
    }
}

void AdvancedDialog::on_separateStreams_stateChanged(int arg1) {
    settings.separateStreams = arg1 == Qt::Checked;
}

void AdvancedDialog::on_dfpwm_stateChanged(int arg1) {
    settings.compressDFPWM = arg1 == Qt::Checked;
}

void AdvancedDialog::on_binary_stateChanged(int arg1) {
    settings.binary = arg1 == Qt::Checked;
}

void AdvancedDialog::on_trimBorders_stateChanged(int arg1) {
    settings.trimBorders = arg1 == Qt::Checked;
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

void AdvancedDialog::on_monitorWidth_valueChanged(int arg1) {
    settings.monitorWidth = arg1;
}

void AdvancedDialog::on_monitorHeight_valueChanged(int arg1) {
    settings.monitorHeight = arg1;
}

void AdvancedDialog::on_monitorScale_valueChanged(double arg1) {
    settings.monitorScale = arg1;
}

void AdvancedDialog::on_forcePalette_stateChanged(int arg1) {
    settings.forcePalette = arg1 == Qt::Checked;
    ui->paletteEdit->setEnabled(arg1 == Qt::Checked);
}

void AdvancedDialog::on_paletteEdit_clicked() {
    PaletteChooser chooser(settings.palette, this);
    chooser.exec();
}
