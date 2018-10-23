#include <QApplication>
#include "notebook_app.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  NotebookApp NA;
  NA.show();

  return app.exec();
}

