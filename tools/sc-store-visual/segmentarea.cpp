#include "segmentarea.h"

SegmentArea::SegmentArea(QWidget *parent) :
    QWidget(parent),
    mSegment(-1),
    mSegmentElements(0),
    dw(12),
    dh(12) {
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

  setFixedSize(SEGMENT_ITEMS_IN_ROW * (dw + SEGMENT_ITEMS_OFFSET) + SEGMENT_GRID_OFFSET_PXX,
               (SEGMENT_SIZE / SEGMENT_ITEMS_IN_ROW + 1) * (dh + SEGMENT_ITEMS_OFFSET) + SEGMENT_GRID_OFFSET_PXY * 2);

  repaint();
}

void SegmentArea::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  for (quint32 i = 0; i < SEGMENT_SIZE; ++i) {
    quint32 x = i % SEGMENT_ITEMS_IN_ROW * (dw + SEGMENT_ITEMS_OFFSET) + SEGMENT_GRID_OFFSET_PXX;
    quint32 y = i / SEGMENT_ITEMS_IN_ROW * (dh + SEGMENT_ITEMS_OFFSET) + SEGMENT_GRID_OFFSET_PXY;
    QRect rect(x, y, dw, dh);
    QBrush brush;

    sc_element_ex *e = mSegmentElements.at(i);

    if (e == nullptr || e->element == nullptr) brush = QBrush(Qt::lightGray);
    else if (e->element->delete_time_stamp > 0) brush = QBrush(Qt::red);
    else brush = QBrush(Qt::darkBlue);

    painter.setPen(QPen(brush, 1.f));
    painter.drawRect(rect);

//    sc_element elx;
//    elx.delete_time_stamp;

    if (e != nullptr && e->element != nullptr) {
      sc_type type = e->element->type;

      QColor brushColor(255, 255, 255);

      if (type & sc_type_link)         brushColor = QColor(85, 97, 181);
      if (type & sc_type_node)         brushColor = QColor(156, 172, 165);
      if (type & sc_type_edge_common)  brushColor = QColor(255, 216, 77);
      if (type & sc_type_arc_common)   brushColor = QColor(232, 155, 3);
      if (type & sc_type_arc_access)   brushColor = QColor(210, 130, 30);

      painter.fillRect(rect, QBrush(brushColor));
    }
  }

  painter.setFont(QFont("Ubuntu Mono", 11));
  painter.setPen(QPen(QBrush(Qt::darkBlue), 1.f));
  for (quint32 i = 0; i <= SEGMENT_SIZE / SEGMENT_ITEMS_IN_ROW; i++) {
    painter.drawText(QRect(10,
                           (dh + SEGMENT_ITEMS_OFFSET) * i + SEGMENT_GRID_OFFSET_PXY - 2,
                           SEGMENT_GRID_OFFSET_PXX - 15,
                           dh + SEGMENT_ITEMS_OFFSET),
                     Qt::AlignRight,
                     QString("%1").arg(i * SEGMENT_ITEMS_IN_ROW));
  }
}
