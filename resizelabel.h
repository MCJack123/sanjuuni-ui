#ifndef RESIZELABEL_H
#define RESIZELABEL_H

#include <QLabel>
#include <QResizeEvent>

class ResizeLabel : public QLabel
{
    Q_OBJECT
public:
    ResizeLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ResizeLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
public:
    virtual int heightForWidth( int width ) const;
    virtual QSize sizeHint() const;
    QPixmap scaledPixmap() const;
public slots:
    void setPixmap ( const QPixmap & );
    void resizeEvent(QResizeEvent *);
private:
    QPixmap pix;
};

#endif // RESIZELABEL_H
