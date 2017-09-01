#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>

#include <vtkOutlineFilter.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkOculusRenderWindow.h>
#include <vtkOculusRenderer.h>
#include <vtkOculusRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include "vtkFitsReader.h"

#ifdef main
#undef main
#endif

int main() 
{

	vtkSmartPointer<vtkOculusRenderer> renderer =
	  vtkSmartPointer<vtkOculusRenderer>::New();

	renderer->SetBackground(.3, .6, .3); // Background color green
	vtkSmartPointer<vtkOculusRenderWindow> renderWindow =
		vtkSmartPointer<vtkOculusRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkOculusRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkOculusRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderWindow->Render();

	// vtk pipeline
	vtkFitsReader *fitsReader = vtkFitsReader::New();
	fitsReader->SetFileName("D:/project/vtk-fits/data/OMC.fits");
	//vtkStructuredPointsReader *fitsReader = vtkStructuredPointsReader::New();
	//fitsReader->SetFileName("D:/sdevk__/vtkData/Data/ironProt.vtk");
	fitsReader->Update();

	vtkSmartPointer<vtkImageDataGeometryFilter> geometryFilter =
	  vtkSmartPointer<vtkImageDataGeometryFilter>::New();
	geometryFilter->SetInputConnection(fitsReader->GetOutputPort());
	geometryFilter->Update();

	vtkSmartPointer<vtkOutlineFilter> outlineFilter =
	  vtkSmartPointer<vtkOutlineFilter>::New();
	outlineFilter->SetInputConnection(fitsReader->GetOutputPort());
	outlineFilter->Update();

	// Visualize
	vtkSmartPointer<vtkPolyDataMapper> ofMapper =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
	ofMapper->SetInputConnection(outlineFilter->GetOutputPort());
	ofMapper->ScalarVisibilityOff();

	vtkSmartPointer<vtkActor> ofActor =
	  vtkSmartPointer<vtkActor>::New();
	ofActor->SetMapper(ofMapper);
	ofActor->GetProperty()->SetColor(0, 0, 0);

	renderer->AddActor(ofActor);
	renderWindow->Render();

	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper =
		vtkSmartPointer<vtkSmartVolumeMapper>::New();
	volumeMapper->SetBlendModeToComposite();
	volumeMapper->SetInputConnection(fitsReader->GetOutputPort());

	vtkSmartPointer<vtkVolumeProperty> volumeProperty = 
		vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->ShadeOff();
	volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity =
		vtkSmartPointer<vtkPiecewiseFunction>::New();
	compositeOpacity->AddPoint(0.0, 0.0);
	compositeOpacity->AddPoint(80.0, 1.0);
	compositeOpacity->AddPoint(80.1, 0.0);
	compositeOpacity->AddPoint(255.0, 0.0);
	volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.

	vtkSmartPointer<vtkColorTransferFunction> color =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	color->AddRGBPoint(0.0, 0.0, 0.0, 1.0);
	color->AddRGBPoint(40.0, 1.0, 0.0, 0.0);
	color->AddRGBPoint(255.0, 1.0, 1.0, 1.0);
	volumeProperty->SetColor(color);

	vtkSmartPointer<vtkVolume> volume =
		vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	renderer->AddViewProp(volume);
	renderer->ResetCamera();

	renderWindow->SetMultiSamples(0);
	renderWindow->Render();

	renderWindowInteractor->Start();
	return EXIT_SUCCESS;
}
