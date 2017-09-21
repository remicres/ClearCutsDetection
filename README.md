# Clear cuts detection applications

This module contains two applications for clear cuts detection.

## Dependencies

* The [OTB](https://www.orfeo-toolbox.org/) library
* The [SimpleExtractionTools](https://github.com/remicres/SimpleExtractionTools) remote module for OTB

## How to build it

The module can be built like any other [otb remote module](https://wiki.orfeo-toolbox.org/index.php/How_to_write_a_remote_module). You can build it either from within OTB's sources or outside it.


## Clear cuts detection application

### Description

This application implements the clear cuts detection pipeline, developed by IRSTEA (Kenji Ose, Michel Deshayes, Rémi Cresson)

### Method

From a pair of images, (each one aquired at a different date), and a set of optional mask (input images masks, forest mask), dNDVI (the difference of the normalized difference vegetation index) is computed. Then, a threshold based on mean and standard deviation is performed to produce a map of clear cuts. The output is a vector data layer.

### How to use it

ClearCutsDetection in an OTBApplication.
It can be used as any OTB application (gui, command line, python, c++, ...).

```
This is the ClearCutsDetection (ClearCutsDetection) application, version 6.1.0

This application performs cuts detection, from two input images and an optional forest mask
Parameters: 
        -progress <boolean>        Report progress 
        -help     <string list>    Display long help (empty list), or help for given parameters keys
        -inbmask  <string>         Input vector data for T0 Image mask (Before)  (optional, off by default)
        -inamask  <string>         Input vector data for T1 Image mask (After)  (optional, off by default)
MISSING -inb      <string>         Input T0 Image (Before)  (mandatory)
MISSING -ina      <string>         Input T1 Image (After)  (mandatory)
        -masksdir <string>         Vegetation masks directory  (optional, off by default)
        -nirb     <int32>          near infrared band index for input T0 image  (mandatory, default value is 4)
        -redb     <int32>          red band index for input T0 image  (mandatory, default value is 1)
        -nira     <int32>          near infrared band index for input T1 image  (mandatory, default value is 4)
        -reda     <int32>          red band index for input T1 image  (mandatory, default value is 1)
        -filt     <int32>          Minimum number of pixels detected  (mandatory, default value is 10)
MISSING -outvec   <string>         Output vector layer  (mandatory)
        -ram      <int32>          Available RAM (Mb)  (optional, off by default, default value is 128)
        -inxml    <string>         Load otb application from xml file  (optional, off by default)

```

Licence
=======

This code is provided under the CeCILL-B free software license agreement.

Contact
=======

For any issues regarding this module please contact Rémi Cresson.

remi.cresson@teledetection.fr

Irstea ((French) National Research Institute of Science and Technology for Environment and Agriculture)
www.irstea.fr
