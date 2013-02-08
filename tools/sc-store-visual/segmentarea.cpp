#include "segmentarea.h"

SegmentArea::SegmentArea(QTreeWidget *tree, QWidget *parent) :
    QWidget(parent),
    mSegment(-1),
    mSegmentElements(0),
    mSegmentInfo(tree),
    dw(12),
    dh(12),
    mCurrentItem(-1) {
}

void SegmentArea::setSegment(quint32 segment) {
  if (mSegment == segment) return;
  mSegment = segment;
  mCurrentItem = -1;

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

QRect SegmentArea::buildRect(quint32 i, quint32 extra = 0) {
  quint32 x = i % SEGMENT_ITEMS_IN_ROW * (dw + SEGMENT_ITEMS_OFFSET) + SEGMENT_GRID_OFFSET_PXX;
  quint32 y = i / SEGMENT_ITEMS_IN_ROW * (dh + SEGMENT_ITEMS_OFFSET) + SEGMENT_GRID_OFFSET_PXY;
  return QRect(x, y, dw + extra, dh + extra);
};

void SegmentArea::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  for (quint32 i = 0; i < SEGMENT_SIZE; ++i) {
    QRect rect = buildRect(i);
    QBrush brush;

    sc_element_ex *e = mSegmentElements.at(i);

    if (e == nullptr || e->element == nullptr) brush = QBrush(Qt::lightGray);
    else if (e->element->delete_time_stamp > 0) brush = QBrush(Qt::red);
    else brush = QBrush(Qt::darkBlue);

    painter.setPen(QPen(brush, i == mCurrentItem ? 5.f : 1.f));
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

quint32 SegmentArea::getCount(sc_type type) {
  quint32 count = 0;
  sc_element_ex *el = 0;

  foreach (el, mSegmentElements) {
    if (el != nullptr && el->element != nullptr && (el->element->type & type)) count++;
  }

  return count;
}

static QTreeWidgetItem* einfoRow(QTreeWidget *tree, QString k, QString v) {
  QTreeWidgetItem *item = new QTreeWidgetItem(tree);

  item->setText(0, k);
  item->setText(1, v);

  return item;
}

static void itemSelected(sc_element_ex *item, QTreeWidget *info) {
  sc_addr addr = item->address;
  sc_element *el = item->element;

  QList<QTreeWidgetItem *> items;

  QTreeWidgetItem *twi = einfoRow(info, "Address", QString("%1 :: %2").arg(addr.seg).arg(addr.offset));
  items.append(twi);

  sc_type type = el->type;
  QString mltype("[%1] Unknown");

  if (type & sc_type_link)         mltype = QString("[%1] Link");
  if (type & sc_type_node)         mltype = QString("[%1] Node");
  if (type & sc_type_edge_common)  mltype = QString("[%1] Edge common");
  if (type & sc_type_arc_common)   mltype = QString("[%1] Arc common");
  if (type & sc_type_arc_access)   mltype = QString("[%1] Arc access");

  twi = einfoRow(info, "Type", mltype.arg(type));
  items.append(twi);

  if (type & sc_type_link) {
    if (&(el->content) != nullptr) {
      twi = einfoRow(info, "Content", QString(el->content.data));
      items.append(twi);
    }
  }

  if (type & (sc_type_edge_common | sc_type_arc_common)) {
    twi = einfoRow(info, "From", QString("%1 :: %2").arg(el->arc.begin.seg).arg(el->arc.begin.offset));
    twi->setData(1, Qt::UserRole, QVariant::fromValue((void *) &(el->arc.begin)));
    items.append(twi);

    twi = einfoRow(info, "To", QString("%1 :: %2").arg(el->arc.end.seg).arg(el->arc.end.offset));
    twi->setData(1, Qt::UserRole, QVariant::fromValue((void *) &(el->arc.end)));
    items.append(twi);
  }

  while (info->takeTopLevelItem(0));
  info->insertTopLevelItems(0, items);
}

void SegmentArea::activateItem(sc_element_ex *csi) {
  if (csi != nullptr) {
    itemSelected(csi, mSegmentInfo);
    mCurrentItem = csi->address.offset;
    repaint();
  }
}

void SegmentArea::mousePressEvent(QMouseEvent *e) {
  quint32 x0 = (e->x() - SEGMENT_GRID_OFFSET_PXX) / (dw + SEGMENT_ITEMS_OFFSET);
  quint32 y0 = (e->y() - SEGMENT_GRID_OFFSET_PXY) / (dh + SEGMENT_ITEMS_OFFSET);

  quint32 idx = x0 + y0 * SEGMENT_ITEMS_IN_ROW;

  QRect rect = buildRect(idx, 4);
  if (!rect.contains(e->x(), e->y(), true)) return;

  sc_element_ex *el = mSegmentElements.at(idx);
  if (el == nullptr || el->element == nullptr) return;

  activateItem(el);
}

void SegmentArea::infoItemClicked(QTreeWidgetItem *item, int row) {
  QVariant data = item->data(1, Qt::UserRole);
  if (data.isNull()) return;

  sc_addr *link = (sc_addr *) data.value<void *>();
  if (link == nullptr) return;

  setSegment(link->seg);
  activateItem(mSegmentElements.at(link->offset));

  QRect rect = buildRect(link->offset);

  QScrollArea *area = dynamic_cast<QScrollArea *>(parent());
  if (area != nullptr) area->scroll(rect.x(), rect.y());
}
