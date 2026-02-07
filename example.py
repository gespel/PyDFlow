from pydflow import PyDFlow
import time

p = PyDFlow("test", "0d:00.0", "0d:00.1")
p.create_testing_pipe()
p.create_entry()
time.sleep(3)
p.dump_pipe_information_for_port(0, 'p0.txt')
p.dump_pipe_information_for_port(1, 'p1.txt')
