//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     G4PhotoElectricAngularGeneratorPolarized
//
// Author: A. C. Farinha, L. Peralta, P. Rodrigues and A. Trindade
//
// Creation date:
//
// Modifications:
// 10 January 2006
// 06 May 2011, Replace FILE with std::ifstream
//
// Class Description:
//
// Concrete class for PhotoElectric Electron Angular Polarized Distribution Generation
//
// Class Description:
// PhotoElectric Electron Angular Generator based on the general Gavrila photoelectron angular distribution.
// Includes polarization effects for K and L1 atomic shells, according to Gavrila (1959, 1961).
// For higher shells the L1 cross-section is used.
//
// The Gavrila photoelectron angular distribution is a complex function which can not be sampled using
// the inverse-transform method (James 1980). Instead a more general approach based on the one already
// used to sample bremsstrahlung 2BN cross section (G4Generator2BN, Peralta, 2005) was used.
//
// M. Gavrila, "Relativistic K-Shell Photoeffect", Phys. Rev. 113, 514-526   (1959)
// M. Gavrila, "Relativistic L-Shell Photoeffect", Phys. Rev. 124, 1132-1141 (1961)
// F. James, Rept. on Prog. in Phys. 43, 1145 (1980)
// L. Peralta et al., "A new low-energy bremsstrahlung generator for GEANT4", Radiat. Prot. Dosimetry. 116, 59-64 (2005)
//
//
// -------------------------------------------------------------------
//
//

#include "G4PhotoElectricAngularGeneratorPolarized.hh"
#include "G4PhysicalConstants.hh"
#include "G4RotationMatrix.hh"
#include "Randomize.hh"
#include "G4Exp.hh"

