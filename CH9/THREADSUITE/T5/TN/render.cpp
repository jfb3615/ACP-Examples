#include <QAction>
#include <QApplication>
#include <QImage>
#include <QKeySequence>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPixmap>
#include <QVBoxLayout>
#include <QWidget>

#include "render.h"

void render(const PixelArray & pixels)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"

    char *       Argv[]={{"splash"}};
    int         Argc=1;
    QApplication application(Argc, Argv);

#pragma GCC diagnostic pop
    
    auto *window = new QMainWindow;
    window->setWindowTitle("Splash!!");
    window->resize(900, 900);

    auto *centralWidget = new QWidget(window);
    auto *layout = new QVBoxLayout(centralWidget);

    //    const PixelArray pixels = createQuadrantImage();
    QImage image(reinterpret_cast<const uchar *>(pixels.data()),
                 kImageWidth,
                 kImageHeight,
                 QImage::Format_ARGB32);
    const QPixmap pixmap = QPixmap::fromImage(image);

    auto *imageLabel = new QLabel;
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(imageLabel);

    window->setCentralWidget(centralWidget);

    auto *quitAction = new QAction("Quit", window);
    quitAction->setShortcut(QKeySequence(Qt::Key_Q));
    quitAction->setStatusTip("Quit the application");
    QObject::connect(quitAction, &QAction::triggered, window, &QMainWindow::close);

    auto *menuBar = window->menuBar();
    menuBar->addAction(quitAction);

    window->show();
    application.exec();
}
