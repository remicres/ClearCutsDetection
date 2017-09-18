cmake_minimum_required (VERSION 2.8)

OTB_CREATE_APPLICATION(NAME           ClearCutsDetection
                       SOURCES        otbClearCutsDetection.cxx
                       LINK_LIBRARIES OTBCommon)

OTB_CREATE_APPLICATION(NAME           ClearCutsAggregation
                       SOURCES        otbClearCutsAggregation.cxx
                       LINK_LIBRARIES OTBCommon)