#include "palettechooser.h"
#include "ui_palettechooser.h"

static void setButtonColor(QPushButton *button, int32_t color) {
    button->setEnabled(color >= 0);
    if (color <= 0) color = 0;
    char buf[64];
    snprintf(buf, 64, "border: 1px solid black; background-color: #%06X;", color);
    button->setStyleSheet(QString(buf));
}

PaletteChooser::PaletteChooser(int32_t *palette, QWidget *parent) : QDialog(parent), ui(new Ui::PaletteChooser), origpalette(palette) {
    ui->setupUi(this);
    memcpy(this->palette, palette, sizeof(this->palette));
    ui->color0enabled->setCheckState(palette[0] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color1enabled->setCheckState(palette[1] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color2enabled->setCheckState(palette[2] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color3enabled->setCheckState(palette[3] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color4enabled->setCheckState(palette[4] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color5enabled->setCheckState(palette[5] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color6enabled->setCheckState(palette[6] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color7enabled->setCheckState(palette[7] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color8enabled->setCheckState(palette[8] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color9enabled->setCheckState(palette[9] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color10enabled->setCheckState(palette[10] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color11enabled->setCheckState(palette[11] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color12enabled->setCheckState(palette[12] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color13enabled->setCheckState(palette[13] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color14enabled->setCheckState(palette[14] >= 0 ? Qt::Checked : Qt::Unchecked);
    ui->color15enabled->setCheckState(palette[15] >= 0 ? Qt::Checked : Qt::Unchecked);
    setButtonColor(ui->color0select, palette[0]);
    setButtonColor(ui->color1select, palette[1]);
    setButtonColor(ui->color2select, palette[2]);
    setButtonColor(ui->color3select, palette[3]);
    setButtonColor(ui->color4select, palette[4]);
    setButtonColor(ui->color5select, palette[5]);
    setButtonColor(ui->color6select, palette[6]);
    setButtonColor(ui->color7select, palette[7]);
    setButtonColor(ui->color8select, palette[8]);
    setButtonColor(ui->color9select, palette[9]);
    setButtonColor(ui->color10select, palette[10]);
    setButtonColor(ui->color11select, palette[11]);
    setButtonColor(ui->color12select, palette[12]);
    setButtonColor(ui->color13select, palette[13]);
    setButtonColor(ui->color14select, palette[14]);
    setButtonColor(ui->color15select, palette[15]);
}

PaletteChooser::~PaletteChooser() {
    delete ui;
}

void PaletteChooser::on_buttonBox_accepted() {
    memcpy(origpalette, palette, sizeof(palette));
    close();
}

void PaletteChooser::on_buttonBox_rejected() {
    close();
}

#define COLOR_HANDLER(n) \
void PaletteChooser::on_color##n##enabled_stateChanged(int arg1) {\
    ui->color##n##select->setEnabled(arg1 == Qt::Checked);\
    if (arg1 == Qt::Unchecked) palette[n] = -1;\
    else if (palette[n] < 0) palette[n] = 0;\
}\
\
void PaletteChooser::on_color##n##select_clicked() {\
    QColor color = QColorDialog::getColor(QColor(palette[n]), this, "Select Color");\
    palette[n] = color.rgb() & 0xFFFFFF;\
    setButtonColor(ui->color##n##select, palette[n]);\
}

COLOR_HANDLER(0)
COLOR_HANDLER(1)
COLOR_HANDLER(2)
COLOR_HANDLER(3)
COLOR_HANDLER(4)
COLOR_HANDLER(5)
COLOR_HANDLER(6)
COLOR_HANDLER(7)
COLOR_HANDLER(8)
COLOR_HANDLER(9)
COLOR_HANDLER(10)
COLOR_HANDLER(11)
COLOR_HANDLER(12)
COLOR_HANDLER(13)
COLOR_HANDLER(14)
COLOR_HANDLER(15)
