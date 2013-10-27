codedude
========

This is a general repository for test.

I won't add anything useful in here, if not my stupid tests.
I think.
Maybe.
Dunno.

Oct 27, 2013
------------
Today I experimented a bit with OpenCL. Coming from the CUDA experience I found
the OpenCL API pretty verbose... There is a simple example that shows it (the
full code is more than 100 lines just to sum two arrays). So, even if I like
OpenCL for its portability, but it is a verbosity hell, a dirty solution to
make its setup shorter has been created. All hail OCHELL, the one-header only
library for excaping the OpenCL verbosity hell! I plan to improve it in the
future (maybe using OOP, so that vars aren't continuously passed around).
In the specific opencl/README there are some links I found nice.
Anyhow, I like OpenCL because it is similar to OpenGL, because of ICD, and
because there is also a C++ specification which I prefer to C.

Oct 25, 2013
------------
I added a simple OpenGL 3/4 + FreeGLUT example. Why? Because it may be useful
to someone. I am using nVidia optimus on Fedora, and I wanted to test if OGL 3
and 4 could run. nVidia supports (right now) up to OpenGL 4.3, but if using
software renderer (MESA), I can get up to 3.1 (or 3.2, not sure). So, I created
a working example of the code used in the OpenGL Programming Guide (8th ed.)
which seems to work. Also, since GLEW crashed the program (and glewExperimental
fix did not fix), I used glLoadGen:

	https://bitbucket.org/alfonse/glloadgen/wiki/Home

A lua script that worked fairly well. Also, this example includes a routine to
load, compile and link shader files into a shader program, which may be useful
to beginners.

Aug 11, 2013
------------
Well, I just added something revolutionary: a 14-line decorator to quickly test
pycairo code. You're welcome, world. A very smart, innovative and elaborated
demo is present in pycairo_helloworld.py.
Well, I plan to include some features to allow simple animations (decorator
parameters are already present! But not used, yet), based on time or something.
