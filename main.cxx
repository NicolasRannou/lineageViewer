// QT includes
#include <QApplication>
#include "lineageViewer.h"

#include "vtkFileOutputWindow.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkTable.h"
#include "vtkTree.h"
#include "vtkTreeReader.h"
#include "vtkTreeWriter.h"

int main( int argc, char** argv )
{
  // QT Stuff
  QApplication app( argc, argv );

  lineageViewer myLineageViewer;
  myLineageViewer.show();

  int ret = app.exec();
  return ret;
}
