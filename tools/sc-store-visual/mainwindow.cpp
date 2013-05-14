#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mSegmentsList(0),
    mSegmentArea(0),
    mLabelNodeCount(0),
    mLabelArcsCount(0),
    mLabelOtherCount(0) {
  ui->setupUi(this);

  createMainWidgets();
  setupMenuActions();
}


void MainWindow::createMainWidgets() {
  QWidget *central = new QWidget();

  mSegmentsList = new QListWidget(central);
  mSegmentsList->setMaximumWidth(200);
  connect(mSegmentsList, SIGNAL(currentTextChanged(QString)), this, SLOT(segmentSelectionChanged(QString)));

  mSegmentInfo = new QTreeWidget();
  mSegmentInfo->setColumnCount(2);
  mSegmentInfo->setSortingEnabled(false);

  QStringList hls;

  hls << QString("Property");
  hls << QString("Value");

  mSegmentInfo->setHeaderLabels(hls);

  QGroupBox *segmentInfoGroup = new QGroupBox(tr("Segment info"), central);
  QVBoxLayout *infoLayout = new QVBoxLayout(central);

  mLabelNodeCount = new QLabel(tr("Nodes: "), segmentInfoGroup);
  mLabelArcsCount = new QLabel(tr("Arcs: "), segmentInfoGroup);
  mLabelOtherCount = new QLabel(tr("Other: "), segmentInfoGroup);
  QLabel *mal0 = new QLabel(tr(""), segmentInfoGroup);

  infoLayout->addWidget(mLabelNodeCount, 0);
  infoLayout->addWidget(mLabelArcsCount, 0);
  infoLayout->addWidget(mLabelOtherCount, 0);
  infoLayout->addWidget(mal0, 1);

  segmentInfoGroup->setLayout(infoLayout);

  QVBoxLayout *rightLayout = new QVBoxLayout();
  rightLayout->addWidget(segmentInfoGroup);
  rightLayout->addWidget(mSegmentInfo);

  QScrollArea *scroll = new QScrollArea(central);

  mSegmentArea = new SegmentArea(mSegmentInfo, scroll);
  connect(mSegmentInfo, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), mSegmentArea, SLOT(infoItemClicked(QTreeWidgetItem*, int)));

  scroll->setWidget(mSegmentArea);

  QHBoxLayout *mainLayout = new QHBoxLayout();
  mainLayout->addWidget(mSegmentsList, 1);
  mainLayout->addWidget(scroll, 10);
  mainLayout->addLayout(rightLayout, 2);

  central->setLayout(mainLayout);

  setCentralWidget(central);
}

void MainWindow::setupMenuActions() {
  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openRepository()));
  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow() {
  delete ui;

  delete mSegmentsList;
//  delete mSegmentView;
  delete mSegmentArea;
  delete mSegmentInfo;
}

void MainWindow::updateSegmentsList() {
  Q_ASSERT(sc_storage_is_initialized());
  mSegmentsList->clear();

  quint32 segmentsCount = sc_storage_get_segments_count();
  for (quint32 seg = 0; seg < segmentsCount; seg++) {
    QListWidgetItem *item = new QListWidgetItem(QString("%1").arg(seg));

    sc_segment *segment = sc_storage_get_segment(seg, SC_FALSE);
    if (segment == nullptr)
      item->setBackgroundColor(QColor(195, 195, 195));

    mSegmentsList->addItem(item);
  }
}


void MainWindow::openRepository() {
  QString path = QFileDialog::getExistingDirectory(this, tr("Choose repository"));

  if (path.length() > 0) {
    if (sc_storage_is_initialized()) sc_storage_shutdown();

    sc_storage_initialize(path.toStdString().c_str());
    updateSegmentsList();
  }
}

void MainWindow::segmentSelectionChanged(QString strId) {
  if (strId.isEmpty()) {
//    mSegmentView->reset();
    return;
  }

  int id = strId.toInt();
//  mSegmentView->setSegmentId(id);
  mSegmentArea->setSegment(id);

  QListWidgetItem *item = mSegmentsList->item(id);
  if (item != nullptr) {
    QColor color = QColor(255, 255, 255);
    if (sc_storage_get_segment(id, SC_FALSE) == nullptr) color = QColor(195, 195, 195);

    item->setBackgroundColor(color);
  }

  quint32 node = mSegmentArea->getCount(sc_type_node);
  quint32 arc  = mSegmentArea->getCount(sc_type_arc_common);

  mLabelNodeCount->setText(QString("Nodes: %1").arg(node));
  mLabelArcsCount->setText(QString("Arcs: %1").arg(arc));
  mLabelOtherCount->setText(QString("Others: %1").arg(SEGMENT_SIZE - node - arc));
}
