// .NAME vtkFitsReader - read structured points from FITS file.
// .SECTION Description
// vtkFitsReader is a source object that reads FITS data files
// .SECTION Caveats
// Uses CFITSIO v2.0 (http://heasarc.gsfc.nasa.gov/docs/software/fitsio)

#ifndef __vtkFitsReader_h
#define __vtkFitsReader_h

#include <vtkDataReader.h>
#include <vtkStructuredPointsReader.h>
#include <fitsio.h>
#include "vtkIOFitsModule.h"
#include "vtkStructuredPoints.h"

class VTK_EXPORT vtkFitsReader : public vtkStructuredPointsReader
{
public:

	static vtkFitsReader *New() {return new vtkFitsReader;};
	vtkTypeMacro(vtkFitsReader, vtkStructuredPointsReader)
	void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

protected:
	vtkFitsReader();
	void PrintError(int status); // from fitsio distribution
	int ProcessRequest(vtkInformation*, vtkInformationVector**,
		vtkInformationVector*) VTK_OVERRIDE;
	int ReadHeader() { return 1; }

	int RequestData(vtkInformation *, vtkInformationVector **,
		vtkInformationVector *) VTK_OVERRIDE;

	int RequestDataObject(vtkInformation *, vtkInformationVector **,
		vtkInformationVector *)
	{
		return 1;
	}

	int RequestUpdateExtent(vtkInformation *, vtkInformationVector **,
		vtkInformationVector *) VTK_OVERRIDE
	{
		return 1;
	}

	int RequestInformation(vtkInformation *, vtkInformationVector **,
		vtkInformationVector *) VTK_OVERRIDE
	{
		return 1;
	}

private:
	fitsfile * pFile;

	// Default method performs Update to get information.  Not all the old
	// structured points sources compute information

};

#endif 
