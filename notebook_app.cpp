#include "notebook_app.hpp"

NotebookApp::NotebookApp(QWidget *parent) : QWidget(parent) {
    setObjectName("notebook");

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
    default_state = mrInterpret;
    c1 = *(new Consumer(inputQ, outputQ, mrInterpret));
    c1.startThread();

    in = new InputWidget(this); //child widgets of notebook
    out = new OutputWidget(this);

    auto row = new QHBoxLayout();
    auto startButton = new QPushButton(QString("Start Kernal"), this);
    row->addWidget(startButton);
    auto stopButton = new QPushButton(QString("Stop Kernal"), this);
    row->addWidget(stopButton);
    auto resetButton = new QPushButton(QString("Reset Kernal"), this);
    row->addWidget(resetButton);
    auto interuptButton = new QPushButton(QString("Interrupt"), this);
    row->addWidget(interuptButton);

    auto layout = new QVBoxLayout(this); // add the widgets to a vertical layout
    layout->addLayout(row);
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

    // connect buttons to functions in this class
    QObject::connect(startButton, SIGNAL(clicked()), this, SLOT(start_kernal()));
    QObject::connect(stopButton, SIGNAL(clicked()), this, SLOT(stop_kernal()));
    QObject::connect(resetButton, SIGNAL(clicked()), this, SLOT(reset_kernal()));
    QObject::connect(resetButton, SIGNAL(clicked()), out, SLOT(clear_screen()));
    QObject::connect(interuptButton, SIGNAL(clicked()), this, SLOT(interrupt_kernal()));
}

void NotebookApp::catch_input(QString s){
    std::string errorMessage;
    Expression result;
    output_type results;

    if(c1.isRunning()){
        inputQ->push(s.toStdString());
        ///THREAD STUFF HAPPENS
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
    else
        emit send_failure("Error: Interpreter kernal is not running");
}

void NotebookApp::start_kernal() {
    c1.startThread();
}

void NotebookApp::stop_kernal() {
    c1.stopThread();
}

void NotebookApp::reset_kernal() {
    c1.resetThread(default_state);
}

void NotebookApp::interrupt_kernal() {
    //TODO
}