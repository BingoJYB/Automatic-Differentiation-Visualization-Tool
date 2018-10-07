#include "global.h"
#include "mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>

int startup(int nodeidx) {
    int argc = 1;
    char *argv = "foo";
    QApplication app(argc, &argv);
    MainWindow window;
    window.resize(1024, 512);
    window.setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            window.size(),
            qApp->desktop()->availableGeometry()
        )
    );
    window.init(nodeidx);
    window.show();

    return app.exec();
}


extern "C" {
    void showGUI(int nodeidx, std::vector<int> &node2assignmentidxmap, const double* adjv) {
        g_nodeidx2assignmentidxmap = &node2assignmentidxmap;
        g_adjoint_vector = adjv;
        startup(nodeidx);
    }

    void initpath(const char* sourcepath) {
        g_mapping_helper.set_path(sourcepath);
    }
}

int main(int argc, char *argv[]) {

}
