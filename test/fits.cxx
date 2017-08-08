#include "vtkCommon.h"
#include "vtkGraphics.h"
#include "vtkPatented.h"
#include "vtkFitsReader.h"
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

main() {

  // create a window to render into
  vtkSmartPointer<vtkRenderWindow>renWin = vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkRenderer>ren1 = vtkSmartPointer<vtkRenderer>::New();
//ren1->SetBackground(1.0, 1.0, 1.0);
  renWin->AddRenderer(ren1);
  vtkSmartPointer<vtkRenderWindowInteractor> iren =
	  vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);
  // vtk pipeline
  vtkFitsReader *fitsReader = vtkFitsReader::New();
  fitsReader->SetFileName("OMC.fits");
  fitsReader->Update();

  // outline
  vtkOutlineFilter *outlineF = vtkOutlineFilter::New();
  outlineF->SetInput(fitsReader->GetOutput());

  vtkPolyDataMapper *outlineM = vtkPolyDataMapper::New();
  outlineM->SetInput(outlineF->GetOutput());
  outlineM->ScalarVisibilityOff();

  vtkActor *outlineA = vtkActor::New();
  outlineA->SetMapper(outlineM);
//outlineA->GetProperty()->SetColor(0.0, 0.0, 0.0);

  // isosurface
  vtkMarchingCubes *shellE = vtkMarchingCubes::New();
  shellE->SetInput(fitsReader->GetOutput());
  shellE->SetValue(0, 10.0f);

  // decimate
//vtkDecimate *shellD = vtkDecimate::New();
//shellD->SetInput(shellE->GetOutput());
//shellD->SetTargetReduction(0.7);

  vtkPolyDataMapper *shellM = vtkPolyDataMapper::New();
  shellM->SetInput(shellE->GetOutput());
//shellM->SetInput(shellD->GetOutput());
  shellM->ScalarVisibilityOff();

  vtkActor *shellA = vtkActor::New();
  shellA->SetMapper(shellM);
  shellA->GetProperty()->SetColor(0.5, 0.5, 1.0);

  // slice
  vtkStructuredPointsGeometryFilter *sliceE =
               vtkStructuredPointsGeometryFilter::New();
  // values are clamped
  sliceE->SetExtent(0, 5000, 0, 5000, 13, 13);
  sliceE->SetInput(fitsReader->GetOutput());

  vtkPolyDataMapper *sliceM = vtkPolyDataMapper::New();
  sliceM->SetInput(sliceE->GetOutput());
  sliceM->ScalarVisibilityOn();
  float *range;
  range = fitsReader->GetOutput()->GetScalarRange();
  sliceM->SetScalarRange(range);

  vtkActor *sliceA = vtkActor::New();
  sliceA->SetMapper(sliceM);

  // add actors to renderer
  ren1->AddActor(outlineA);
  ren1->AddActor(shellA);
  ren1->AddActor(sliceA);

  // Render an image; since no lights/cameras specified, created automatically
  renWin->Render();

  // uncomment to write VRML
  //vtkVRMLExporter *vrml = vtkVRMLExporter::New();
  //vrml->SetRenderWindow(renWin);
  //vrml->SetFileName("out.wrl");
  //vrml->Write(); 

  // Begin mouse interaction
  iren->Start();

}
