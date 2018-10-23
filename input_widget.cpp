#include "input_widget.hpp"

InputWidget::InputWidget(QWidget *parent) : QPlainTextEdit(parent) {
    setObjectName("input");
    setPlaceholderText(QString("Type code here."));
    installEventFilter(this);
}

bool InputWidget::eventFilter(QObject * obj, QEvent * event) {
    if(event->type()==QEvent::KeyPress) {
        pressedKeys += ((QKeyEvent*)event)->key();
        if(pressedKeys.contains(Qt::Key_Shift) && pressedKeys.contains(Qt::Key_Return)) {
            // sent the current text to be evaluated and output
            QString program = this->toPlainText();
            emit send_input(program);
        }
        else if(pressedKeys.contains(Qt::Key_C) && pressedKeys.contains(Qt::Key_Control)) {
            // exit the application
            emit QApplication::quit();
        }
        else if(pressedKeys.contains(Qt::Key_Space) && pressedKeys.contains(Qt::Key_Control)) {
            //clear the screen
            emit clear_output();
            setPlainText(QString(""));
        }
    }
    else if(event->type()==QEvent::KeyRelease) {
        pressedKeys -= ((QKeyEvent*)event)->key();
    }
    return obj->event(event);
}