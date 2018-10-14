#ifndef MYWIDGETS_HPP
#define MYWIDGETS_HPP

#include <QWidget>
#include <QPlainTextEdit>
#include <QLayout>

class InputWidget: public QPlainTextEdit {
    Q_OBJECT

    public:
        InputWidget(QObject * parent = nullptr);
  

};

class OutputWidget: public QWidget {
    Q_OBJECT

    public:
        OutputWidget(QWidget * parent = nullptr);

  

};

class NotebookApp : public QWidget{
    Q_OBJECT

    public:
        NotebookApp(QWidget *parent = nullptr);

    private:
        InputWidget *in;
        OutputWidget *out;
};

#endif