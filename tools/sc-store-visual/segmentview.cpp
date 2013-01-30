#include "segmentview.h"

#include <QGraphicsTextItem>
#include <QFontMetrics>

SegmentView::SegmentView(QTreeWidget *tree, QWidget *parent) :
    QGraphicsView(parent),
    mSegmentId(-1),
    mCurrentItem(0) {

  mSegmentInfo = tree;

  setScene(new SegmentScene(this));
}

SegmentView::~SegmentView() {}

void SegmentView::updateSegmentItems() {
  if (mSegmentItems.size() == 0) {
    for (quint32 i = 0; i < SEGMENT_SIZE; ++i) {
      SegmentItem *item = new SegmentItem();

      item->setPos(i % SEGMENT_ITEMS_IN_ROW * (item->boundingRect().width() + SEGMENT_ITEMS_OFFSET),
                   i / SEGMENT_ITEMS_IN_ROW * (item->boundingRect().height() + SEGMENT_ITEMS_OFFSET));
      scene()->addItem(item);

      mSegmentItems.push_back(item);
    }

    // draw row numbers
    qreal dh = mSegmentItems.first()->boundingRect().height();
    qreal dw = mSegmentItems.first()->boundingRect().width();
    quint32 n = SEGMENT_SIZE / SEGMENT_ITEMS_IN_ROW;
    QString value;

    for (quint32 i = 0; i <= n; i++) {
      value = QString("%1").arg(i * SEGMENT_ITEMS_IN_ROW);
      QGraphicsTextItem *text = scene()->addText(value);
      QFontMetrics metrics(text->font());
      QRect rc = metrics.boundingRect(value);

      text->setPos(-SEGMENT_ITEMS_OFFSET - dw / 2.f - rc.width(),
                   (dh + SEGMENT_ITEMS_OFFSET) * i - rc.height() / 2.f + 1);
    }
  }

  SegmentItem *item = 0;
  quint32 i = 0;
  foreach (item, mSegmentItems) {
    sc_addr addr;
    addr.seg = mSegmentId;
    addr.offset = i++;

    sc_element *el = sc_storage_get_element(addr, SC_TRUE);
    if (el != nullptr) item->element(addr, el);
  }
}

void SegmentView::setSegmentId(quint32 segId) {
  if (mSegmentId == segId) return;

  mSegmentId = segId;
  updateSegmentItems();
}

quint32 SegmentView::segmentId() const {
  return mSegmentId;
}

quint32 SegmentView::getCount(sc_type type) {
  quint32 count = 0;
  SegmentItem *item = 0;

  foreach (item, mSegmentItems) {
    sc_element *el = item->getElement();
    if (el != nullptr && (el->type & type)) count++;
  }

  return count;
}

void SegmentView::reset() {}

static QTreeWidgetItem* einfoRow(QTreeWidget *tree, QString k, QString v) {
  QTreeWidgetItem *item = new QTreeWidgetItem(tree);

  item->setText(0, k);
  item->setText(1, v);

  return item;
}

static void itemSelected(SegmentItem *item, QTreeWidget *info) {
  sc_addr addr = item->getAddr();
  sc_element *el = item->getElement();

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

static void setRectWidth(QGraphicsRectItem *item, qreal w) {
  if (item == nullptr) return;

  QPen pen = item->pen();
  pen.setWidth(w);
  item->setPen(pen);
}

void SegmentView::activateItem(SegmentItem *csi) {
  if (csi != nullptr) {
    itemSelected(csi, mSegmentInfo);

    setRectWidth(mCurrentItem, 1.f);
    mCurrentItem = csi;
    setRectWidth(mCurrentItem, 2.f);
  }
}

void SegmentView::mousePressEvent(QMouseEvent *e) {
  QGraphicsView::mousePressEvent(e); //Call the ancertor

  activateItem(qgraphicsitem_cast<SegmentItem *>(itemAt(e->pos())));
}

void SegmentView::infoItemClicked(QTreeWidgetItem *item, int row) {
  QVariant data = item->data(1, Qt::UserRole);
  if (data.isNull()) return;

  sc_addr *link = (sc_addr *) data.value<void *>();
  if (link == nullptr) return;

  setSegmentId(link->seg);
  activateItem(mSegmentItems.at(link->offset));
}




//#include "segmentarea.h"

//SegmentArea::SegmentArea(QWidget *parent) :
//    QWidget(parent) {
//}

//void SegmentArea::setSegment(quint32 segment) {
//  if (mSegment == segment) return;
//  mSegment = segment;

//  mSegmentElements.clear();

//  for (quint32 i = 0; i < SEGMENT_SIZE; ++i) {
//    sc_addr addr;
//    addr.seg = mSegment;
//    addr.offset = i;

//    sc_element *el = sc_storage_get_element(addr, SC_TRUE);

//    sc_element_ex *element = g_new0(sc_element_ex, 1);
//    element->element = el;
//    element->address = addr;

//    mSegmentElements.push_back(element);
//  }
//}

//void SegmentArea::paintEvent(QPaintEvent *) {
//  QPainter painter(this);
//  painter.setRenderHint(QPainter::Antialiasing);
//  painter.setPen(Qt::darkGreen);
////  painter.drawRect(1, 2, 100, 200);

////  sc_element_ex *e;
////  quint32 i = 0;
//  for (quint32 i = 0; i < SEGMENT_SIZE; ++i) {
//    quint32 x = i % SEGMENT_ITEMS_IN_ROW * (10 + SEGMENT_ITEMS_OFFSET);
//    quint32 y = i / SEGMENT_ITEMS_IN_ROW * (10 + SEGMENT_ITEMS_OFFSET);

//    painter.drawRect(x, y, 10, 10);
//  }
//}
