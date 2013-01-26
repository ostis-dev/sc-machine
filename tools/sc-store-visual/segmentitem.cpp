#include "segmentitem.h"

#include <QBrush>
#include <QPen>

SegmentItem::SegmentItem() :
    QGraphicsRectItem() {
  setRect(0, 0, 10, 10);
}

SegmentItem::~SegmentItem() {}

void SegmentItem::update() {
  sc_type type = el->type;

  // update brush color
  QColor brushColor(255, 255, 255);

  if (type & sc_type_link)         brushColor = QColor(85, 97, 181);
  if (type & sc_type_node)         brushColor = QColor(156, 172, 165);
  if (type & sc_type_edge_common)  brushColor = QColor(255, 216, 77);
  if (type & sc_type_arc_common)   brushColor = QColor(232, 155, 3);
  if (type & sc_type_arc_access)   brushColor = QColor(210, 130, 30);

  setBrush(QBrush(brushColor));
  setOpacity(0.6f);

  QBrush brush(Qt::black);
  if (el->delete_time_stamp > 0) brush.setColor(Qt::red);

  setPen(QPen(brush, 1.f));
}

void SegmentItem::element(sc_addr a, sc_element *e) {
  el = e;
  addr = a;
  update();
}

sc_addr SegmentItem::getAddr() {
  return addr;
}

sc_element* SegmentItem::getElement() {
  return el;
}
