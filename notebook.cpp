#include <QApplication>
#include "notebook_app.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  NotebookApp na;
  na.show();

  return app.exec();
}

