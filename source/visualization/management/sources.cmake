# - G4vis_management module build definition

# Define the Geant4 Module.
geant4_add_module(G4vis_management
  PUBLIC_HEADERS
    G4GraphicsSystemList.hh
    G4Scene.hh
    G4Scene.icc
    G4SceneHandlerList.hh
    G4SceneList.hh
    G4VGraphicsSystem.hh
    G4VSceneHandler.hh
    G4VSceneHandler.icc
    G4VUserVisAction.hh
    G4VViewer.hh
    G4VViewer.icc
    G4VVisCommand.hh
    G4ViewParameters.hh
    G4ViewParameters.icc
    G4ViewerList.hh
    G4VisCommandModelCreate.hh
    G4VisCommands.hh
    G4VisCommandsCompound.hh
    G4VisCommandsGeometry.hh
    G4VisCommandsGeometrySet.hh
    G4VisCommandsListManager.hh
    G4VisCommandsMultithreading.hh
    G4VisCommandsSet.hh
    G4VisCommandsScene.hh
    G4VisCommandsSceneAdd.hh
    G4VisCommandsSceneHandler.hh
    G4VisCommandsTouchable.hh
    G4VisCommandsTouchableSet.hh
    G4VisCommandsViewer.hh
    G4VisCommandsViewerDefault.hh
    G4VisCommandsViewerSet.hh
    G4VisExecutive.hh
    G4VisExecutive.icc
    G4VisFilterManager.hh
    G4VisListManager.hh
    G4VisManager.hh
    G4VisManager.icc
    G4VisModelManager.hh
    G4VisStateDependent.hh
  SOURCES
    G4GraphicsSystemList.cc
    G4Scene.cc
    G4SceneHandlerList.cc
    G4SceneList.cc
    G4VGraphicsSystem.cc
    G4VSceneHandler.cc
    G4VViewer.cc
    G4VVisCommand.cc
    G4ViewParameters.cc
    G4ViewerList.cc
    G4VisCommands.cc
    G4VisCommandsCompound.cc
    G4VisCommandsGeometry.cc
    G4VisCommandsGeometrySet.cc
    G4VisCommandsMultithreading.cc
    G4VisCommandsSet.cc
    G4VisCommandsScene.cc
    G4VisCommandsSceneAdd.cc
    G4VisCommandsSceneHandler.cc
    G4VisCommandsTouchable.cc
    G4VisCommandsTouchableSet.cc
    G4VisCommandsViewer.cc
    G4VisCommandsViewerDefault.cc
    G4VisCommandsViewerSet.cc
    G4VisManager.cc
    G4VisStateDependent.cc)

geant4_module_link_libraries(G4vis_management
  PUBLIC
    G4modeling
    G4hepgeometry
    G4graphics_reps
    G4globman
    G4intercoms
    G4hits
  PRIVATE
    G4csg
    G4detutils
    G4geomBoolean
    G4specsolids
    G4event
    G4geometrymng
    G4materials
    G4partman
    G4tracking
    G4run
    G4tasking
    G4navigation
    G4digits
    G4heprandom)
