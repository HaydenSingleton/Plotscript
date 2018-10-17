#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QApplication>

#include "interpreter.hpp"
#include "iostream"

class OutputWidget: public QWidget {
    Q_OBJECT

    public:
        OutputWidget(QWidget * parent = (QWidget *)nullptr);

    private slots:
        void catch_result(Expression e);
        void catch_failure(int code, std::string message);
        void clear_screen();

    private:
        QGraphicsView * view = new QGraphicsView(this);
        QGraphicsScene * scene = new QGraphicsScene(this);

};

#endif