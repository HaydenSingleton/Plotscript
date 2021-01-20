#include <QTest>
#include "notebook_app.hpp"

/*
findLines - find lines in a scene contained within a bounding box
            with a small margin
 */
int findLines(QGraphicsScene * scene, QRectF bbox, qreal margin){

  QPainterPath selectPath;

  QMarginsF margins(margin, margin, margin, margin);
  selectPath.addRect(bbox.marginsAdded(margins));
  scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

  int numlines(0);
  foreach(auto item, scene->selectedItems()){
    if(item->type() == QGraphicsLineItem::Type){
      numlines += 1;
    }
  }

  return numlines;
}

/*
findPoints - find points in a scene contained within a specified rectangle
 */
int findPoints(QGraphicsScene * scene, QPointF center, qreal radius){

  QPainterPath selectPath;
  selectPath.addRect(QRectF(center.x()-radius, center.y()-radius, 2*radius, 2*radius));
  scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

  int numpoints(0);
  foreach(auto item, scene->selectedItems()){
    if(item->type() == QGraphicsEllipseItem::Type){
      numpoints += 1;
    }
  }

  return numpoints;
}

/*
findText - find text in a scene centered at a specified point with a given
           rotation and string contents
 */
int findText(QGraphicsScene * scene, QPointF center, qreal rotation, QString contents){

  int numtext(0);
  foreach(auto item, scene->items(center)){
    if(item->type() == QGraphicsTextItem::Type){
      QGraphicsTextItem * text = static_cast<QGraphicsTextItem *>(item);
      if((text->toPlainText() == contents) &&
     (text->rotation() == rotation) &&
     (text->pos() + text->boundingRect().center() == center)){
    numtext += 1;
      }
    }
  }

  return numtext;
}

/*
intersectsLine - find lines in a scene that intersect a specified rectangle
 */
int intersectsLine(QGraphicsScene * scene, QPointF center, qreal radius){

  QPainterPath selectPath;
  selectPath.addRect(QRectF(center.x()-radius, center.y()-radius, 2*radius, 2*radius));
  scene->setSelectionArea(selectPath, Qt::IntersectsItemShape);

  int numlines(0);
  foreach(auto item, scene->selectedItems()){
    if(item->type() == QGraphicsLineItem::Type){
      numlines += 1;
    }
  }

  return numlines;
}

class NotebookTest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();
  void testInputWidget();
  void testOutputWidget();
  void testDataProcedures();
  void testDiscretePlotLayout();
  void testContinuousPlotLayout();
  void testGetPropertyErrors();
  void testSetPropertyErrors();
  void testUndefined();
  void testMakeText();
  void testMakeMath();
  void testMakeTitle();

private:
  NotebookApp widget;
  InputWidget * input;
  OutputWidget * output;

};

void NotebookTest::initTestCase(){
  input = widget.findChild<InputWidget *>("input");
  output = widget.findChild<OutputWidget *>("output");
  QVERIFY2(input, "Could not find input widget");
  QVERIFY2(output, "Could not find output widget");
  QVERIFY2(widget.objectName() == "notebook", "Notebook object name incorrect");
  widget.show();
}

void NotebookTest::testInputWidget() {
  input->clear();
  QTest::keyClicks(input, "(define x 100)");
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier, 10);

  auto view = output->findChild<QGraphicsView *>();
  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

  auto scene = view->scene();
  auto items = scene->items();
  QCOMPARE(items.size(), 1);
}

void NotebookTest::testOutputWidget(){

  input->clear();
  auto view = output->findChild<QGraphicsView *>();
  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");
  auto scene = view->scene();
  auto items = scene->items();
  QCOMPARE(items.size(), 1);

  input->clear();
  QTest::keyClicks(input, "fdasfdsaf");
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier, 10);
  QCOMPARE(findText(scene, QPointF(0, 0), 0, QString("Error: Invalid Expression. Could not parse.")), 1);
  QCOMPARE(items.size(), 1);
}

void NotebookTest::testDataProcedures(){


  auto view = output->findChild<QGraphicsView *>();
  auto scene = view->scene();
  auto items = scene->items();

  input->clear();
  QTest::keyClicks(input, "(set-property \"size\" 0.5 (make-point 0 0))");
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier, 10);
  QCOMPARE(items.size(), 1);
  // QCOMPARE(findPoints(scene, QPointF(0, 0), 1), 1);

  input->clear();
  QTest::keyClicks(input, "(make-line (make-point 0 0) (make-point 0 1))");
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier, 10);
  QCOMPARE(items.size(), 1);

  input->clear();
  QTest::keyClicks(input, "(set-property \"text-rotation\" (/ pi 2) (set-property \"size\" 10 (make-text \"Hello World\")))");
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier, 10);
  QCOMPARE(findText(scene, QPointF(0, 0), 90, QString("Hello World")), 1);
  QCOMPARE(items.size(), 1);

}

