#include <QApplication>
/*#include <QFileDialog>
#include <QHeaderView>
#include <QInputDialog>
#include <QListView>
#include <QPushButton>
#include <QString>
#include <QStandardItem>
#include <QStandardItemModel>
*/
#include "ui_lineageViewer.h"
#include "lineageViewer.h"

// QT  general
#include <QGridLayout>

// tab view
#include <vtkQtTreeView.h>

// graph view
#include "vtkGraphLayoutView.h"
#include "vtkRenderWindow.h"
#include "vtkTreeLayoutStrategy.h"

// create the tree
#include "vtkMutableDirectedGraph.h"
#include "vtkStringArray.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkTree.h"

#include "vtkDataSetAttributes.h"

#include "vtkDataRepresentation.h"

//connect table and graph
#include "vtkAnnotationLink.h"
#include <vtkEventQtSlotConnect.h>

#include "vtkLookupTable.h"
#include "vtkViewTheme.h"

#include "vtkSelectionNode.h"
#include "vtkIdTypeArray.h"
#include "vtkSelection.h"

// back plane
#include "vtkDelaunay2D.h"
#include "vtkGraphToPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRendererCollection.h"
/*
#include <vtkAlgorithmOutput.h>
#include <vtkAnnotationLink.h>
#include <vtkConvertSelection.h>
#include <vtkDataRepresentation.h>
#include <vtkDelimitedTextReader.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkGraph.h>
#include <vtkIdTypeArray.h>
#include <vtkLineageView.h>
#include <vtkOutEdgeIterator.h>
#include <vtkPointData.h>

#include <vtkSelectionNode.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToGraph.h>
#include <vtkTableWriter.h>
#include <vtkTree.h>
#include <vtkTreeReader.h>
#include <vtkQtTreeModelAdapter.h>
#include <vtkVariant.h>
#include <vtkViewTheme.h>

#include <vector>
*/

