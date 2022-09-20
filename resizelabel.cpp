#include "resizelabel.h"

ResizeLabel::ResizeLabel(const QString &text, QWidget *parent, Qt::WindowFlags f): QLabel(text, parent, f) {
    this->setMinimumSize(1, 1);
    setScaledContents(false);
}

ResizeLabel::ResizeLabel(QWidget *parent, Qt::WindowFlags f): QLabel(parent, f) {
    this->setMinimumSize(1, 1);
    setScaledContents(false);
}

void ResizeLabel::setPixmap ( const QPixmap & p) {
    pix = p;
    QLabel::setPixmap(scaledPixmap());
}

int ResizeLabel::heightForWidth( int width ) const {
    return pix.isNull() ? this->height() : ((qreal)pix.height()*width)/pix.width();
}

QSize ResizeLabel::sizeHint() const {
    int w = this->width();
    return QSize( w, heightForWidth(w) );
}

QPixmap ResizeLabel::scaledPixmap() const {
    return pix.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void ResizeLabel::resizeEvent(QResizeEvent * e) {
    if (!pix.isNull()) QLabel::setPixmap(scaledPixmap());
}
