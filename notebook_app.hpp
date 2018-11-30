#ifndef NOTEBOOK_APP_H
#define NOTEBOOK_APP_H

#include <QWidget>
#include <QLayout>

#include "input_widget.hpp"
#include "output_widget.hpp"

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "TSmessage.hpp"

#include <fstream>
#include <sstream>

typedef TSmessage<std::string> InputQueue;
typedef std::pair<Expression, std::string> output_type;
typedef TSmessage<output_type> OutputQueue;

class NotebookApp : public QWidget {
    Q_OBJECT

    public:
        NotebookApp(QWidget *parent = nullptr);

    signals:
        void send_result(Expression exp);
        void send_failure(std::string message);

     private slots:
        void catch_input(QString r);

    private:
        InputWidget * in;
        OutputWidget * out;
        Interpreter mrInterpret;
        InputQueue * inputQ = new InputQueue;
        OutputQueue * outputQ = new OutputQueue;
};

#endif