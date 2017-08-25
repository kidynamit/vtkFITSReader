#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>

#include <vtkOutlineFilter.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include "vtkFitsReader.h"

int main() 
{

	vtkSmartPointer<vtkRenderer> renderer =
	  vtkSmartPointer<vtkRenderer>::New();

	renderer->SetBackground(.3, .6, .3); // Background color green
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderWindow->Render();

	// vtk pipeline
	vtkFitsReader *fitsReader = vtkFitsReader::New();
	fitsReader->SetFileName("D:/project/vtk-fits/data/OMC.fits");
	//vtkStructuredPointsReader *fitsReader = vtkStructuredPointsReader::New();
	//fitsReader->SetFileName("D:/sdevk__/vtkData/Data/ironProt.vtk");
	fitsReader->Update();

#ifndef VIEW_VOLUME_OUTLINE
#define VIEW_VOLUME_OUTLINE
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
#endif 

//#ifdef VIEW_VOLUME_OUTLINE
//#ifndef VIEW_VOLUMETRIC_RAYCASTING
//#define VIEW_VOLUMETRIC_RAYCASTING
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

	vtkSmartPointer<KeyPressInteractorStyle> style =
		vtkSmartPointer<KeyPressInteractorStyle>::New();
	renderWindowInteractor->SetInteractorStyle(style);
	style->SetCurrentRenderer(renderer);

	renderer->AddViewProp(volume);
	renderer->ResetCamera();

	renderWindow->SetMultiSamples(0);
	renderWindow->Render();

//#endif
//#endif

	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
