#include "segmentarea.h"

SegmentArea::SegmentArea(QWidget *parent) :
    QWidget(parent) {
}

void SegmentArea::setSegment(quint32 segment) {
  if (mSegment == segment) return;
  mSegment = segment;

  mSegmentElements.clear();

  for (quint32 i = 0; i < SEGMENT_SIZE; ++i) {
    sc_addr addr;
    addr.seg = mSegment;
    addr.offset = i;

    sc_element *el = sc_storage_get_element(addr, SC_TRUE);

    sc_element_ex *element = g_new0(sc_element_ex, 1);
    element->element = el;
    element->address = addr;

    mSegmentElements.push_back(element);
  }

  setFixedSize(SEGMENT_SIZE % SEGMENT_ITEMS_IN_ROW * (10 + SEGMENT_ITEMS_OFFSET) + 500,
               SEGMENT_SIZE / SEGMENT_ITEMS_IN_ROW * (10 + SEGMENT_ITEMS_OFFSET) + 500);
}

void SegmentArea::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::darkGreen);

//  sc_element_ex *e;
//  quint32 i = 0;
  for (quint32 i = 0; i < SEGMENT_SIZE; ++i) {
    quint32 x = i % SEGMENT_ITEMS_IN_ROW * (10 + SEGMENT_ITEMS_OFFSET);
    quint32 y = i / SEGMENT_ITEMS_IN_ROW * (10 + SEGMENT_ITEMS_OFFSET);

    painter.drawRect(x, y, 10, 10);
  }
}
