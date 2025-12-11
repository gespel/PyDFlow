from pydflow import PyDFlow

p = PyDFlow("test")
p.create_testing_pipe()
p.dump_pipe_information_for_port(0, 'p0.txt')
p.dump_pipe_information_for_port(1, 'p1.txt')
