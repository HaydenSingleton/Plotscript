#ifndef NOTEBOOK_APP_H
#define NOTEBOOK_APP_H

#include <QWidget>
#include <QLayout>

#include "input_widget.hpp"
#include "output_widget.hpp"

class NotebookApp : public QWidget{
    Q_OBJECT

    public:
        explicit NotebookApp(QWidget *parent = nullptr) {
            setObjectName("notebook");
            in = new InputWidget(this); //child widgets of notebook
            out = new OutputWidget(this);

            auto layout = new QVBoxLayout(this); // add the widgets to a vertical layout
            layout->addWidget(in);
            layout->addWidget(out);
            setLayout(layout);

        };

    private:
        InputWidget *in;
        OutputWidget *out;
};

#endif