#ifndef SEGMENTITEM_H
#define SEGMENTITEM_H

#include "definition.h"

#include <QGraphicsRectItem>

class SegmentItem :
    public QObject,
    public QGraphicsRectItem {

  public:
    explicit SegmentItem();
    virtual ~SegmentItem();

    void element(sc_addr addr, sc_element *e);

    sc_element* getElement();
    sc_addr getAddr();

  private:
    void update();

    sc_addr addr;
    sc_element *el;

  protected:
  public slots:
    
};

#endif // SEGMENTITEM_H
