# Overview

![Persival Icon](persival_icon.png)

This document describes all classes, variables and functions in the phenomenological radar model. It is intended for
developers who want to extend the model. For a user's guide with information about the model functionality, please
refer to the README in the project's root folder.

In the image below you can find an overview of the code and how each method interacts with each other.
In general the whole radar model is seperated into two classes.
The SensorModel class contains all methods related to the sensor model with its signal propagation and signal
processing.
The ObjectScatterCenters class contains all methods to generate and extract data of different objects' scatter centers.
A scatter center thereby is a point in space on an object which typically reflects the radar signal.
Each scatter center has a certain azimuth incident angle dependent radar cross section (RCS) and a certain position.
Each method is visualized with a block and the arrows show the flow of information in between.
Next to the visualized blocks some additional functionalities are implemented, which are used in different code
parts.
These more general functionalities are:

- ApplyGaussianNoise
- GetBin
- GetAmbiguousValue
- ReformatToRadarDetection

For more information regarding these functions please refer to the section in this documentation.

![block_diagram.png](../img/block_diagram.png)