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

  // vtk pipeline
  vtkFitsReader *fitsReader = vtkFitsReader::New();
  fitsReader->SetFileName("D:/project/vtk-fits/data/OMC.fits");
  fitsReader->Update();

  vtkSmartPointer<vtkImageDataGeometryFilter> geometryFilter =
	  vtkSmartPointer<vtkImageDataGeometryFilter>::New();
  geometryFilter->SetInputConnection(fitsReader->GetOutputPort());
  geometryFilter->Update();

  // Visualize
  vtkSmartPointer<vtkPolyDataMapper> mapper =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(geometryFilter->GetOutputPort());

  vtkSmartPointer<vtkActor> actor =
	  vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  vtkSmartPointer<vtkRenderer> renderer =
	  vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
	  vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
	  vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(.3, .6, .3); // Background color green

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
