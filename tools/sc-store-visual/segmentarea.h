#ifndef SEGMENTAREA_H
#define SEGMENTAREA_H

#include "definition.h"

#include <QWidget>

class SegmentArea : public QWidget {
    Q_OBJECT
  public:
    explicit SegmentArea(QWidget *parent = 0);
    void setSegment(quint32 segment);

  protected:
    void paintEvent(QPaintEvent *);

  private:
    quint32 mSegment;
    QVector<sc_element_ex *> mSegmentElements;

  signals:
    
  public slots:
    
};

#endif // SEGMENTAREA_H
