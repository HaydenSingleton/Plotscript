#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <QPlainTextEdit>
#include <QKeySequence>
#include <QSet>
#include "iostream"

class InputWidget: public QPlainTextEdit {
    Q_OBJECT

    public:
        InputWidget(QWidget *parent = nullptr) : QPlainTextEdit(parent) {
            setObjectName("input");
            setBackgroundVisible(true);
            setPlaceholderText(QString("Type code here."));
            installEventFilter(this);
        };

    protected:

        // convience function to clear the screen on a click
        void mousePressEvent(QMouseEvent *event) override {
            if (event->button() == Qt::LeftButton) {
                setPlainText(QString());
            }
        };

        // to catch when users press SHIFT+ENTER
        bool eventFilter(QObject * obj, QEvent * event) {
            if(event->type()==QEvent::KeyPress) {
                pressedKeys += ((QKeyEvent*)event)->key();
                if( pressedKeys.contains(Qt::Key_Shift) && pressedKeys.contains(Qt::Key_Return) )
                {
                    emit send_input(this->toPlainText());
                    setPlainText(QString("YES!! YOU PRESSED SHIFT AND ENTER"));
                    return true;
                }
                if( pressedKeys.contains(Qt::Key_C) && pressedKeys.contains(Qt::Key_Control) )
                {
                    // clear screen
                    setPlainText(QString(""));
                    return true;
                }
            }
            else if(event->type()==QEvent::KeyRelease) {
                pressedKeys -= ((QKeyEvent*)event)->key();
            }
            return obj->event(event);
        }

    signals:
        void send_input(QString r);

    private:
        QSet<int> pressedKeys;

};

#endif