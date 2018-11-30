#include "notebook_app.hpp"

class Consumer {
  private:
    InputQueue * iqueue;
    OutputQueue * oqueue;
  public:
    Consumer(InputQueue * i, OutputQueue * o) : iqueue(i), oqueue(o){}
    void operator()(Interpreter & miniInterpret) const {
      while(true){

        std::string line;
        iqueue->wait_and_pop(line);
        if(line==""){
          break;
        }
        std::istringstream expression(line);

        Expression result;
        std::string error;

        if(!miniInterpret.parseStream(expression)){
          error = "Error: Invalid Expression. Could not parse.";
        }
        else{
          try{
            result = miniInterpret.evaluate();
          }
          catch(const SemanticError & ex){
            error = ex.what();
          }
        }

        output_type output = std::make_pair(result, error);
        oqueue->push(output);
      }
    }
};

NotebookApp::NotebookApp(QWidget *parent) : QWidget(parent) {
    setObjectName("notebook");

    in = new InputWidget(this); //child widgets of notebook
    out = new OutputWidget(this);

    auto layout = new QVBoxLayout(this); // add the widgets to a vertical layout
    layout->addWidget(in, 1);
    layout->addWidget(out, 1);
    setLayout(layout);

    // connect input to this notebook for evaluating
    QObject::connect(in, SIGNAL(send_input(QString)), this, SLOT(catch_input(QString)));

    // sent evaluated expressions to output widget
    QObject::connect(this, SIGNAL(send_result(Expression)), out, SLOT(catch_result(Expression)));

    // send any errors to output widget with a different mechanism
    QObject::connect(this, SIGNAL(send_failure(std::string)), out, SLOT(catch_failure(std::string)));

    // send the clear signal to the output
    QObject::connect(in, SIGNAL(clear_output()), out, SLOT(clear_screen()));

    //startup procedure
    std::ifstream startip_str(STARTUP_FILE);
    if(!mrInterpret.parseStream(startip_str)){
        emit send_failure("Error: Invalid Startup Program. Could not parse.");
    }
    else{
        try{
            Expression exp = mrInterpret.evaluate();
        }
        catch(const SemanticError & ex){
            emit send_failure(ex.what());
        }
    }
}

void NotebookApp::catch_input(QString s){
    std::string errorMessage;
    Expression result;

    Consumer c1(inputQ, outputQ);
    std::thread cThread(c1, std::ref(mrInterpret));
    cThread.detach();

    inputQ->push(s.toStdString());

    std::cout << "Sent input string: " << s.toStdString() << "\n";

    output_type results;
    outputQ->wait_and_pop(results);

    if(results.second=="")
    {
        emit send_result(results.first);
    }
    else
    {
        emit send_failure(results.second);
    }
}



    // std::istringstream expression(s.toStdString());
    // if(!mrInterpret.parseStream(expression)){
    //     emit send_failure("Error: Invalid Expression. Could not parse.");
    // }
    // else{
    //     try{
    //         Expression exp = mrInterpret.evaluate();
    //         emit send_result(exp);
    //     }
    //     catch(const SemanticError & ex){
    //         emit send_failure(ex.what());
    //     }
    // }
// }