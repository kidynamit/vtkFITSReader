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

class VTK_EXPORT vtkFitsReader : public vtkStructuredPointsReader
{
public:

	static vtkFitsReader *New() {return new vtkFitsReader;};
	vtkTypeMacro(vtkFitsReader, vtkStructuredPointsReader)
	void PrintSelf(ostream& os, vtkIndent indent);

	void SetFileName(char *name);
	char *GetFileName(){return filename;}


protected:
	vtkFitsReader();
	~vtkFitsReader();
	void Execute();
	void ReadHeader();
	void printerror(int status); // from fitsio distribution
	int RequestData(vtkInformation *, vtkInformationVector **,
		vtkInformationVector *) VTK_OVERRIDE;

	// Default method performs Update to get information.  Not all the old
	// structured points sources compute information
	int RequestInformation(vtkInformation *, vtkInformationVector **,
		vtkInformationVector *) VTK_OVERRIDE;

	int FillOutputPortInformation(int, vtkInformation *) VTK_OVERRIDE;

private:
	char title[80];
	char filename[256]; // static buffer for filename
	char xStr[80];
	char yStr[80];
	char zStr[80];

};

#endif 
