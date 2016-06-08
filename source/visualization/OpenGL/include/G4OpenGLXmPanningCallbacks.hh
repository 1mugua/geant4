// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4OpenGLXmPanningCallbacks.hh,v 1.3 1999/12/15 14:54:05 gunter Exp $
// GEANT4 tag $Name: geant4-01-01 $
//
//

#ifdef G4VIS_BUILD_OPENGLXM_DRIVER

#ifndef G4OPENGLXMPANNINGCALLBACKS_HH
#define G4OPENGLXMPANNINGCALLBACKS_HH

#include "G4OpenGLXmViewer.hh"

void zoom_callback (Widget w, 
		    XtPointer clientData, 
		    XtPointer callData);

void dolly_callback (Widget w, 
		     XtPointer clientData, 
		     XtPointer callData);

void pan_left_right_callback (Widget w, 
			      XtPointer clientData, 
			      XtPointer callData);

void pan_up_down_callback (Widget w, 
			   XtPointer clientData, 
			   XtPointer callData);

void left_right_pan_callback (XtPointer clientData, 
			      XtIntervalId timer_id);

void up_down_pan_callback (XtPointer clientData, 
			   XtIntervalId timer_id);

#endif

#endif