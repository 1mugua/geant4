#ifndef G4GEOMETRYTABLE_HH
#define G4GEOMETRYTABLE_HH

#include "G4GeometryCreator.hh"
//#include "G4OrderedTable.hh"
//#include "G4NISTStepReader.hh"

//typedef G4RWTPtrOrderedVector<G4Curve> G4CurveVector;
//typedef G4RWTPtrOrderedVector<G4CurveVector> G4BoundaryVector;
//typedef G4RWTPtrOrderedVector<G4PlacedSolid> G4PlacedSolidVector;
//typedef G4RWTPtrOrderedVector<G4GeometryCreator*> G4CreatorVector;
typedef G4RWTPtrOrderedVector<G4GeometryCreator> G4CreatorVector;
//typedef G4RWTPtrOrderedVector<G4Surface> G4SurfaceVector;
//typedef G4RWTPtrOrderedVector<G4BREPSolid> G4SolidVector;



//operator ==(class G4RWTPtrOrderedVector<G4Curve> a, class G4RWTPtrOrderedVector<G4Curve> b){}


class G4GeometryTable
{
public:
  // Destructor
  ~G4GeometryTable();    
  //Member functions
  static void RegisterObject(G4GeometryCreator*);
  static G4bool ExistsInTable(G4String&);
  static G4GeometryCreator* GetObject(G4String);
  static void* CreateObject(STEPentity&);
  static void* CreateSTEPObject(void*, G4String&);  
  static void PrintObjectNames();
  static G4GeometryTable& GetInstance(){return gt;}

public:
  // Constructor, singleton
  G4GeometryTable();

  //Members
private:
  static G4CreatorVector RegisteredObjects;
  static G4GeometryTable gt;
};

#endif


