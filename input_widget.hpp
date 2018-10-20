#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <QPlainTextEdit>
#include <QKeySequence>
#include <QSet>
#include <QApplication>

class InputWidget: public QPlainTextEdit {
    Q_OBJECT

    public:
        InputWidget(QWidget *parent = nullptr);

    signals:
        void send_input(QString r);
        void clear_output();

    private:
        QSet<int> pressedKeys;

        // to catch when users press SHIFT+ENTER
        bool eventFilter(QObject * obj, QEvent * event);
};

#endif