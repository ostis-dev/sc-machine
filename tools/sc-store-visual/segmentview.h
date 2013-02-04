#ifndef SEGMENTVIEW_H
#define SEGMENTVIEW_H

#include "definition.h"

#include <QTreeWidget>
#include <QGraphicsView>
#include <QVector>
#include <QMouseEvent>

// Number of segment items in one row
#define SEGMENT_ITEMS_IN_ROW     64
#define SEGMENT_ITEMS_OFFSET     5
#define SEGMENT_GRID_OFFSET_PXX  60
#define SEGMENT_GRID_OFFSET_PXY  5

class SegmentItem;

class SegmentView : public QGraphicsView {
    Q_OBJECT
  public:
    explicit SegmentView(QTreeWidget *tree, QWidget *parent = 0);
    virtual ~SegmentView();

    //! Setup id of segment to visualize
    void setSegmentId(quint32 segId);
    //! Return currently visualized segment id
    quint32 segmentId() const;

    //! Reset view
    void reset();

    void activateItem(SegmentItem *csi);
    quint32 getCount(sc_type type);

  protected:
    //! Updates segment items
    void updateSegmentItems();

  private:
    //! Id of visualized segment
    quint32 mSegmentId;
    //! Vector of segment items
    typedef QVector<SegmentItem*> tSegmentItemsVector;
    tSegmentItemsVector mSegmentItems;

    QTreeWidget *mSegmentInfo;
    QGraphicsRectItem *mCurrentItem;

  signals:
  public slots:
    void mousePressEvent(QMouseEvent *e);
    void infoItemClicked(QTreeWidgetItem *item, int row);
};

#endif // SEGMENTVIEW_H
