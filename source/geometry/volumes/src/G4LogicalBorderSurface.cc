// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4LogicalBorderSurface.cc,v 1.2 1999/12/15 14:50:26 gunter Exp $
// GEANT4 tag $Name: geant4-01-01 $
//
////////////////////////////////////////////////////////////////////////
// G4LogicalBorderSurface Implementation
////////////////////////////////////////////////////////////////////////
//
// File:        G4LogicalBorderSurface.cc
// Description: A Logical Surface class for surfaces defined by the
//               boundary of two physical volumes.
// Version:     1.0
// Created:     1997-06-26
// Author:      John Apostolakis
// mail:        John.Apostolakis@cern.ch
// Modified:    1997-06-26  John Apostolakis
//
// Id tag:      
////////////////////////////////////////////////////////////////////////

#include "G4LogicalBorderSurface.hh"


G4LogicalBorderSurfaceTable G4LogicalBorderSurface::theBorderSurfaceTable;

/////////////////////////
// Class Implementation
/////////////////////////

  /////////////////
  // Constructors
  /////////////////

G4LogicalBorderSurface::G4LogicalBorderSurface(const G4String& name,
					       G4VPhysicalVolume* vol1, 
					       G4VPhysicalVolume* vol2,
					       G4OpticalSurface* opticsSurface)
		: G4LogicalSurface(name, opticsSurface),
		  Volume1(vol1),
		  Volume2(vol2)
{
	// Store in the table of Surfaces
	theBorderSurfaceTable.insert(this);
	theIndexInTable = theBorderSurfaceTable.index(this);
}

G4LogicalBorderSurface::G4LogicalBorderSurface(const G4LogicalBorderSurface &right)
       : G4LogicalSurface(right.GetName(), right.GetOpticalSurface())
{
	*this = right;
}

G4LogicalBorderSurface::~G4LogicalBorderSurface(){}

  //////////////
  // Operators
  //////////////

const G4LogicalBorderSurface& G4LogicalBorderSurface::operator=(const G4LogicalBorderSurface &right)
{
	return right;
}

G4int G4LogicalBorderSurface::operator==(const G4LogicalBorderSurface &right) const
{
	return (this == (G4LogicalBorderSurface *) &right);
}

G4int G4LogicalBorderSurface::operator!=(const G4LogicalBorderSurface &right) const
{
	return (this != (G4LogicalBorderSurface *) &right);
}
  ////////////
  // Methods
  ////////////

G4LogicalBorderSurface* G4LogicalBorderSurface::GetSurface(const G4VPhysicalVolume* vol1,
				 const G4VPhysicalVolume* vol2)
{
	for (int i=0; i<theBorderSurfaceTable.length(); i++) {
		if(theBorderSurfaceTable[i]->GetVolume1() == vol1 &&
		   theBorderSurfaceTable[i]->GetVolume2() == vol2 )
			return theBorderSurfaceTable[i];
	}
	return NULL;
}

void G4LogicalBorderSurface::DumpInfo() // Class method (it is really const)
{

    // Dump info for known surfaces

    G4cout << "***** Surface Table : Nb of Surfaces = " << 
            GetNumberOfBorderSurfaces() << " *****" << G4endl;

    for (int i=0; i<theBorderSurfaceTable.length(); i++) {
      G4cout << theBorderSurfaceTable[i]->GetName() << " : " << G4endl <<
          "  Surface type   = " << theBorderSurfaceTable[i]->GetName() << G4endl;
#ifdef PRINT_INFO
          "  Surface type   = " << theBorderSurfaceTable[i]->GetOpticalSurface()->GetType()   << G4endl;
          "  Surface finish = " << theBorderSurfaceTable[i]->GetFinish() << G4endl <<
	  "  Surface model  = " << theBorderSurfaceTable[i]->GetModel()  << G4endl;
#endif 
    }
    G4cout << G4endl;
}