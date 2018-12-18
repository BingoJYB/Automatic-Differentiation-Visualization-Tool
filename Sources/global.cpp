#include "global.h"

QGraphicsView *view;
GraphicsScene *scene;
MainWindow *mainwindow;
CodeViewer *editor;
int currIndex = 0;
int lastStart = 0;
long firstNodeId = 0;
bool isNotSetParam = true;
dag g_dag;
dag_layouter g_dag_layouter;
dag_extractor g_dag_extractor;
mapping_helper g_mapping_helper;
std::vector<int> *g_nodeidx2assignmentidxmap;
const double *g_adjoint_vector = 0;
