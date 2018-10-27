#include <QTest>
#include <QtTest>
#include "notebook_app.hpp"

class NotebookTest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();

  void testInputWidget();

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

void NotebookTest::testInputWidget(){
  QTest::keyClicks(input, "hello world");
  input->clear();
  QTest::keyClicks(input, "(define x 100)");
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier, 10);
  // QTest::qWait(10);
  auto scene = output->findChild<QGraphicsScene *>();
  const QString name = QString();
  auto result = scene->items()[0];
  auto expectedresult = new QGraphicsTextItem(QString("(100)"));
  QVERIFY2(result == expectedresult, "Could not find expected result in ouput");

}


QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
