#ifndef lineageViewer_H
#define lineageViewer_H

#include <QMainWindow>

#include "vtkSmartPointer.h"

class vtkObject;

// Forward Qt class declarations
class Ui_lineageViewer;
//table view
class vtkQtTreeView;
//graph view
class vtkGraphLayoutView;
class vtkTreeLayoutStrategy;
class vtkLookupTable;

// connection between graph and table
class vtkAnnotationLink;
class vtkEventQtSlotConnect;

class vtkMutableDirectedGraph;
class vtkTree;



// EXPERIMENTAL
// back plane
class vtkDelaunay2D;
class vtkGraphToPolyData;
class vtkPolyDataMapper;
class vtkActor;

class lineageViewer : public QMainWindow
{
  Q_OBJECT

public:

  lineageViewer( QWidget * iParent = 0, Qt::WindowFlags iFlags = 0 );
  ~lineageViewer();

  vtkSmartPointer<vtkMutableDirectedGraph> CreateGraph();

private:

  /*
   *
   */
  void ConfigureGraphView();
  /*
   *
   */
  void ConfigureTableView();

  /*
   *
   */
  void ConnectQtButtons();
  /*
   *
   */
  void FillQtComboBoxes();


  vtkSmartPointer<vtkTree> m_Tree;
  vtkSmartPointer<vtkMutableDirectedGraph> m_Graph;

  vtkSmartPointer<vtkQtTreeView>         m_treeTableView;
  vtkSmartPointer<vtkGraphLayoutView>    m_treeGraphView;
  vtkSmartPointer<vtkAnnotationLink>     m_annotationLink;
  vtkSmartPointer<vtkEventQtSlotConnect> m_connect;
  vtkSmartPointer<vtkLookupTable>        m_lut;
  vtkSmartPointer<vtkTreeLayoutStrategy> m_treeLayoutStrategy;

  // Experimental stuff...
  vtkSmartPointer<vtkDelaunay2D>         m_backPlane;
  vtkSmartPointer<vtkGraphToPolyData>    m_graphToPolyData;
  vtkSmartPointer<vtkPolyDataMapper>     m_planeMapper;
  vtkSmartPointer<vtkActor>              m_planeActor;

  Ui_lineageViewer *ui;

private slots:

    void slotLoadLineage();

  void selectionChanged(vtkObject*, unsigned long,void* ,void* );

  void slotEnableScale(int state);
  void slotChangeScale(QString array);

  void slotEnableColorCode(int state);
  void slotChangeColorCode(QString array);

  void slotEnableLabel(int state);
  void slotChangeLabel(QString array);

  void slotEnableRadialLayout(int state);
  void slotChangeRadialLayout(int angle);

  void slotEnableLog(int state);
  void slotChangeLog(double angle);

  void slotEnableBackPlane(int state);
};

#endif // lineageViewer_H
