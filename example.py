import pydflow
import time


# Creating a DOCA Flow Pipe with the name test and the PCIe addresses of Port 0 and Port 1 of the Bluefield
pyd = pydflow.PyDFlow("test", "03:00.0", "03:00.1")
pipe = pyd.create_pipe()