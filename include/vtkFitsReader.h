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

class VTKIOFITS_EXPORT vtkFitsReader : public vtkStructuredPointsReader
{
public:

	static vtkFitsReader *New() {return new vtkFitsReader;};
	vtkTypeMacro(vtkFitsReader, vtkStructuredPointsReader)
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkStructuredPoints * GetOutput();
	vtkStructuredPoints * GetOutput(int);

	int ReadMetaData(vtkInformation *) VTK_OVERRIDE;

protected:
	vtkFitsReader();
	~vtkFitsReader() VTK_OVERRIDE;
	void PrintError(int status); // from fitsio distribution

	int ReadHeader() { return 1; }
	int ReadScalarData(vtkDataSet *, vtkIdType);

	int FillOutputPortInformation(int, vtkInformation *) VTK_OVERRIDE;

	int RequestData(vtkInformation *, vtkInformationVector **,
		vtkInformationVector *) VTK_OVERRIDE;
	int RequestInformation(vtkInformation *, vtkInformationVector **,
		vtkInformationVector *) VTK_OVERRIDE;
	int RequestUpdateExtent(vtkInformation *, vtkInformationVector **,
		vtkInformationVector *) VTK_OVERRIDE
	{
		cerr << "RequestUpdateExtent" << endl;
		return 1;
	}

	// Default method performs Update to get information.  Not all the old
	// structured points sources compute information
private:
	fitsfile * pFitsFile;
	float DataMin;
	float DataMax;

};

#endif 
