from pydflow import PyDFlow
import time


# Creating a DOCA Flow Pipe with the name test and the PCIe addresses of Port 0 and Port 1 of the Bluefield
p = PyDFlow("test", "03:00.0", "03:00.1")
p.create_pipe()
#p.create_testing_pipe()
#p.create_entry()
#time.sleep(3)
#p.dump_pipe_information_for_port(0, 'p0.txt')
#p.dump_pipe_information_for_port(1, 'p1.txt')
