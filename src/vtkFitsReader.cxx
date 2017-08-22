#include "vtkFitsReader.h"

#include "ieeefp.h"
#include <vtkDataArray.h>
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

vtkFitsReader::vtkFitsReader() {
	vtkStructuredPoints *output = vtkStructuredPoints::New();
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
	vtkStructuredPoints *output = vtkStructuredPoints::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	int status = 0, nfound = 0, anynull = 0;
	long naxes[3], fpixel, npixels;
	const int buffsize = 1000;

	// ImageSource superclass does not do this.
	output->ReleaseData();

	vtkDebugMacro(<< "Reading vtk structured points file...");
	if (fits_open_file(&this->pFitsFile, this->FileName, READONLY, &status))
	{
		PrintError(status);
	}
	/* read the NAXIS1 and NAXIS2 keyword to get image size */
	if (fits_read_keys_lng(this->pFitsFile, "NAXIS", 1, 3, naxes, &nfound, &status))
	{
		PrintError(status);
	}
	npixels = naxes[0] * naxes[1] * naxes[2]; /* num of pixels in the image */
	fpixel = 1;
	int dim [3] = { naxes[0], naxes[1], naxes[2] };
	output->SetDimensions(dim);
	output->SetOrigin(0.0, 0.0, 0.0);
	output->SetSpacing(1.0, 1.0, 1.0);
	this->SetScalarLut("default"); //may be "default"

	this->ReadScalarData(output, npixels);

	if (fits_close_file(this->pFitsFile, &status))
		PrintError(status);
	this->pFitsFile = nullptr;
	cerr << "done." << endl;

	return 1;
}

int vtkFitsReader::ReadScalarData(vtkDataSet * dataSet, vtkIdType numPts)
{
	vtkIdType numComp = 1;
	vtkDataSetAttributes * attrib = dataSet->GetPointData();
	vtkAbstractArray * array;
	array = vtkFloatArray::New();
	array->SetNumberOfComponents(numComp);
	float *ptr = ((vtkFloatArray *)array)->WritePointer(0, numPts*numComp);
	size_t idx = 0;
	int status = 0;
	long fpixel = 1, nbuffer;
	float buffer[IOBUFLEN];
	float nullval;
	int anynull = 0;

	vtkIdType npixels = numPts;
	while (npixels > 0) 
	{
		nbuffer = npixels;
		if (npixels > IOBUFLEN)
			nbuffer = IOBUFLEN;

		if (fits_read_img(this->pFitsFile, TFLOAT, fpixel, nbuffer, &nullval,
			buffer, &anynull, &status))
			PrintError(status);

		for (int i = 0; i < nbuffer; i++) 
		{
			if (_isnanf(buffer[i])) buffer[i] = -1000000.0; // hack for now
			ptr[idx++] = buffer[i];
		}

		npixels -= nbuffer;    /* increment remaining number of pixels */
		fpixel += nbuffer;    /* next pixel to be read in image */
	}
	vtkDataArray * data = vtkArrayDownCast<vtkDataArray>(array);

	data->SetName(this->FileName);
	attrib->AddArray(data);
	data->Delete();
	return 1;
}

int vtkFitsReader::ReadMetaData(vtkInformation * outInformation)
{
	vtkStructuredPoints *output = vtkStructuredPoints::SafeDownCast(
		outInformation->Get(vtkDataObject::DATA_OBJECT()));

	int status = 0, nfound = 0, anynull = 0;
	long naxes[3]; 
	const int buffsize = 1000;

	// ImageSource superclass does not do this.
	output->ReleaseData();

	vtkDebugMacro(<< "Reading vtk structured points file...");
	if (fits_open_file(&this->pFitsFile, this->FileName, READONLY, &status))
	{
		PrintError(status);
	}
	/* read the NAXIS1 and NAXIS2 keyword to get image size */
	if (fits_read_keys_lng(this->pFitsFile, "NAXIS", 1, 3, naxes, &nfound, &status))
	{
		PrintError(status);
	}
	outInformation->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
		0, naxes[0] - 1, 0, naxes[1] - 1, 0, naxes[2] - 1);
	double ar[3] = { 1, 1, 1 };
	vtkDataObject::SetPointDataActiveScalarInfo(outInformation, VTK_FLOAT, 1);
	outInformation->Set(vtkDataObject::SPACING(), ar, 3);
	double origin[3] = { 0, 0, 0 };
	outInformation->Set(vtkDataObject::ORIGIN(), origin, 3);

	vtkDataObject::SetPointDataActiveScalarInfo(outInformation, VTK_FLOAT, 1);
	if (fits_close_file(this->pFitsFile, &status))
		PrintError(status);
	this->pFitsFile = nullptr;
	return 1;
}


int vtkFitsReader::RequestInformation(vtkInformation *, vtkInformationVector **,
	vtkInformationVector * outputVector) 
{
	cerr << "RequestInformation" << endl;
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	return this->ReadMetaData(outInfo);
}


void vtkFitsReader::PrintSelf(ostream& os, vtkIndent indent) 
{
	this->Superclass::PrintSelf(os, indent);
	os << indent << "FITS File Name: " << (this->FileName) << "\n";
}
