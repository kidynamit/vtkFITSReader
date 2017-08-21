#include "vtkFitsReader.h"

#include "ieeefp.h"
#include <vtkDataArray.h>
#include <vtkPointSet.h>
#include <vtkByteSwap.h>
#include <vtkFloatArray.h>
#include <vtkErrorCode.h>
#include <vtkFieldData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkStreamingDemandDrivenPipeline.h>

vtkFitsReader::vtkFitsReader() 
{
	vtkStructuredGrid *output = vtkStructuredGrid::New();
	this->SetOutput(output);
	// Releasing data for pipeline parallism.
	// Filters will know it is empty.
	output->ReleaseData();
	output->Delete();
	this->pFitsFile = nullptr;
}

vtkFitsReader::~vtkFitsReader()
{
	delete this->pFitsFile;
}

// Note: from cookbook.c in fitsio distribution.
void vtkFitsReader::PrintError(int status) {

    cerr << "vtkFitsReader ERROR.";
    if (status) {
       fits_report_error(stderr, status); /* print error report */
       exit( status );    /* terminate the program, returning error status */
    }
    return;
}

//----------------------------------------------------------------------------

int vtkFitsReader::RequestData(
	vtkInformation *,
	vtkInformationVector **,
	vtkInformationVector *outputVector)
{

	cerr << "RequestData" << std::endl;
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	this->SetErrorCode(vtkErrorCode::NoError);
	vtkIdType numPts = 0, numCells = 0;
	int dimsRead = 0, arRead = 0, originRead = 0;
	vtkStructuredGrid *output = vtkStructuredGrid::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	int status = 0, nfound = 0, anynull = 0;
	long naxes[3], fpixel, nbuffer, npixels;
	const int buffsize = 1000;

	float datamin, datamax, nullval, buffer[buffsize];
	// ImageSource superclass does not do this.
	output->ReleaseData();

	vtkDebugMacro(<< "Reading vtk structured points file...");

	npixels = naxes[0] * naxes[1] * naxes[2]; /* num of pixels in the image */
	fpixel = 1;
	nullval = 0;                /* don't check for null values in the image */

	output->SetDimensions(4, 4, 4);


	
	this->ReadPoints(output, 64);

	this->pFitsFile = nullptr;
	cerr << "done." << endl;

	return 1;
}



int vtkFitsReader::ReadPoints(vtkPointSet * pointSet, vtkIdType numPoints)
{
	vtkIdType numComp = 3;
	vtkAbstractArray * array = vtkFloatArray::New();
	array->SetNumberOfComponents(numComp);	
	float *ptr = ((vtkFloatArray *)array)->WritePointer(0, numPoints*numComp);
	for (int i = 0; i < numPoints; i++)
		for (int j = 0; j < numComp; j++)
			ptr[i*numComp + j] = (float)rand() / (float)(RAND_MAX);
	
	vtkDataArray * data = vtkArrayDownCast<vtkDataArray>(array);
	vtkPoints * points = vtkPoints::New();
	points->SetData(data);
	data->Delete();
	pointSet->SetPoints(points);
	points->Delete();
	return 1;
}

void vtkFitsReader::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	os << indent << "FITS File Name: " << (this->FileName) << "\n";
}