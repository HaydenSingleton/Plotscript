#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "interpreter.hpp"


class OutputWidget: public QWidget {
    Q_OBJECT

    public:
        OutputWidget(QWidget * parent = nullptr) : QWidget(parent) {
            setObjectName("output");
            scene = new QGraphicsScene(this);
            scene->addText("Output");
            view = new QGraphicsView(scene);
        };

    private slots:
        void catch_result(Expression e){
            if(e.isNone()){ // Not a special case, display normally

            }


        };

        void catch_failure(int code, std::string message){
            QString msg = QString::fromStdString(message);
            std::cout << msg.toStdString() << std::endl;
            if(code == 0) {
                QGraphicsTextItem *output = new QGraphicsTextItem();
                output->setAcceptHoverEvents(false);
                output->setPlainText(msg);
            }
            else {
                emit QCoreApplication::quit();
            }
        };

    private:
        QGraphicsScene *scene;
        QGraphicsView *view;

};

#endif