#ifndef lineageViewer_H
#define lineageViewer_H

#include <QMainWindow>

#include "vtkSmartPointer.h"

// Forward Qt class declarations
class Ui_lineageViewer;
//table view
class vtkQtTreeView;
//graph view
class vtkGraphLayoutView;
class vtkTreeLayoutStrategy;
// connection between graph and table
class vtkAnnotationLink;
class vtkEventQtSlotConnect;

class vtkObject;

class vtkLookupTable;

// back plane
class vtkDelaunay2D;
class vtkGraphToPolyData;
class vtkPolyDataMapper;
class vtkActor;

class lineageViewer : public QMainWindow
{
  Q_OBJECT

public:

  // Constructor/Destructor
  lineageViewer( QWidget * iParent = 0, Qt::WindowFlags iFlags = 0 );
  ~lineageViewer();

public slots:
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

protected:

protected slots:

  // Description:
  // Called when selection changed in the Qt tree view
  void selectionChanged(vtkObject*, unsigned long,void* ,void* );

private:
  vtkSmartPointer<vtkQtTreeView>         m_treeTableView;
  vtkSmartPointer<vtkGraphLayoutView>    m_treeGraphView;
  vtkSmartPointer<vtkAnnotationLink>     m_annotationLink;
  vtkSmartPointer<vtkEventQtSlotConnect> m_connect;
  vtkSmartPointer<vtkLookupTable>        m_lut;
  vtkSmartPointer<vtkTreeLayoutStrategy> m_treeLayoutStrategy;
  vtkSmartPointer<vtkDelaunay2D>         m_backPlane;
  vtkSmartPointer<vtkGraphToPolyData>    m_graphToPolyData;
  vtkSmartPointer<vtkPolyDataMapper>     m_planeMapper;
  vtkSmartPointer<vtkActor>              m_planeActor;

  // Designer form
  Ui_lineageViewer *ui;
};

#endif // lineageViewer_H
