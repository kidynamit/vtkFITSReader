#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkRenderWindow.h>
#include <vtkMarchingCubes.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include "vtkFitsReader.h"

int main() {

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
  fitsReader->SetFileName("D:/project/Masters/vtkFitsReader/data/OMC.fits");
  fitsReader->Update();

  // outline
  vtkSmartPointer<vtkOutlineFilter> outlineF = 
	  vtkSmartPointer<vtkOutlineFilter>::New();
  outlineF->SetInputData(fitsReader->GetOutput());

  vtkSmartPointer<vtkPolyDataMapper> outlineM =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
  outlineM->SetInputData(outlineF->GetOutput());
  outlineM->ScalarVisibilityOff();

  vtkSmartPointer<vtkActor> outlineA =
	  vtkSmartPointer<vtkActor>::New();
  outlineA->SetMapper(outlineM);
//outlineA->GetProperty()->SetColor(0.0, 0.0, 0.0);

  // isosurface
  vtkSmartPointer<vtkMarchingCubes> shellE = 
	  vtkSmartPointer<vtkMarchingCubes>::New();
  shellE->SetInputData(fitsReader->GetOutput());
  shellE->SetValue(0, 10.0f);

  // decimatevoid 
//vtkDecimate *shellD = vtkDecimate::New();
//shellD->SetInputData(shellE->GetOutput());
//shellD->SetTargetReduction(0.7);

  vtkSmartPointer<vtkPolyDataMapper> shellM =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
  shellM->SetInputData(shellE->GetOutput());
//shellM->SetInputData(shellD->GetOutput());
  shellM->ScalarVisibilityOff();

  vtkSmartPointer<vtkActor> shellA =
	  vtkSmartPointer<vtkActor>::New();
  shellA->SetMapper(shellM);
  shellA->GetProperty()->SetColor(0.5, 0.5, 1.0);

  // slice
  vtkImageDataGeometryFilter *sliceE =
               vtkImageDataGeometryFilter::New();
  // values are clamped
  sliceE->SetExtent(0, 5000, 0, 5000, 13, 13);
  sliceE->SetInputData(fitsReader->GetOutput());

  vtkSmartPointer<vtkPolyDataMapper> sliceM = 
	  vtkSmartPointer<vtkPolyDataMapper>::New();
  sliceM->SetInputData(sliceE->GetOutput());
  sliceM->ScalarVisibilityOn();
  double * range = fitsReader->GetOutput()->GetScalarRange();
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

  return 0;
}
