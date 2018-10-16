#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "interpreter.hpp"


class OutputWidget: public QWidget {
    Q_OBJECT

    public:
        OutputWidget(QWidget * parent = nullptr) : QWidget(parent) {
            setObjectName("output");
        };

    private slots:
        void catch_result(Expression exp){

        };
        void catch_failure(int code, std::string message){

        };

};

#endif