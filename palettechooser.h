#ifndef PALETTECHOOSER_H
#define PALETTECHOOSER_H

#include <QDialog>
#include <QColorDialog>

namespace Ui {
    class PaletteChooser;
}

class PaletteChooser : public QDialog {
    Q_OBJECT

  public:
    explicit PaletteChooser(int32_t* palette, QWidget *parent = nullptr);
    ~PaletteChooser();

  protected slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_color0enabled_stateChanged(int arg1);
    void on_color1enabled_stateChanged(int arg1);
    void on_color2enabled_stateChanged(int arg1);
    void on_color3enabled_stateChanged(int arg1);
    void on_color4enabled_stateChanged(int arg1);
    void on_color5enabled_stateChanged(int arg1);
    void on_color6enabled_stateChanged(int arg1);
    void on_color7enabled_stateChanged(int arg1);
    void on_color8enabled_stateChanged(int arg1);
    void on_color9enabled_stateChanged(int arg1);
    void on_color10enabled_stateChanged(int arg1);
    void on_color11enabled_stateChanged(int arg1);
    void on_color12enabled_stateChanged(int arg1);
    void on_color13enabled_stateChanged(int arg1);
    void on_color14enabled_stateChanged(int arg1);
    void on_color15enabled_stateChanged(int arg1);
    void on_color0select_clicked();
    void on_color1select_clicked();
    void on_color2select_clicked();
    void on_color3select_clicked();
    void on_color4select_clicked();
    void on_color5select_clicked();
    void on_color6select_clicked();
    void on_color7select_clicked();
    void on_color8select_clicked();
    void on_color9select_clicked();
    void on_color10select_clicked();
    void on_color11select_clicked();
    void on_color12select_clicked();
    void on_color13select_clicked();
    void on_color14select_clicked();
    void on_color15select_clicked();

  private:
    Ui::PaletteChooser *ui;
    int32_t* origpalette;
    int32_t palette[16];
};

#endif // PALETTECHOOSER_H
