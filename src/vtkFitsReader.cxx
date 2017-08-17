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

void vtkFitsReader::SetOutput(vtkStructuredPoints *output)
{
	this->GetExecutive()->SetOutputData(0, output);
}

vtkStructuredPoints* vtkFitsReader::GetOutput()
{
	return this->GetOutput(0);
}

vtkStructuredPoints* vtkFitsReader::GetOutput(int idx)
{
	return vtkStructuredPoints::SafeDownCast(this->GetOutputDataObject(idx));
}

// Note: from cookbook.c in fitsio distribution.
//----------------------------------------------------------------------------
// Default method performs Update to get information.  Not all the old
// structured points sources compute information
int vtkFitsReader::RequestInformation(
	vtkInformation*,
	vtkInformationVector**,
	vtkInformationVector* outputVector)
{
	// get the info objects
	//vtkInformation* outInfo = outputVector->GetInformationObject(0);
	return 1;
}

int vtkFitsReader::FillOutputPortInformation(int, vtkInformation *info)
{
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkStructuredPoints");
	return 1;
}

// Note: This function adapted from readimage() from cookbook.c in
// fitsio distribution.
int vtkFitsReader::RequestData(
	vtkInformation*,
	vtkInformationVector**,
	vtkInformationVector* outputVector)
{

	vtkInformation * outInfo = outputVector->GetInformationObject(0);

	this->SetErrorCode(vtkErrorCode::NoError);

	vtkStructuredPoints * output = vtkStructuredPoints::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	int status = 0, nfound = 0, anynull = 0;
	long naxes[3], fpixel, nbuffer, npixels;
	const int buffsize = 100000;
	char comm[FLEN_COMMENT];
	long *pNaxis;

	pNaxis = new long[3];
	float buffer[buffsize];
	float nullval;

	FITS_SAFE_CALL(fits_open_file(&this->pFile, this->FileName, 
		READONLY, &status), status);

	/* read the NAXIS1 and NAXIS2 keyword to get image size */
	FITS_SAFE_CALL(fits_read_keys_lng(this->pFile, "NAXIS", 
		1, 3, naxes, &nfound, &status), status);

	FITS_SAFE_CALL(fits_read_key_lng(this->pFile, "NAXIS", 
		pNaxis, comm, &status), status);

	///fits_read_key_lng(this->pFile, "EPOCH",pEpoch,comm, &status);

	npixels = naxes[0] * naxes[1] * naxes[2]; /* num of pixels in the image */
	fpixel = 1;
	nullval = 0;                /* don't check for null values in the image */

	output->SetDimensions(naxes[0], naxes[1], naxes[2]);
	output->SetOrigin(0.0, 0.0, 0.0);

	vtkFloatArray *scalars = vtkFloatArray::New();
	scalars->Allocate(npixels);

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
			scalars->InsertNextValue(buffer[i]);
		}
		npixels -= nbuffer;    /* increment remaining number of pixels */
		fpixel += nbuffer;    /* next pixel to be read in image */
	}

	FITS_SAFE_CALL(fits_close_file(this->pFile, &status), status)
	output->GetPointData()->SetScalars(scalars);
	output->SetScalarType(scalars->GetDataType(), outInfo);
	ReadHeader();

	delete [] pNaxis;
	return 1;
}

// Note: This function adapted from printheader() from cookbook.c in
// fitsio distribution.
void vtkFitsReader::ReadHeader() 
{

	int status, nkeys, keypos, hdutype;
	char card[FLEN_CARD];   /* standard string lengths defined in fitsioc.h */

	status = 0;

	FITS_SAFE_CALL(fits_open_file(&this->pFile, FileName, READONLY, &status), status)

	/* attempt to move to next HDU, until we get an EOF error */
	for (int i = 1; !(fits_movabs_hdu(this->pFile, i, &hdutype, &status)); i++)
	{
		/* get no. of keywords */
		FITS_SAFE_CALL(fits_get_hdrpos(this->pFile, &nkeys, &keypos, &status), status)

		//cerr << "from header:" << endl;
		// not part of header...
		//printf("Header listing for HDU #%d:\n", ii);
		for (int j = 1; j <= nkeys; j++) {
			FITS_SAFE_CALL(fits_read_record(this->pFile, j, card, &status), status)
		}
	}
	//cerr << "\nextracted strings:\n" << title << "\n" << xStr << "\n" << yStr << "\n" << zStr << endl;
}

void vtkFitsReader::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "FITS File Name: " << (this->FileName) << "\n";
}
