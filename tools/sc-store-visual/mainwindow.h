#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "segmentarea.h"

#include <QMainWindow>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QListWidget>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>


class QListWidget;
class QLabel;
class SegmentView;
class SegmentArea;

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT
    
  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //! Create central widgets
    void createMainWidgets();

    //! Setup action slots
    void setupMenuActions();

  private:
    void updateSegmentsList();


  protected slots:
    void openRepository();

    void segmentSelectionChanged(QString strId);

  private:

    Ui::MainWindow *ui;

    QListWidget *mSegmentsList;
    SegmentArea *mSegmentArea;

    QLabel *mLabelNodeCount;
    QLabel *mLabelArcsCount;
    QLabel *mLabelOtherCount;

    QTreeWidget *mSegmentInfo;
};

#endif // MAINWINDOW_H
