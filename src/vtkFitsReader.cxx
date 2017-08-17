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

vtkFitsReader::vtkFitsReader() 
{
	this->pFile = nullptr;
	vtkStructuredPoints *output = vtkStructuredPoints::New();
	// Releasing data for pipeline parallism.
	// Filters will know it is empty.
	output->ReleaseData();
	output->Delete();
}


// Note: from cookbook.c in fitsio distribution.
void vtkFitsReader::PrintError(int status) 
{
    cerr << "vtkFitsReader ERROR.";
    if (status) {
       fits_report_error(stderr, status); /* print error report */
       exit( status );    /* terminate the program, returning error status */
    }
    return;
}

int vtkFitsReader::ProcessRequest(vtkInformation* request,
	vtkInformationVector** inputVector,
	vtkInformationVector* outputVector)
{
	// generate the data
	if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
	{
		return this->RequestData(request, inputVector, outputVector);
	}
	
	if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
	{
		return this->RequestDataObject(request, inputVector, outputVector);
	}

	if (request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
	{
		return this->RequestUpdateExtent(request, inputVector, outputVector);
	}
	// execute information
	if (request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
	{
		return this->RequestInformation(request, inputVector, outputVector);
	}
	return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------

int vtkFitsReader::RequestData(vtkInformation * request,
	vtkInformationVector ** inputVector,
	vtkInformationVector *outputVector)
{
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	this->SetErrorCode(vtkErrorCode::NoError);
	vtkIdType numPts = 0, numCells = 0;
	int dimsRead = 0, arRead = 0, originRead = 0;
	vtkStructuredPoints *output = vtkStructuredPoints::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));
	int status = 0, nkeys;
	char card[FLEN_CARD];
	int nfound = 0, anynull = 0;
	long naxes[3], fpixel, nbuffer, npixels;
	const int buffsize = 1000;

	float datamin, datamax, nullval, buffer[buffsize];
	// ImageSource superclass does not do this.
	output->ReleaseData();

	vtkDebugMacro(<< "Reading vtk structured points file...");
	FITS_SAFE_CALL(fits_open_file(&this->pFile, this->FileName, READONLY, &status), status)


	//FITS_SAFE_CALL(fits_get_hdrspace(this->pFile, &nkeys, NULL, &status), status);
	//for (int i = 1; i <= nkeys; i ++) 
	//{
	//	FITS_SAFE_CALL(fits_read_record(this->pFile, i , 
	//		card, &status), status); /* read keyword */
	//	std::clog << card << std::endl;
	//}

	/* read the NAXIS1 and NAXIS2 keyword to get image size */
	FITS_SAFE_CALL(fits_read_keys_lng(this->pFile, "NAXIS",
		1, 3, naxes, &nfound, &status), status);

	npixels = naxes[0] * naxes[1] * naxes[2]; /* num of pixels in the image */
	fpixel = 1;
	nullval = 0;                /* don't check for null values in the image */


	output->SetDimensions(naxes[0], naxes[1], naxes[2]);
	output->SetOrigin(0.0, 0.0, 0.0);

	// Read structured points specvtkStructuredPointsReaderific stuff
	//
	vtkFloatArray* scalars = vtkFloatArray::New();
	scalars->SetNumberOfComponents(npixels);
	float * pscalars = scalars->WritePointer(0, npixels);

	size_t idx = 0;
	while (npixels > 0) 
	{
		nbuffer = npixels;
		if (npixels > buffsize)
			nbuffer = buffsize;

		FITS_SAFE_CALL(fits_read_img(this->pFile, TFLOAT, fpixel, nbuffer, &nullval,
			buffer, &anynull, &status), status)

		for (int i = 0; i < nbuffer; i++) 
		{
			if (_isnanf(buffer[i])) 
				buffer[i] = -1000000.0; // hack for now
			pscalars[idx++] = buffer[i];
		}

		npixels -= nbuffer;    /* increment remaining number of pixels */
		fpixel += nbuffer;    /* next pixel to be read in image */
	}

	FITS_SAFE_CALL(fits_close_file(this->pFile, &status), status)
	output->GetPointData()->SetScalars(scalars);
	return 1;
}


int vtkFitsReader::RequestInformation(vtkInformation * request,
	vtkInformationVector ** inputVector,
	vtkInformationVector * outputVector)
{
	int status = 0;
	char comment[FLEN_COMMENT];
	vtkInformation * outInfo = outputVector->GetInformationObject(0);

	FITS_SAFE_CALL(fits_open_file(&this->pFile,
		this->FileName, READONLY, &status), status);
		
	float origin[3];
	FITS_SAFE_CALL(fits_read_key_flt(this->pFile, "CRVAL1",
		origin, comment, &status), status);
	cerr << "CRVAL1: [" << origin[0] << "] " << comment << endl;
	FITS_SAFE_CALL(fits_read_key_flt(this->pFile, "CRVAL2",
		origin + 1, comment, &status), status);
	cerr << "CRVAL2: [" << origin[1] << "] " << comment << endl;
	FITS_SAFE_CALL(fits_read_key_flt(this->pFile, "CRVAL3",
		origin + 2, comment, &status), status);
	cerr << "CRVAL3: [" << origin[2] << "] " << comment << endl;

	long naxes[3];
	int nfound;
	/* read the NAXIS1 and NAXIS2 keyword to get image size */
	FITS_SAFE_CALL(fits_read_keys_lng(this->pFile, "NAXIS",
		1, 3, naxes, &nfound, &status), status);
	
	vtkStructuredPoints *output = vtkStructuredPoints::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	output->SetDimensions(naxes[0], naxes[1], naxes[2]);
	output->SetOrigin(origin[0], origin[1], origin[2]);
	return 1;
}


void vtkFitsReader::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "FITS File Name: " << (this->FileName) << "\n";
}
