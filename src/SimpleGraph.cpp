#include <iostream>
// Bugfix: Disable C++11 feature macros on Mac/libstdc++ to fool Qt into not using C++11 headers
#ifdef __APPLE__
#undef __cplusplus
#define __cplusplus 200303L
#undef __GXX_EXPERIMENTAL_CXX0X__
#endif

#include <QtGui>
#include <QWidget>
#include <QApplication>
#include <algorithm>
#include <QCoreApplication>
#include <QObject>
#include <QSemaphore>

#include "SimpleGraph.h"
#undef main

MyWidget& MyWidget::getInstance() {
    static MyWidget instance;
    return instance;
}

/* global semaphore to ensure only one update call at a time.
 * Prevents overload of update calls */
QSemaphore semaphore;

void InitGraphVisualizer(SimpleGraph & userGraph) {
    MyWidget& g = MyWidget::getInstance();
    QObject::connect(&userGraph, SIGNAL(graphUpdated(SimpleGraph)),
                     &g, SLOT(repaint()));
}

void MyWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    if (!userGraph.nodes.empty()) {
        SimpleGraph copy;
        copy.nodes = userGraph.nodes;
        copy.edges = userGraph.edges;        auto getX = [](const Node& a, const Node& b) {return a.x < b.x;};
        double maxX = std::max_element(copy.nodes.begin(), copy.nodes.end(), getX)->x;
        double minX = std::min_element(copy.nodes.begin(), copy.nodes.end(), getX)->x;

        auto getY = [](const Node& a, const Node& b) { return a.y < b.y;};
        double maxY = std::max_element(copy.nodes.begin(), copy.nodes.end(), getY)->y;
        double minY = std::min_element(copy.nodes.begin(), copy.nodes.end(), getY)->y;

        auto scale = [maxX, maxY, minX, minY](const Node& a) -> Node {
            return {(a.x - minX) * 590 / (-minX + maxX) + 5, (a.y - minY) * 590 / (-minY + maxY) + 5};
        };
        painter.setBrush(Qt::blue);
        std::transform(copy.nodes.begin(), copy.nodes.end(), copy.nodes.begin(), scale);
        for (Edge & e : copy.edges) {
            Node start = copy.nodes[e.start];
            Node end = copy.nodes[e.end];
            painter.drawLine(start.x, start.y, end.x, end.y);
        }
        for (Node & n : copy.nodes) {
            painter.drawEllipse(n.x - 5, n.y - 5, 10, 10);
        }
    }
    //last_run = QTime::currentTime();
    semaphore.release();
}

class WorkerThread : public QThread {
    void run() {
        int _userMain();
        _userMain();

    }
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    MyWidget & myWidget = MyWidget::getInstance();
    myWidget.resize(600, 600);
    myWidget.show();
    qRegisterMetaType<SimpleGraph>(); //allows use of SimpleGraph in signals/slots
    WorkerThread x;
    x.start();
    return app.exec();
}

void SimpleGraph::drawGraph(SimpleGraph &graph) {
    if (!semaphore.tryAcquire()) return;
    MyWidget& m = MyWidget::getInstance();
    m.userGraph.nodes = graph.nodes;
    m.userGraph.edges = graph.edges;
    emit graphUpdated(graph);
}

void DrawGraph(SimpleGraph& userGraph) {
    userGraph.drawGraph(userGraph);
}
