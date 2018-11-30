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

class Consumer {
  private:
    InputQueue * iqueue;
    OutputQueue * oqueue;
    Interpreter cInterp;
    bool running = false;
    std::thread cThread;
  public:
    Consumer(InputQueue * inq, OutputQueue * outq, Interpreter & inter): cThread() {
      iqueue = inq;
      oqueue = outq;
      cInterp = inter;
    }
    Consumer(){
    }
    Consumer & operator=(Consumer & c){
      iqueue = c.iqueue;
      oqueue = c.oqueue;
      cInterp = c.cInterp;
      cThread.swap(c.cThread);
    }
    ~Consumer(){
      stopThread();
    }
    Consumer(Consumer & c){
      iqueue = c.iqueue;
      oqueue = c.oqueue;
      cInterp = c.cInterp;
      cThread.swap(c.cThread);
    }
    void ThreadFunction() {
        while(isRunning()){
          std::string line;
          iqueue->wait_and_pop(line);
          std::istringstream expression(line);

          if(line == "")
            continue;

          Expression result;
          std::string error;

          if(!cInterp.parseStream(expression)){
            error = "Invalid Expression. Could not parse.";
          }
          else{
            try{
              result = cInterp.evaluate();
            }
            catch(const SemanticError & ex){
              error = ex.what();
            }
          }

          output_type output = std::make_pair(result, error);
          oqueue->push(output);
        }
    }
    bool isRunning(){
      return running;
    }
    void startThread(){
      if(!running){
        running = true;
        cThread = std::thread(&Consumer::ThreadFunction, this);
      }
    }
    void stopThread(){
      if(running){
        running = false;
        std::string empty;
        iqueue->push(empty);
        cThread.join();
        if(!iqueue->empty())
          iqueue->wait_and_pop(empty);
          assert(iqueue->empty());
      }
    }
    void resetThread(Interpreter & newinter){
      if(running){
        stopThread();
        cInterp = newinter;
        startThread();
      }
      else{
        startThread();
      }
    }
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

    private:
        InputWidget * in;
        OutputWidget * out;
        Interpreter mrInterpret;
        InputQueue * inputQ = new InputQueue;
        OutputQueue * outputQ = new OutputQueue;
        Consumer c1;
};

#endif