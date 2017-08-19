#include "vtkFitsReader.h"

#include "ieeefp.h"
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkErrorCode.h>
#include <vtkFieldData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
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
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	this->SetErrorCode(vtkErrorCode::NoError);
	vtkIdType numPts = 0, numCells = 0;
	int dimsRead = 0, arRead = 0, originRead = 0;
	vtkStructuredPoints *output = vtkStructuredPoints::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	int status = 0, nfound = 0, anynull = 0;
	long naxes[3], fpixel, nbuffer, npixels;
	const int buffsize = 1000;

	float datamin, datamax, nullval, buffer[buffsize];
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
	nullval = 0;                /* don't check for null values in the image */
	datamin = 1.0E30;
	datamax = -1.0E30;

	cerr << "\nvtkFitsReader: " << this->FileName<< endl;
	cerr << "Dim: " << naxes[0] << " " << naxes[1] << " " << naxes[2] << endl;
	cerr << "points: " << npixels << endl;
	cerr << "creating vtk structured points dataset..." << endl;

	output->SetDimensions(naxes[0], naxes[1], naxes[2]);
	output->SetOrigin(0.0, 0.0, 0.0);

	// Read structured points specific stuff
	//
	vtkFloatArray* scalars = vtkFloatArray::New();
	scalars->SetNumberOfComponents(npixels);
	float * pscalars = scalars->WritePointer(0, npixels);
	
	size_t idx = 0;
	while (npixels > 0) {

		nbuffer = npixels;
		if (npixels > buffsize)
			nbuffer = buffsize;

		if (fits_read_img(this->pFitsFile, TFLOAT, fpixel, nbuffer, &nullval,
			buffer, &anynull, &status))
			PrintError(status);

		for (int i = 0; i < nbuffer; i++) {

			if (_isnanf(buffer[i])) buffer[i] = -1000000.0; // hack for now
			pscalars[idx++] = buffer[i];

			if (buffer[i] < datamin)
				datamin = buffer[i];
			if (buffer[i] > datamax)
				datamax = buffer[i];
		}

		npixels -= nbuffer;    /* increment remaining number of pixels */
		fpixel += nbuffer;    /* next pixel to be read in image */
	}

	cerr << "min: " << datamin << " max: " << datamax << endl;

	if (fits_close_file(this->pFitsFile, &status))
		PrintError(status);
	this->pFitsFile = nullptr;

	output->GetPointData()->SetScalars(scalars);

	cerr << "done." << endl;

	return 1;
}

void vtkFitsReader::PrintSelf(ostream& os, vtkIndent indent) 
{
  vtkStructuredPointsReader::PrintSelf(os, indent);
  os << indent << "FITS File Name: " << (this->FileName) << "\n";
}
