#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QTimer>
#include <QFile>

class ProcessMonitor : public QMainWindow {
    Q_OBJECT
public:
    ProcessMonitor() {
        auto *textEdit = new QTextEdit(this);
        textEdit->setReadOnly(true);
        setCentralWidget(textEdit);

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [textEdit]() {
            QFile file("/proc/srmc");
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                textEdit->setPlainText(file.readAll());
            }
        });
        timer->start(1000);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ProcessMonitor monitor;
    monitor.show();
    return app.exec();
}
