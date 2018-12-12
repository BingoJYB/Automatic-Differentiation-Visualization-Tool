#ifndef GLOBAL_H
#define GLOBAL_H

#include "graphicsscene.h"
#include "dag.h"
#include "dag_layouter.h"
#include "dag_extractor.h"
#include "mapping_helper.h"

#include <QtWidgets>

extern QGraphicsView *view;
extern GraphicsScene *scene;
extern MainWindow *mainwindow;
extern CodeViewer *editor;
extern int currIndex;
extern int lastStart;
extern long firstNodeId;
extern bool isNotSetParam;
extern dag g_dag;
extern dag_layouter g_dag_layouter;
extern dag_extractor g_dag_extractor;
extern mapping_helper g_mapping_helper;
extern std::vector<int> *g_nodeidx2assignmentidxmap;
extern const double *g_adjoint_vector;

#endif // GLOBAL_H
