#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QApplication>
#include <QtMath>

#include "interpreter.hpp"

class OutputWidget: public QWidget {
    Q_OBJECT

    public:
        OutputWidget(QWidget * parent = (QWidget *)nullptr);

    private slots:
        void catch_result(Expression e);
        void catch_failure(std::string message);
        void clear_screen();

    private:
        QGraphicsView * view = new QGraphicsView(this);
        QGraphicsScene * scene = new QGraphicsScene(this);
        bool clear_on_print = true;
        void resizeEvent(QResizeEvent *event) override;
        void drawText(QString str, double sf = 1, double rot = 0, double x = 0, double y = 0);
        void drawLine(double, double, double, double, double);
        void drawPoint(double, double, double);

        std::string removeQuotes(std::string s){
            s.erase(s.begin());
            s.erase(s.size()-1);
            return s;
        };
};


#endif