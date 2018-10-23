#include <QTest>
#include "notebook_app.hpp"

class NotebookTest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();
  void testInputWidget();

private:
  NotebookApp widget;

};

void NotebookTest::initTestCase(){
  auto input = widget.findChild<InputWidget *>("input");
  auto output = widget.findChild<OutputWidget *>("output");
  QVERIFY2(input, "Could not find input widget");
  QVERIFY2(output, "Could not find output widget");
  QVERIFY2(widget.objectName() == "notebook", "Notebook object name incorrect");
  widget.show();
}

void NotebookTest::testInputWidget(){

}


QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