G4PhotoElectricAngularGeneratorPolarized::G4PhotoElectricAngularGeneratorPolarized()
  :G4VEmAngularDistribution("AngularGenSauterGavrilaPolarized")
{
  const G4int arrayDim = 980;

  //minimum electron beta parameter allowed
  betaArray[0] = 0.02;
  //beta step
  betaArray[1] = 0.001;
  //maximum index array for a and c tables
  betaArray[2] = arrayDim - 1;

  // read Majorant Surface Parameters. This are required in order to generate 
  //Gavrila angular photoelectron distribution
  for(G4int level = 0; level < 2; level++){
    char nameChar0[100] = "ftab0.dat"; // K-shell Majorant Surface Parameters
    char nameChar1[100] = "ftab1.dat"; // L-shell Majorant Surface Parameters

    G4String filename;
    if(level == 0) filename = nameChar0;
    if(level == 1) filename = nameChar1;

    char* path = std::getenv("G4LEDATA");
    if (!path)
      {
        G4String excep = "G4EMDataSet - G4LEDATA environment variable not set";
        G4Exception("G4PhotoElectricAngularGeneratorPolarized::G4PhotoElectricAngularGeneratorPolarized",
		    "em0006",FatalException,"G4LEDATA environment variable not set");
	return;
      }

    G4String pathString(path);
    G4String dirFile = pathString + "/photoelectric_angular/" + filename;
    std::ifstream infile(dirFile);
    if (!infile.is_open())
      {
	G4String excep = "data file: " + dirFile + " not found";
        G4Exception("G4PhotoElectricAngularGeneratorPolarized::G4PhotoElectricAngularGeneratorPolarized",
		    "em0003",FatalException,excep);
	return;
      }

    // Read parameters into tables. The parameters are function of incident electron 
    // energy and shell level
    G4float aRead=0,cRead=0, beta=0;
    for(G4int i=0 ; i<arrayDim ;++i){     
      infile >> beta >> aRead >> cRead;
      aMajorantSurfaceParameterTable[i][level] = aRead;
      cMajorantSurfaceParameterTable[i][level] = cRead;
    }
    infile.close();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4PhotoElectricAngularGeneratorPolarized::~G4PhotoElectricAngularGeneratorPolarized()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ThreeVector&
G4PhotoElectricAngularGeneratorPolarized::SampleDirection(
                                 const G4DynamicParticle* dp,
				 G4double eKinEnergy,
				 G4int shellId,
				 const G4Material*)
{
  // (shellId == 0) - K-shell  - Polarized model for K-shell
  // (shellId > 0)  - L1-shell - Polarized model for L1 and higher shells

  // Calculate Lorentz term (gamma) and beta parameters
  G4double gamma = 1 + eKinEnergy/electron_mass_c2;
  G4double beta  = std::sqrt((gamma - 1)*(gamma + 1))/gamma;

  const G4ThreeVector& direction = dp->GetMomentumDirection();
  const G4ThreeVector& polarization = dp->GetPolarization();

  G4double theta, phi = 0;
  // Majorant surface parameters
  // function of the outgoing electron kinetic energy
  G4double aBeta = 0;
  G4double cBeta = 0;

  // For the outgoing kinetic energy
  // find the current majorant surface parameters
  PhotoElectronGetMajorantSurfaceAandCParameters(shellId, beta, &aBeta, &cBeta);

  // Generate pho and theta according to the shell level
  // and beta parameter of the electron
  PhotoElectronGeneratePhiAndTheta(shellId, beta, aBeta, cBeta, &phi, &theta);

  // Determine the rotation matrix
  const G4RotationMatrix rotation =
    PhotoElectronRotationMatrix(direction, polarization);

  // Compute final direction of the outgoing electron
  fLocalDirection = PhotoElectronComputeFinalDirection(rotation, theta, phi);

  return fLocalDirection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void
G4PhotoElectricAngularGeneratorPolarized::PhotoElectronGeneratePhiAndTheta(
      G4int shellLevel, G4double beta, G4double aBeta, G4double cBeta,
      G4double *pphi, G4double *ptheta) const
{
  G4double rand1, rand2, rand3 = 0;
  G4double phi = 0;
  G4double theta = 0;
  G4double crossSectionValue = 0;
  G4double crossSectionMajorantFunctionValue = 0;
  G4double maxBeta = 0;

  do {

    rand1 = G4UniformRand();
    rand2 = G4UniformRand();
    rand3 = G4UniformRand();

    phi=2*pi*rand1;

    if(shellLevel == 0){
      // Polarized Gavrila Cross-Section for K-shell (1959)
      theta=std::sqrt(((G4Exp(rand2*std::log(1+cBeta*pi*pi)))-1)/cBeta);
      crossSectionMajorantFunctionValue =
	CrossSectionMajorantFunction(theta, cBeta);
      crossSectionValue = DSigmaKshellGavrila1959(beta, theta, phi);
    } else {
      //  Polarized Gavrila Cross-Section for other shells (L1-shell) (1961)
      theta = std::sqrt(((G4Exp(rand2*std::log(1+cBeta*pi*pi)))-1)/cBeta);
      crossSectionMajorantFunctionValue =
	CrossSectionMajorantFunction(theta, cBeta);
      crossSectionValue = DSigmaL1shellGavrila(beta, theta, phi);
    }

    maxBeta=rand3*aBeta*crossSectionMajorantFunctionValue;
    if(crossSectionValue < 0.0) { crossSectionValue = maxBeta; }

  } while(maxBeta > crossSectionValue || theta > CLHEP::pi);

  *pphi = phi;
  *ptheta = theta;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double
G4PhotoElectricAngularGeneratorPolarized::CrossSectionMajorantFunction(
         G4double theta, G4double cBeta) const
{
  // Compute Majorant Function
  G4double crossSectionMajorantFunctionValue = 0;
  crossSectionMajorantFunctionValue = theta/(1+cBeta*theta*theta);
  return crossSectionMajorantFunctionValue;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double
G4PhotoElectricAngularGeneratorPolarized::DSigmaKshellGavrila1959(
         G4double beta, G4double theta, G4double phi) const
{
  //Double differential K shell cross-section (Gavrila 1959)

  G4double beta2 = beta*beta;
  G4double oneBeta2 = 1 - beta2;
  G4double sqrtOneBeta2 = std::sqrt(oneBeta2);
  G4double oneBeta2_to_3_2 = std::pow(oneBeta2,1.5);
  G4double cosTheta = std::cos(theta);
  G4double sinTheta2 = std::sin(theta)*std::sin(theta);
  G4double cosPhi2 = std::cos(phi)*std::cos(phi);
  G4double oneBetaCosTheta = 1-beta*cosTheta;
  G4double dsigma = 0;
  G4double firstTerm = 0;
  G4double secondTerm = 0;

  firstTerm = sinTheta2*cosPhi2/std::pow(oneBetaCosTheta,4)-(1 - sqrtOneBeta2)/(2*oneBeta2) *
              (sinTheta2 * cosPhi2)/std::pow(oneBetaCosTheta,3) + (1-sqrtOneBeta2)*
              (1-sqrtOneBeta2)/(4*oneBeta2_to_3_2) * sinTheta2/std::pow(oneBetaCosTheta,3);

  secondTerm = std::sqrt(1 - sqrtOneBeta2)/(std::pow(2.,3.5)*beta2*std::pow(oneBetaCosTheta,2.5)) *
               (4*beta2/sqrtOneBeta2 * sinTheta2*cosPhi2/oneBetaCosTheta + 4*beta/oneBeta2 * cosTheta * cosPhi2
               - 4*(1-sqrtOneBeta2)/oneBeta2 *(1+cosPhi2) - beta2 * (1-sqrtOneBeta2)/oneBeta2 * sinTheta2/oneBetaCosTheta
               + 4*beta2*(1-sqrtOneBeta2)/oneBeta2_to_3_2 - 4*beta*(1-sqrtOneBeta2)*(1-sqrtOneBeta2)/oneBeta2_to_3_2 * cosTheta)
               + (1-sqrtOneBeta2)/(4*beta2*oneBetaCosTheta*oneBetaCosTheta) * (beta/oneBeta2 - 2/oneBeta2 * cosTheta * cosPhi2 +
               (1-sqrtOneBeta2)/oneBeta2_to_3_2 * cosTheta - beta * (1-sqrtOneBeta2)/oneBeta2_to_3_2);

  dsigma = ( firstTerm*(1-pi*fine_structure_const/beta) + secondTerm*(pi*fine_structure_const) )*std::sin(theta);

  return dsigma;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double
G4PhotoElectricAngularGeneratorPolarized::DSigmaL1shellGavrila(
        G4double beta, G4double theta, G4double phi) const
{
  //Double differential L1 shell cross-section (Gavrila 1961)
  G4double beta2 = beta*beta;
  G4double oneBeta2 = 1-beta2;
  G4double sqrtOneBeta2 = std::sqrt(oneBeta2);
  G4double oneBeta2_to_3_2=std::pow(oneBeta2,1.5);
  G4double cosTheta = std::cos(theta);
  G4double sinTheta2 =std::sin(theta)*std::sin(theta);
  G4double cosPhi2 = std::cos(phi)*std::cos(phi);
  G4double oneBetaCosTheta = 1-beta*cosTheta;

  G4double dsigma = 0;
  G4double firstTerm = 0;
  G4double secondTerm = 0;

  firstTerm = sinTheta2*cosPhi2/std::pow(oneBetaCosTheta,4)-(1 - sqrtOneBeta2)/(2*oneBeta2)
              *  (sinTheta2 * cosPhi2)/std::pow(oneBetaCosTheta,3) + (1-sqrtOneBeta2)*
              (1-sqrtOneBeta2)/(4*oneBeta2_to_3_2) * sinTheta2/std::pow(oneBetaCosTheta,3);

  secondTerm = std::sqrt(1 - sqrtOneBeta2)/(std::pow(2.,3.5)*beta2*std::pow(oneBetaCosTheta,2.5)) *
               (4*beta2/sqrtOneBeta2 * sinTheta2*cosPhi2/oneBetaCosTheta + 4*beta/oneBeta2 * cosTheta * cosPhi2
               - 4*(1-sqrtOneBeta2)/oneBeta2 *(1+cosPhi2) - beta2 * (1-sqrtOneBeta2)/oneBeta2 * sinTheta2/oneBetaCosTheta
               + 4*beta2*(1-sqrtOneBeta2)/oneBeta2_to_3_2 - 4*beta*(1-sqrtOneBeta2)*(1-sqrtOneBeta2)/oneBeta2_to_3_2 * cosTheta)
               + (1-sqrtOneBeta2)/(4*beta2*oneBetaCosTheta*oneBetaCosTheta) * (beta/oneBeta2 - 2/oneBeta2 * cosTheta * cosPhi2 +
               (1-sqrtOneBeta2)/oneBeta2_to_3_2*cosTheta - beta*(1-sqrtOneBeta2)/oneBeta2_to_3_2);

  dsigma = ( firstTerm*(1-pi*fine_structure_const/beta) + secondTerm*(pi*fine_structure_const) )*std::sin(theta);

  return dsigma;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4RotationMatrix
G4PhotoElectricAngularGeneratorPolarized::PhotoElectronRotationMatrix(
           const G4ThreeVector& direction,
	   const G4ThreeVector& polarization)
{
  G4double mK = direction.mag();
  G4double mS = polarization.mag();
  G4ThreeVector polarization2 = polarization;
  const G4double kTolerance = 1e-6;

  if(!(polarization.isOrthogonal(direction,kTolerance)) || mS == 0){
    G4ThreeVector d0 = direction.unit();
    G4ThreeVector a1 = PerpendicularVector(d0);
    G4ThreeVector a0 = a1.unit();
    G4double rand1 = G4UniformRand();
    G4double angle = twopi*rand1;
    G4ThreeVector b0 = d0.cross(a0);
    G4ThreeVector c;
    c.setX(std::cos(angle)*(a0.x())+std::sin(angle)*b0.x());
    c.setY(std::cos(angle)*(a0.y())+std::sin(angle)*b0.y());
    c.setZ(std::cos(angle)*(a0.z())+std::sin(angle)*b0.z());
    polarization2 = c.unit();
    mS = polarization2.mag();
  }else
    {
      if ( polarization.howOrthogonal(direction) != 0)
	{
	  polarization2 = polarization
	    - polarization.dot(direction)/direction.dot(direction) * direction;
	}
    }

  G4ThreeVector direction2 = direction/mK;
  polarization2 = polarization2/mS;

  G4ThreeVector y = direction2.cross(polarization2);

  G4RotationMatrix R(polarization2,y,direction2);
  return R;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void
G4PhotoElectricAngularGeneratorPolarized::PhotoElectronGetMajorantSurfaceAandCParameters(G4int shellId, G4double beta, G4double *majorantSurfaceParameterA, G4double *majorantSurfaceParameterC) const
{
  // This member function finds for a given shell and beta value
  // of the outgoing electron the correct Majorant Surface parameters
  G4double aBeta,cBeta;
  G4double bMin,bStep;
  G4int indexMax;
  G4int level = 0;
  if(shellId > 0) { level = 1; }

  bMin = betaArray[0];
  bStep = betaArray[1];
  indexMax = (G4int)betaArray[2];
  const G4double kBias = 1e-9;

  G4int k = (G4int)((beta-bMin+kBias)/bStep);

  if(k < 0)
    k = 0;
  if(k > indexMax)
    k = indexMax;

  if(k == 0)
    aBeta = std::max(aMajorantSurfaceParameterTable[k][level],aMajorantSurfaceParameterTable[k+1][level]);
  else if(k==indexMax)
    aBeta = std::max(aMajorantSurfaceParameterTable[k-1][level],aMajorantSurfaceParameterTable[k][level]);
  else{
    aBeta = std::max(aMajorantSurfaceParameterTable[k-1][level],aMajorantSurfaceParameterTable[k][level]);
    aBeta = std::max(aBeta,aMajorantSurfaceParameterTable[k+1][level]);
  }

  if(k == 0)
    cBeta = std::max(cMajorantSurfaceParameterTable[k][level],cMajorantSurfaceParameterTable[k+1][level]);
  else if(k == indexMax)
    cBeta = std::max(cMajorantSurfaceParameterTable[k-1][level],cMajorantSurfaceParameterTable[k][level]);
  else{
    cBeta = std::max(cMajorantSurfaceParameterTable[k-1][level],cMajorantSurfaceParameterTable[k][level]);
    cBeta = std::max(cBeta,cMajorantSurfaceParameterTable[k+1][level]);
  }

  *majorantSurfaceParameterA = aBeta;
  *majorantSurfaceParameterC = cBeta;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ThreeVector G4PhotoElectricAngularGeneratorPolarized::PhotoElectronComputeFinalDirection(const G4RotationMatrix& rotation, G4double theta, G4double phi) const
{
  //computes the photoelectron momentum unitary vector
  G4double sint = std::sin(theta);
  G4double px = std::cos(phi)*sint;
  G4double py = std::sin(phi)*sint;
  G4double pz = std::cos(theta);

  G4ThreeVector samplingDirection(px,py,pz);

  G4ThreeVector outgoingDirection = rotation*samplingDirection;
  return outgoingDirection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4PhotoElectricAngularGeneratorPolarized::PrintGeneratorInformation() const
{
  G4cout << "\n" << G4endl;
  G4cout << "Polarized Photoelectric Angular Generator" << G4endl;
  G4cout << "PhotoElectric Electron Angular Generator based on the general Gavrila photoelectron angular distribution" << G4endl;
  G4cout << "Includes polarization effects for K and L1 atomic shells, according to Gavrilla (1959, 1961)." << G4endl;
  G4cout << "For higher shells the L1 cross-section is used." << G4endl;
  G4cout << "(see Physics Reference Manual) \n" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ThreeVector
G4PhotoElectricAngularGeneratorPolarized::PerpendicularVector(
         const G4ThreeVector& a) const
{
  G4double dx = a.x();
  G4double dy = a.y();
  G4double dz = a.z();
  G4double x = dx < 0.0 ? -dx : dx;
  G4double y = dy < 0.0 ? -dy : dy;
  G4double z = dz < 0.0 ? -dz : dz;
  if (x < y) {
    return x < z ? G4ThreeVector(-dy,dx,0) : G4ThreeVector(0,-dz,dy);
  }else{
    return y < z ? G4ThreeVector(dz,0,-dx) : G4ThreeVector(-dy,dx,0);
  }
}
