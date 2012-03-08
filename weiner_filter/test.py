import Image
from scipy import signal, misc
import scipy
import numpy as np
import sys
import re

src = sys.argv[1]
im = misc.imread(src)

print im

filtered = signal.wiener(im)

print np.int8(filtered)

#m = re.search('(.*)\.(.*)',src)

misc.imsave('wiener.jpg',np.int8(filtered))
