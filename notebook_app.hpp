#ifndef NOTEBOOK_APP_H
#define NOTEBOOK_APP_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>

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

class Consumer {
  private:
    InputQueue * iqueue;
    OutputQueue * oqueue;
    Interpreter cInterp;
    bool running = false;
    std::thread cThread;
  public:
    Consumer(InputQueue * inq, OutputQueue * outq, Interpreter & inter);
    Consumer();
    Consumer & operator=(Consumer & c) noexcept;
    ~Consumer();
    Consumer(Consumer & c);
    void ThreadFunction();
    bool isRunning();
    void startThread();
    void stopThread();
    void resetThread(Interpreter & newinter);
};

class NotebookApp : public QWidget {
    Q_OBJECT

    public:
        NotebookApp(QWidget *parent = nullptr);

    signals:
        void send_result(Expression exp);
        void send_failure(std::string message);

     private slots:
        void catch_input(QString r);
        void start_kernal();
        void stop_kernal();
        void reset_kernal();
        void interrupt_kernal();

    private:
        InputWidget * in;
        OutputWidget * out;
        Interpreter mrInterpret;
        Interpreter default_state;
        InputQueue * inputQ = new InputQueue;
        OutputQueue * outputQ = new OutputQueue;
        Consumer * c1;
        QPushButton* startButton, stopButton, resetButton, interuptButton;
};

#endif