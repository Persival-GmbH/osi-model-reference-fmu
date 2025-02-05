# OSI Model Reference FMU

This fmu receives a SensorView message and changes the model_reference fields of moving_objects and the static environment based on a JSON mapping file.
A JSON file is needed with key-value pairs.
For every model_reference of a moving_objects, the FMU checks if it is present as a key in the JSON file.
If so, the model_reference is replaced by the corresponding value.
The model_reference of the static environment can be changed with the key `static_environment`.

## Parameterization

The following FMI parameters can be set.

| Parameter                 | Description                                        |
|---------------------------|----------------------------------------------------|
| `model_reference_map_path | Absolute path to a JSON file with key-value pairs. |

## Build instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

With the cmake option `INSTALL_DIR` you can set the installation path of the FMU.
