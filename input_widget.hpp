#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <QPlainTextEdit>
#include <QKeySequence>
#include <QSet>
#include "iostream"
#include <QApplication>

class InputWidget: public QPlainTextEdit {
    Q_OBJECT

    public:
        InputWidget(QWidget *parent = nullptr) : QPlainTextEdit(parent) {
            setObjectName("input");
            setBackgroundVisible(true);
            setPlaceholderText(QString("Type code here."));
            installEventFilter(this);
        };

    signals:
        void send_input(QString r);

    private:
        QSet<int> pressedKeys;

    // convience function to clear the screen on a click
        void mousePressEvent(QMouseEvent *event) override {
            if (event->button() == Qt::LeftButton) {
                setPlainText(QString(""));
            }
        };

        // to catch when users press SHIFT+ENTER
        bool eventFilter(QObject * obj, QEvent * event) {
            if(event->type()==QEvent::KeyPress) {
                pressedKeys += ((QKeyEvent*)event)->key();
                if(pressedKeys.contains(Qt::Key_Shift) && pressedKeys.contains(Qt::Key_Return)) {
                    // sent the current text to be evaluated and output
                    QString program = this->toPlainText();
                    setPlainText(QString(""));
                    emit send_input(program);
                }
                else if(pressedKeys.contains(Qt::Key_C) && pressedKeys.contains(Qt::Key_Control)) {
                    // exit the application
                    emit QCoreApplication::quit();
                }
                else if(pressedKeys.contains(Qt::Key_Control) && pressedKeys.contains(Qt::Key_Space)) {
                    //clear the screen
                    setPlainText(QString(""));
                }
            }
            else if(event->type()==QEvent::KeyRelease) {
                pressedKeys -= ((QKeyEvent*)event)->key();
            }
            return obj->event(event);
        }

};

#endif