/*
//----------------------------------------------------------------------------
class CellLineageUpdater : public vtkCommand
{
public:
  static CellLineageUpdater* New()
  { return new CellLineageUpdater; }

  void AddView(vtkView* view)
  {
    this->Views.push_back(view);
    view->AddObserver(vtkCommand::SelectionChangedEvent, this);
  }

  virtual void Execute(vtkObject* , unsigned long , void*)
  {
    std::vector< vtkView* >::iterator it = Views.begin();
    std::vector< vtkView* >::iterator end = Views.end();

    while( it != end )
      {
      (*it)->Update();
      ++it;
      }
  }
private:
  CellLineageUpdater() { }
  ~CellLineageUpdater() { }
  std::vector<vtkView*> Views;
};
//----------------------------------------------------------------------------
*/
//----------------------------------------------------------------------------
// Constructor
lineageViewer::
lineageViewer( QWidget* iParent, Qt::WindowFlags iFlags ) :
  QMainWindow( iParent, iFlags )
{
  // Create simple tree
  // should add edges manually for the coloring

  vtkSmartPointer<vtkMutableDirectedGraph> graph =
    vtkSmartPointer<vtkMutableDirectedGraph>::New();
  vtkIdType a = graph->AddVertex();
  vtkIdType b = graph->AddChild(a);
  vtkIdType c = graph->AddChild(a);
  vtkIdType d = graph->AddChild(b);
  vtkIdType e = graph->AddChild(c);
  vtkIdType f = graph->AddChild(c);
  vtkIdType g = graph->AddChild(c);
  vtkIdType h = graph->AddChild(f);
  vtkIdType i = graph->AddChild(f);

  // First array: first column of the graph

  vtkSmartPointer<vtkStringArray> cellType =
      vtkSmartPointer<vtkStringArray>::New();
  cellType->SetName("name");
  cellType->InsertValue(a, "TypeA");
  cellType->InsertValue(b, "TypeB");
  cellType->InsertValue(c, "TypeC");
  cellType->InsertValue(d, "TypeD");
  cellType->InsertValue(e, "TypeE");
  cellType->InsertValue(f, "TypeF");
  cellType->InsertValue(g, "TypeG");
  cellType->InsertValue(h, "TypeH");
  cellType->InsertValue(i, "TypeI");
  graph->GetVertexData()->AddArray(cellType);
  graph->GetEdgeData()->AddArray(cellType);

  vtkSmartPointer<vtkPoints> points =
      vtkSmartPointer<vtkPoints>::New();
  points->InsertNextPoint(0.0, 0.5, 0.0);
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 1.0, 0.0);
  points->InsertNextPoint(0.0, 0.0, 2.0);
  points->InsertNextPoint(0.0, 10.0, 2.0);
  points->InsertNextPoint(10.0, 0.0, 2.0);
  points->InsertNextPoint(1.0, 3.0, 2.0);
  points->InsertNextPoint(1.0, 20.0, 5.0);
  points->InsertNextPoint(30.0, 1.0, 2.0);
  graph->SetPoints(points);

  vtkSmartPointer<vtkDoubleArray> end =
      vtkSmartPointer<vtkDoubleArray>::New();
  end->SetName("EndTime");
  end->InsertValue(a, 10);
  end->InsertValue(b, 15);
  end->InsertValue(c, 16);
  end->InsertValue(d, 25);
  end->InsertValue(e, 27);
  end->InsertValue(f, 28);
  end->InsertValue(g, 30);
  end->InsertValue(h, 43);
  end->InsertValue(i, 37);
  graph->GetVertexData()->AddArray(end);

  //graph->GetEdgeData()->AddArray(end);

 /* vtkSmartPointer<vtkDoubleArray> xPos =
      vtkSmartPointer<vtkDoubleArray>::New();
  xPos->SetName("XPos");
  xPos->InsertValue(a, 113);
  xPos->InsertValue(b, 51);
  xPos->InsertValue(c, 77);
  xPos->InsertValue(d, 98);
  xPos->InsertValue(e, 51);
  xPos->InsertValue(f, 50);
  xPos->InsertValue(g, 70);
  xPos->InsertValue(h, 116);
  xPos->InsertValue(i, 119);
  graph->AddArray(xPos);*/
  //graph->GetEdgeData()->AddArray(xPos);

  this->ui = new Ui_lineageViewer;
  this->ui->setupUi(this);

  // we need a tree as input for the table
  vtkSmartPointer<vtkTree> tree =
    vtkSmartPointer<vtkTree>::New();
  tree->CheckedShallowCopy(graph);

  //Create the table View
  this->m_treeTableView          = vtkSmartPointer<vtkQtTreeView>::New();
  //and add the widget
  QGridLayout* tableLayout = new QGridLayout(this->ui->tableFrame);
  tableLayout->addWidget(this->m_treeTableView->GetWidget());

  this->m_treeTableView->AddRepresentationFromInput(tree);
  this->m_treeTableView->Update();

  //Create the graph View
  this->m_treeGraphView =
    vtkSmartPointer<vtkGraphLayoutView>::New();
  this->m_treeGraphView->AddRepresentationFromInput(graph);
  this->m_treeGraphView->SetLayoutStrategyToTree();
  this->m_treeGraphView->ResetCamera();

  this->m_treeGraphView->SetInteractor(
      this->ui->graphViewWidget->GetInteractor() );
  this->ui->graphViewWidget->SetRenderWindow(
      this->m_treeGraphView->GetRenderWindow() );

  // and the associated LUT
  this->m_lut = vtkSmartPointer<vtkLookupTable>::New();
  this->m_lut->SetHueRange(0.667, 0.0);
  this->m_lut->Build();

  vtkSmartPointer<vtkViewTheme> theme =
    vtkSmartPointer<vtkViewTheme>::New();
  theme->SetPointLookupTable(this->m_lut);
  theme->SetCellLookupTable(this->m_lut);

  this->m_treeGraphView->ApplyViewTheme(theme);

  // add the layout strategy (scale and circular)
  this->m_treeLayoutStrategy    = vtkSmartPointer<vtkTreeLayoutStrategy>::New();
  // radial layout default values
  this->m_treeLayoutStrategy->SetAngle(90);
  this->m_treeLayoutStrategy->SetRadial(false);
  // Log
  this->m_treeLayoutStrategy->SetLogSpacingValue(1);

  this->m_treeGraphView->SetLayoutStrategy(this->m_treeLayoutStrategy);

  // create the back plane
  this->m_backPlane = vtkSmartPointer<vtkDelaunay2D>::New();
  this->m_graphToPolyData = vtkSmartPointer<vtkGraphToPolyData>::New();
  this->m_graphToPolyData->SetInput(graph);
  this->m_graphToPolyData->Update();
  this->m_planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->m_planeActor = vtkSmartPointer<vtkActor>::New();

  // add annotations
  // annotations are ...
  this->m_annotationLink = vtkSmartPointer<vtkAnnotationLink>::New();
  this->m_treeGraphView->GetRepresentation()->SetAnnotationLink(this->m_annotationLink);
  this->m_treeTableView->GetRepresentation()->SetAnnotationLink(this->m_annotationLink);
  //this->m_annotationLink->AddObserver("AnnotationChangedEvent", selectionCallback);

  // connect table and graph
  this->m_connect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  this->m_connect->Connect(this->m_treeTableView->GetRepresentation(),
    vtkCommand::SelectionChangedEvent,
    this, SLOT(selectionChanged(vtkObject*, unsigned long, void*, void*)));
  this->m_connect->Connect(this->m_treeGraphView->GetRepresentation(),
    vtkCommand::SelectionChangedEvent,
    this, SLOT(selectionChanged(vtkObject*, unsigned long, void*, void*)));

  // color coding
  connect(this->ui->colorCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableColorCode(int)));
  connect(this->ui->colorComboBox, SIGNAL(currentIndexChanged(QString)),
    this, SLOT(slotChangeColorCode(QString)));

  // scaling
  connect(this->ui->scaleCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableScale(int)));
  connect(this->ui->scaleComboBox, SIGNAL(currentIndexChanged(QString)),
    this, SLOT(slotChangeScale(QString)));

  // labeling
  connect(this->ui->labelCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableLabel(int)));
  connect(this->ui->labelComboBox, SIGNAL(currentIndexChanged(QString)),
    this, SLOT(slotChangeLabel(QString)));

  // radial rendering
  connect(this->ui->radialCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableRadialLayout(int)));
  connect(this->ui->radialSlider, SIGNAL(valueChanged(int)),
    this, SLOT(slotChangeRadialLayout(int)));

  // log rendering
  connect(this->ui->logCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableLog(int)));
  connect(this->ui->logSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(slotChangeLog(double)));

  // back plane
  connect(this->ui->backCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableBackPlane(int)));

  // Fill combo boxes
  // Update combo boxes (fill content with arrays names)
  // how many fields do we have?
  int numberOfArrays = graph->GetVertexData()->GetNumberOfArrays();
  this->ui->colorComboBox->clear();
  this->ui->scaleComboBox->clear();
  this->ui->labelComboBox->clear();

  // fill comboxes according to the data
  for(int i=0;i<numberOfArrays; i++)
    {
    const char* name =
    		graph->GetVertexData()->GetArrayName(i);
    this->ui->labelComboBox->addItem(name);
    // if data array (i.e. numbers), add it
    if(graph->GetVertexData()->GetArray(name))
      {
      this->ui->colorComboBox->addItem(name);
      this->ui->scaleComboBox->addItem(name);
      }
    }

  this->slotEnableScale(false);

  /*125   Connections->Connect(
   126     this->TableView->GetRepresentation(),
   127     vtkCommand::SelectionChangedEvent,
   128     this,
   129     SLOT(selectionChanged(vtkObject*, unsigned long, void*, void*)));
   */

  /*
  this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

  this->qvtkWidget->GetInteractor()->SetInteractorStyle(m_InteractorStyle3D);
  m_InteractorStyle3D->EnablePickMode();

  this->qvtkWidget->GetRenderWindow()->Render();*/

  // Lineage Viewer needs to get my render window
  //this->LineageView->SetInteractor(this->ui->vtkLineageViewWidget->GetInteractor());
  //this->ui->vtkLineageViewWidget->SetRenderWindow(this->LineageView->GetRenderWindow());


