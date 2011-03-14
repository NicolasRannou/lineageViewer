#include <QApplication>

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
  //graph->GetEdgeData()->AddArray(cellType);

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

  vtkSmartPointer<vtkDoubleArray> xPos =
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
  graph->GetVertexData()->AddArray(xPos);

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
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lineageViewer::~lineageViewer()
{
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
	  this->m_treeGraphView->ResetCamera();
	  this->m_treeGraphView->Render();
 }
 //----------------------------------------------------------------------------

 //----------------------------------------------------------------------------
 void lineageViewer::slotChangeScale(QString array)
 {
 this->m_treeLayoutStrategy->SetDistanceArrayName(array.toLocal8Bit().data());

 //update visu
   this->m_treeGraphView->ResetCamera();
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
