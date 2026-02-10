# PyDFlow
PyDFlow is a Python library for building and executing data processing pipelines using the DOCA Flow framework developed by NVIDIA. It provides a high-level API for defining and managing data flows, making it easier to create complex data processing workflows. PyDFlow abstracts away the complexities of working with DOCA Flow, allowing developers to focus on the logic of their data processing tasks rather than the underlying infrastructure. Its core features is the easy-to-use programming interface, which allows users to define data processing pipelines using Python code.
This library is part of my master studies at the University of Potsdam, and it is still in development. The current version may not be fully stable, and there may be bugs or missing features. However, I am actively working on improving the library and adding new features, so please feel free to use it and provide feedback. Also note that this library is not affiliated with NVIDIA or the DOCA Flow framework, and it is not an official product. It is a personal project that I am developing for educational purposes. Feel free to contact me via email at ```heimbrodt@uni-potsdam.de``` if you have any questions or suggestions regarding the library.
## Installation
To install PyDFlow, you can build it from source. First, clone the repository:
```bash
git clone https://github.com/gespel/PyDFlow.git
cd PyDFlow
```
Then, build the library using Meson:
```bash
meson setup build
ninja -C build
```
After building, you can install the library using pip:
```bash
pip install .
```
Alternatively, you can import the built library directly from the build directory without installing it:
```python
import pydflow
```
## Usage
Here is a simple example of how to use PyDFlow to create a data processing pipeline:
```python
import pydflow
# Create a new data flow with a name and specify the port 0 and port 1 BlueField PCIE devices
flow = pydflow.PyDFlow("MyDataFlow", "00:00.0", "00:00.1")
p = flow.create_pipe()
```
In this example, we create a new data flow named "MyDataFlow" and specify the BlueField PCIE devices to be used for data processing. We then create a new pipe within the data flow, which can be used to define the processing entries.

## Contributing
Contributions are welcome, but please keep in mind that the project is still in development and may not be fully stable. If you would like to contribute, please fork the repository and submit a pull request with your changes.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.