/*
  this->LineageReader       = vtkTreeReader::New();
  this->LineageView         = vtkLineageView::New();
  this->QtTreeView          = vtkQtTreeView::New();
  this->AnnotationLink      = vtkAnnotationLink::New();
  this->Updater             = CellLineageUpdater::New();
  this->Connect             = vtkEventQtSlotConnect::New();

  vtkSmartPointer<vtkViewTheme> theme = vtkSmartPointer<vtkViewTheme>::New();
  theme->SetBackgroundColor(1.0, 1.0, 1.0);
  theme->SetBackgroundColor2(1.0, 1.0, 1.0);

  this->LineageView->ApplyViewTheme(theme);

  this->Updater->AddView(this->LineageView);
  this->Updater->AddView(this->QtTreeView);

  this->ui->treeTextView->layout()->addWidget(this->QtTreeView->GetWidget());

  // Lineage Viewer needs to get my render window
  this->LineageView->SetInteractor(this->ui->vtkLineageViewWidget->GetInteractor());
  this->ui->vtkLineageViewWidget->SetRenderWindow(this->LineageView->GetRenderWindow());

  // Lineage view parameters
  connect(this->ui->collapseModeCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotSetCollapseMode(int)));
  connect(this->ui->labelsCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotSetLabels(int)));
  connect(this->ui->radialLayoutCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotSetRadialLayout(int)));
  connect(this->ui->radialLayoutAngleSpinBox, SIGNAL(valueChanged(int)),
    this, SLOT(slotSetRadialAngle(int)));
  connect(this->ui->logSpacingSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(slotSetLogSpacingFactor(double)));
  connect(this->ui->backPlaneCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotSetBackPlane(int)));
  connect(this->ui->isoContourCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotSetIsoContour(int)));
  connect(this->ui->elbowCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotSetElbow(int)));
  connect(this->ui->elbowAngleSlider, SIGNAL(valueChanged(int)),
    this, SLOT(slotSetElbowAngle(int)));
  // new slots
  //for scaling - should turn it on by default?
  connect(this->ui->scaleBy, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableScale(int)));
  connect(this->ui->scaleType, SIGNAL(currentIndexChanged(QString)),
    this, SLOT(slotChangeScale(QString)));
  // color coding
  connect(this->ui->colorCodeBy, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableColorCode(int)));
  connect(this->ui->colorCodeType, SIGNAL(currentIndexChanged(QString)),
    this, SLOT(slotChangeColorCode(QString)));
  // labels
  connect(this->ui->labelType, SIGNAL(currentIndexChanged(QString)),
    this, SLOT(slotChangeLabel(QString)));
  // Time controls
  this->ui->timeSlider->setMinimum(0);
  this->ui->timeSlider->setMaximum(100);
  connect(this->ui->timeSlider, SIGNAL(valueChanged(int)),
    this, SLOT(slotSetGlobalTimeValue(int)));
  connect(this->ui->timeSlider, SIGNAL(sliderMoved(int)),
    this, SLOT(slotGlobalTimeValueChanging(int)));

  // Application signals and slots
  connect(this->ui->actionOpenLineageFile, SIGNAL(triggered()), this, SLOT(slotOpenLineageData()));
  connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
*/

}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lineageViewer::~lineageViewer()
{
/*  this->LineageReader->Delete();
  this->LineageView->Delete();
  this->QtTreeView->Delete();
  this->AnnotationLink->Delete();
  this->Updater->Delete();
  this->Connect->Delete();
*/
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

 // This defines the QT slot. They way it works is first get the vtkSelection,
 // push it to the default vtkAnnotationLink associated with each
 // vtkDataRepresentation of each view type and then call Update or
 // Render (if it is a vtkRenderView) on each view.
 void lineageViewer::selectionChanged(vtkObject*,
                                       unsigned long,
                                       void* vtkNotUsed(clientData),
                                       void* callData)
 {
   vtkSelection* selection = reinterpret_cast<vtkSelection*>(callData);

   if(selection)
   {
  	 // Create a new selection without edges
  	 vtkSelection* s = vtkSelection::New();

  	 vtkSelectionNode* vertices = NULL;
  	   	     vtkSelectionNode* edges = NULL;

  	 int numberOfNodes = selection->GetNumberOfNodes();

  	 for (int i = 0; i < numberOfNodes; i++)
  	 {
	     if(selection->GetNode(i)->GetFieldType() == vtkSelectionNode::VERTEX)
	       {
	       vertices = selection->GetNode(i);
	       s->AddNode(selection->GetNode(i));
	       std::cout << "VERTEX SELECTED" << std::endl;
	       }
	     else if(selection->GetNode(i)->GetFieldType() == vtkSelectionNode::EDGE)
	       {
	       edges = selection->GetNode(i);
	       std::cout << "EDGES SELECTED" << std::endl;
	       }
  	 }
/*
  	 vtkSelectionNode* vertices;
  	     vtkSelectionNode* edges;
  	     if(selection->GetNode(0)->GetFieldType() == vtkSelectionNode::VERTEX)
  	       {
  	       vertices = selection->GetNode(0);
  	       s->AddNode(vertices);
  	       }
  	     else if(selection->GetNode(0)->GetFieldType() == vtkSelectionNode::EDGE)
  	       {
  	       edges = selection->GetNode(0);
  	       }

  	     if(selection->GetNode(1)->GetFieldType() == vtkSelectionNode::VERTEX)
  	       {
  	       vertices = selection->GetNode(1);
  	       s->AddNode(vertices);
  	       }
  	     else if(selection->GetNode(1)->GetFieldType() == vtkSelectionNode::EDGE)
  	       {
  	       edges = selection->GetNode(1);
  	       }
*//*
     if(vertices)
     {
  	     vtkIdTypeArray* vertexList = vtkIdTypeArray::SafeDownCast(vertices->GetSelectionList());
  	     std::cout << "There are " << vertexList->GetNumberOfTuples() << " vertices selected." << std::endl;

  	     if(vertexList->GetNumberOfTuples() > 0)
  	       {
  	       std::cout << "Vertex Ids: ";
  	       }
  	     for(vtkIdType i = 0; i < vertexList->GetNumberOfTuples(); i++)
  	       {
  	       std::cout << vertexList->GetValue(i) << " ";
  	       }

  	     std::cout << std::endl;

     }
  	     if(edges)
  	     {
  	     vtkIdTypeArray* edgeList = vtkIdTypeArray::SafeDownCast(edges->GetSelectionList());
  	     std::cout << "There are " << edgeList->GetNumberOfTuples() << " edges selected." << std::endl;
  	     if(edgeList->GetNumberOfTuples() > 0)
  	       {
  	       std::cout << "Edge Ids: ";
  	       }

  	     for(vtkIdType i = 0; i < edgeList->GetNumberOfTuples(); i++)
  	       {
  	       std::cout << edgeList->GetValue(i) << " ";
  	       }
  	     std::cout << std::endl;
  	     }
  	     // should add edges somewhere here...
  	     this->m_treeTableView->GetRepresentation()->GetAnnotationLink()->
  	       SetCurrentSelection(s);
  	     this->m_treeGraphView->GetRepresentation()->GetAnnotationLink()->
  	       SetCurrentSelection(selection);
*/
  	     this->m_treeTableView->Update();
  	     this->m_treeGraphView->Render();
   }
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotEnableColorCode(int state)
 {
	 /*
	  * \todo shouldnt we define array to display somewhere?
	  */
   this->m_treeGraphView->SetColorVertices(state);
   this->m_treeGraphView->SetColorEdges(state);

   //update visu
   this->m_treeGraphView->Render();
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotChangeColorCode(QString array)
 {
 this->m_treeGraphView->SetVertexColorArrayName(array.toLocal8Bit().data());
 this->m_treeGraphView->SetEdgeColorArrayName(array.toLocal8Bit().data());

 //update visu
   this->m_treeGraphView->Render();
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotEnableScale(int state)
 {
	  //scale
	  this->m_treeLayoutStrategy->SetDistanceArrayName
	  (state ? this->ui->scaleComboBox->currentText().toLocal8Bit().data() : NULL);

	  //update visu
	  this->m_treeGraphView->Render();
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotChangeScale(QString array)
 {
 this->m_treeLayoutStrategy->SetDistanceArrayName(array.toLocal8Bit().data());

 //update visu
   this->m_treeGraphView->Render();
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotEnableLabel(int state)
 {
	  //scale
	  this->m_treeGraphView->SetVertexLabelVisibility(state);

	  //update visu
	  this->m_treeGraphView->Render();
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotChangeLabel(QString array)
 {
 this->m_treeGraphView->SetVertexLabelArrayName(array.toLocal8Bit().data());

 //update visu
   this->m_treeGraphView->Render();
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotEnableRadialLayout(int state)
 {
	 if(!state)
	 {
		 this->m_treeLayoutStrategy->SetAngle(90);
	 }
	 else
	 {
		 this->m_treeLayoutStrategy->SetAngle( this->ui->radialSlider->value() );
	 }

	  //radial layout
	 this->m_treeLayoutStrategy->SetRadial(state);

	  //update visu
	  this->m_treeGraphView->ResetCamera();
	  this->m_treeGraphView->Render();
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotChangeRadialLayout(int angle)
 {
	 if(this->ui->radialCheckBox->isChecked() )
	 {
	 // change the layout angle
	 this->m_treeLayoutStrategy->SetAngle( angle );

	  //update visu
	  this->m_treeGraphView->ResetCamera();
	  this->m_treeGraphView->Render();
	 }
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotEnableLog(int state)
 {
	 if(!state)
	 {
		 this->m_treeLayoutStrategy->SetLogSpacingValue(1);
	 }
	 else
	 {
		 this->m_treeLayoutStrategy->SetLogSpacingValue( this->ui->logSpinBox->value() );
	 }

	  //update visu
	  this->m_treeGraphView->ResetCamera();
	  this->m_treeGraphView->Render();
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotChangeLog(double angle)
 {
	 if(this->ui->logCheckBox->isChecked() )
	 {
	 // change the layout angle
	 this->m_treeLayoutStrategy->SetLogSpacingValue( angle );

	  //update visu
	  this->m_treeGraphView->ResetCamera();
	  this->m_treeGraphView->Render();
	 }
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotEnableBackPlane(int state)
 {
	  if(state)
	  {
	  	//this->m_backPlane->SetInput(
	  	//		this->m_graphToPolyData->GetOutput());
	  	this->m_planeMapper->SetInput(
	  			this->m_graphToPolyData->GetOutput());
	  	this->m_planeActor->SetMapper(this->m_planeMapper);
	  	this->ui->graphViewWidget->GetRenderWindow()->GetRenderers()
	  			->GetFirstRenderer()->AddActor(this->m_planeActor);
	  }
	  else
	  {
	  	this->ui->graphViewWidget->GetRenderWindow()->GetRenderers()
	  			->GetFirstRenderer()->RemoveActor(this->m_planeActor);
	  }

	  //update visu
	  this->m_treeGraphView->Render();
 }
 //----------------------------------------------------------------------------
/*
 *
//----------------------------------------------------------------------------
// Description:
// Set the global time value for all views
void CellLineage::slotGlobalTimeValueChanging(int value)
{
  // Set the value of the slider and line edit.
  this->ui->timeSlider->setValue(value);

  // Have the lineage view update itself
  this->LineageView->SetCurrentTime(value);
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Description:
// Set the global time value for all views
void CellLineage::slotSetGlobalTimeValue(int value)
{
  // Set the value of the slider and line edit.
  this->ui->timeSlider->setValue(value);

  // Have the lineage view update itself
  this->LineageView->SetCurrentTime(value);
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Description:
// Set mouse mode
void CellLineage::slotSetCollapseMode(int on)
{
  if (on)
    {
    this->LineageView->SetSelectMode(vtkLineageView::COLLAPSE_MODE);
    }
  else
    {
    this->LineageView->SetSelectMode(vtkLineageView::SELECT_MODE);
    }
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Description:
// Set labels on/off
void CellLineage::slotSetLabels(int on)
{
  if (on)
    {
    this->LineageView->SetLabelsOn();
    }
  else
    {
    this->LineageView->SetLabelsOff();
    }
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Description:
// Set whether to use radial layout.
void CellLineage::slotSetRadialLayout(int radial)
{
  if ( !radial )
    {
    if ( this->LineageView->GetRadialAngle() > 179 )
      {
      this->LineageView->BlockUpdateOn();
      this->ui->radialLayoutAngleSpinBox->setValue(120);
      this->LineageView->BlockUpdateOff();
      }
    this->ui->radialLayoutAngleSpinBox->setMinimum(30);
    this->ui->radialLayoutAngleSpinBox->setMaximum(179);
    }
  else
    {
    this->ui->radialLayoutAngleSpinBox->setMinimum(90);
    this->ui->radialLayoutAngleSpinBox->setMaximum(360);
    }
  this->LineageView->SetRadialLayout(radial);
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Description:
// Set the radial layout angle.
void CellLineage::slotSetRadialAngle(int angle)
{
  this->LineageView->SetRadialAngle(angle);
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Description:
// Set the log spacing for the layout.
void CellLineage::slotSetLogSpacingFactor(double spacing)
{
  this->LineageView->SetLogSpacingFactor(spacing);
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Description:
// Set whether to set the back plane
void CellLineage::slotSetBackPlane(int state)
{
  this->LineageView->SetBackPlane(state);
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Description:
// Set whether to set the iso contour
void CellLineage::slotSetIsoContour(int state)
{
  this->LineageView->SetIsoContour(state);
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Action to be taken upon lineage file open
void CellLineage::slotOpenLineageData()
{
  // Browse for and read the lineage data
  if (this->readLineageData())
    {
    return;
    }

  // Set up the lineage view of this data
  this->LineageView->AddRepresentationFromInputConnection(
    this->LineageReader->GetOutputPort());
  this->LineageView->SetLayoutStrategyToTree();
  this->LineageView->ResetCamera();
  this->LineageView->Render();

  this->QtTreeView->AddRepresentationFromInputConnection(
    this->LineageReader->GetOutputPort());
  this->QtTreeView->Update();

  // Set up the text view of the lineage data
  this->setUpLineageListView();

  this->LineageView->GetRepresentation()->SetAnnotationLink(this->AnnotationLink);
  this->QtTreeView->GetRepresentation()->SetAnnotationLink(this->AnnotationLink);

  // Link Qt tree selection to the linkage viewer selection
  this->Connect->Connect(this->QtTreeView->GetRepresentation(),
    vtkCommand::SelectionChangedEvent,
    this, SLOT(slotSelectionChanged()));

  // Update combo boxes (fill content with arrays names)
  // how many fields do we have?
  int numberOfArrays = LineageReader->GetOutput()->GetVertexData()->GetNumberOfArrays();
  this->ui->scaleType->clear();
  this->ui->colorCodeType->clear();
  this->ui->labelType->clear();

  // fill comboxes according to the data
  for(int i=0;i<numberOfArrays; i++)
    {
    const char* name =
        LineageReader->GetOutput()->GetVertexData()->GetArrayName(i);
    this->ui->labelType->addItem(name);
    // if data array (i.e. numbers), add it
    if(LineageReader->GetOutput()->GetVertexData()->GetArray(name))
      {
      this->ui->scaleType->addItem(name);
      this->ui->colorCodeType->addItem(name);
      }
    }

  // set the active scalar, update mappers and LUTs
  char* activeScalar = this->ui->colorCodeType->currentText().toLocal8Bit().data();

  //vertex: node (small square)
  //this->LineageView->SetVertexColorFieldName(activeScalar);
  //this->LineageView->SetEdgeColorFieldName(activeScalar);

  // update time slider for the iso contours
  double* range =
      LineageReader->GetOutput()->GetVertexData()->GetArray(activeScalar)->GetRange();
  this->ui->timeSlider->setMinimum(range[0]);
  this->ui->timeSlider->setMaximum(range[1]);
  this->ui->timeSlider->setValue(range[0]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CellLineage::slotEnableScale(int state)
{
  //scale
  this->LineageView->SetDistanceArrayName
  (state ? this->ui->scaleType->currentText().toLocal8Bit().data() : NULL);

  //update visu
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CellLineage::slotChangeScale(QString array)
{
  if(!LineageReader->GetOutput()->GetVertexData()->GetArray(array.toLocal8Bit().data()))
    {
    return;
    }

  //scale
  this->LineageView->SetDistanceArrayName
  (this->ui->scaleBy->isChecked() ? array.toLocal8Bit().data() : NULL);

  //update visu
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CellLineage::slotEnableColorCode(int state)
{
  // update visibility
  this->LineageView->SetEdgeScalarVisibility(state);

  //update visu
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CellLineage::slotChangeColorCode(QString array)
{
  if(!LineageReader->GetOutput()->GetVertexData()->GetArray(array.toLocal8Bit().data()))
    {
    return;
    }

 this->LineageView->SetVertexColorFieldName(array.toLocal8Bit().data());
 this->LineageView->SetEdgeColorFieldName(array.toLocal8Bit().data());

  // update time slider for the iso contours - maybe not here
  double* range =
      LineageReader->GetOutput()->GetVertexData()->GetArray(array.toLocal8Bit().data())->GetRange();
  this->ui->timeSlider->setMinimum(range[0]);
  this->ui->timeSlider->setMaximum(range[1]);
  this->ui->timeSlider->setValue(range[0]);

  //update visu
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CellLineage::slotChangeLabel(QString array)
{
  this->LineageView->SetLabelFieldName(array.toLocal8Bit().data());

  //update visu
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CellLineage::slotSetElbow(int state)
{
  this->LineageView->SetElbow(state?1:0);
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CellLineage::slotSetElbowAngle(int value)
{
  this->LineageView->SetElbowAngle(static_cast<double>(value)/100.0);
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CellLineage::slotSelectionChanged()
{
  this->LineageView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Set up the lineage list view of the data
void CellLineage::setUpLineageListView()
{
  // Expand all
  this->QtTreeView->ExpandAll();

  // Now resize the first column to fit it's contents
  this->QtTreeView->ResizeColumnToContents(0);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Browse for and read in the lineage data
int CellLineage::readLineageData()
{
  QDir dir;

  // Open the lineage data file
  QString fileName = QFileDialog::getOpenFileName(
    this,
    "Select the lineage tree file",
    QDir::homePath(),
    "VTK Tree Files (*.vtk);;All Files (*.*)");

  if (fileName.isNull())
    {
    return -1;
    }

  // Create lineage reader
  this->LineageReader->SetFileName( fileName.toAscii() );
  this->LineageReader->Update();
  return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CellLineage::slotExit()
{
  qApp->exit();
}*/