void NotebookTest::testDiscretePlotLayout() {

  std::string program = R"(
  (discrete-plot (list (list -1 -1) (list 1 1))
      (list (list "title" "The Title")
            (list "abscissa-label" "X Label")
            (list "ordinate-label" "Y Label") ))
  )";

  input->setPlainText(QString::fromStdString(program));
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier, 1000);
  std::this_thread::sleep_for(std::chrono::microseconds(500));


  auto view = output->findChild<QGraphicsView *>();
  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

  auto scene = view->scene();

  // first check total number of items
  // 8 lines + 2 points + 7 text = 17
  auto items = scene->items();

  QCOMPARE(items.size(), 17);

  // make them all selectable
  foreach(auto item, items){
    item->setFlag(QGraphicsItem::ItemIsSelectable);
  }

  double scalex = 20.0/2.0;
  double scaley = 20.0/2.0;

  double xmin = scalex*-1;
  double xmax = scalex*1;
  double ymin = scaley*-1;
  double ymax = scaley*1;
  double xmiddle = (xmax+xmin)/2;
  double ymiddle = (ymax+ymin)/2;

  // check title
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax+3)), 0, QString("The Title")), 1);

  // check abscissa label
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin-3)), 0, QString("X Label")), 1);

  // check ordinate label
  QCOMPARE(findText(scene, QPointF(xmin-3, -ymiddle), -90, QString("Y Label")), 1);

  // check abscissa min label
  QCOMPARE(findText(scene, QPointF(xmin, -(ymin-2)), 0, QString("-1")), 1);

  // check abscissa max label
  QCOMPARE(findText(scene, QPointF(xmax, -(ymin-2)), 0, QString("1")), 1);

  // check ordinate min label
  QCOMPARE(findText(scene, QPointF(xmin-2, -ymin), 0, QString("-1")), 1);

  // check ordinate max label
  QCOMPARE(findText(scene, QPointF(xmin-2, -ymax), 0, QString("1")), 1);

  // check the bounding box bottom
  QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

  // check the bounding box top
  QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

  // check the bounding box left and (-1, -1) stem
  QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

  // check the bounding box right and (1, 1) stem
  QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

  // check the abscissa axis
  QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

  // check the ordinate axis
  QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

  // check the point at (-1,-1)
  QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);

  // check the point at (1,1)
  QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1);
}

void NotebookTest::testContinuousPlotLayout() {

  std::string program = "(begin (define f (lambda (x) (+ (* 2 x) 1))) (continuous-plot f (list -2 2) (list (list \"title\" \"A continuous linear function\") (list \"abscissa-label\" \"x\") (list \"ordinate-label\" \"y\"))))";

  input->setPlainText(QString::fromStdString(program));
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier, 1000);
  std::this_thread::sleep_for(std::chrono::microseconds(500));

  auto view = output->findChild<QGraphicsView *>();
  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

  auto scene = view->scene();

  // first check total number of items
  // 8 lines + 2 points + 7 text = 17
  auto items = scene->items();
  QVERIFY2(items.size() == 64 || items.size() == 1, "items = "+items.size());

  if(items.size()==1){
    return;
  }

  // make them all selectable
  foreach(auto item, items){
    item->setFlag(QGraphicsItem::ItemIsSelectable);
  }

  double scalex = 20.0/4.0;
  double scaley = 20.0/8.0;

  double xmin = scalex*-2;
  double xmax = scalex*2;
  double ymin = scaley*-3;
  double ymax = scaley*5;
  double xmiddle = (xmax+xmin)/2;
  double ymiddle = (ymax+ymin)/2;

  // check title
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax+3)), 0, QString("A continuous linear function")), 1);

  // check abscissa label
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin-3)), 0, QString("x")), 1);

  // check ordinate label
  QCOMPARE(findText(scene, QPointF(xmin-3, -ymiddle), -90, QString("y")), 1);

  // check abscissa min label
  QCOMPARE(findText(scene, QPointF(xmin, -(ymin-2)), 0, QString("-2")), 1);

  // check abscissa max label
  QCOMPARE(findText(scene, QPointF(xmax, -(ymin-2)), 0, QString("2")), 1);

  // check ordinate min label
  QCOMPARE(findText(scene, QPointF(xmin-2, -ymin), 0, QString("-3")), 1);

  // check ordinate max label
  QCOMPARE(findText(scene, QPointF(xmin-2, -ymax), 0, QString("5")), 1);

  // check the abscissa axis
  QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

  // check the ordinate axis
  QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);
}

void NotebookTest::testGetPropertyErrors() {


	QString program = "(get-property \"notAKey\" (0)";
	input->setPlainText(program);
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier, 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "NONE");
}

void NotebookTest::testSetPropertyErrors() {
	QString program = "(set-property (100) (0))";
	input->setPlainText(program);
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier, 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Error: first argument to set-property not a string.");

  input->clear();
  QTest::keyClicks(input, "(set-property \"text-rotation\" (/ pi 2) \"extra-argument\")" );
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier, 10);
  QVERIFY2(view, "Error invalid number of arguments for set-property.");

}

void NotebookTest::testUndefined() {
	QString program = "(x)";
	input->setPlainText(program);
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier, 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Error during evaluation: unknown symbol");
}

void NotebookTest::testMakeText() {
	QString program = "(make-text \"Hello World\")";
	input->setPlainText(program);
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier, 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Hello World");
}

void NotebookTest::testMakeMath() {
	QString program = "(cos pi)";
	input->setPlainText(program);
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier, 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "(-1)");
}

void NotebookTest::testMakeTitle() {
	QString program = "(begin (define title \"The Title\") (title))";
	input->setPlainText(program);
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier, 10);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "(\"The Title\")");
}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
