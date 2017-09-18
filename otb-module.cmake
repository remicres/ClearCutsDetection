set(DOCUMENTATION "Clear Cuts Detection")

otb_module(ClearCutsDetection
  DEPENDS
    Mosaic
    OTBIndices
    OTBStatistics
    OTBIOXML
    SimpleExtractionTools
    	
  TEST_DEPENDS
    OTBTestKernel
    OTBCommandLine
  DESCRIPTION
    "Set of tools to detect clear cuts in forests"
)
