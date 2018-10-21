#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QApplication>

#include "interpreter.hpp"

class OutputWidget: public QWidget {
    Q_OBJECT

    public:
        OutputWidget(QWidget * parent = (QWidget *)nullptr);

    private slots:
        void catch_result(Expression e);
        void catch_failure(bool fatal, std::string message);
        void clear_screen();

    private:
        QGraphicsView * view = new QGraphicsView(this);
        QGraphicsScene * scene = new QGraphicsScene(this);
        bool clear_on_print = true;

};

#endif