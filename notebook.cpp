#include <QApplication>
#include <QWidget>
#include "myWidgets.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  NotebookApp widget;
  widget.setObjectName("NotebookApp");



  widget.show();

  return app.exec();
}

