#ifndef SEGMENTAREA_H
#define SEGMENTAREA_H

#include "definition.h"

#include <QWidget>
#include <QTreeWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QScrollArea>
#include <QScrollBar>

#define SEGMENT_ITEMS_IN_ROW     64
#define SEGMENT_ITEMS_OFFSET     4
#define SEGMENT_GRID_OFFSET_PXX  60
#define SEGMENT_GRID_OFFSET_PXY  5

class SegmentArea : public QWidget {
    Q_OBJECT
  public:
    explicit SegmentArea(QTreeWidget *tree, QWidget *parent = 0);
    void setSegment(quint32 segment);
    quint32 getCount(sc_type type);

  protected:
    void paintEvent(QPaintEvent *);

  private:
    quint32 mSegment;
    QVector<sc_element_ex *> mSegmentElements;
    QTreeWidget *mSegmentInfo;
    QScrollArea *mScrollArea;
    quint32 dw;
    quint32 dh;
    quint32 mCurrentItem;

    QRect buildRect(quint32 i, quint32 extra);
    void activateItem(sc_element_ex *csi);

  signals:
    
  public slots:
    void mousePressEvent(QMouseEvent *e);
    void infoItemClicked(QTreeWidgetItem *item, int row);
    
};

#endif // SEGMENTAREA_H
