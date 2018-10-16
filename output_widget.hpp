#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>

class OutputWidget: public QWidget {
    Q_OBJECT

    public:
        OutputWidget(QWidget * parent = nullptr) : QWidget(parent) {
            setObjectName("output");
        };

    private slots:
        void expression_result(QString r){
            return;
        };


};

#endif