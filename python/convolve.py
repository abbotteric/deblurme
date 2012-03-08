from scipy import signal,misc
import sys
import numpy as np

src = sys.argv[1]

im = misc.imread(src)

out = convolve2d(im,im,fft=1)

misc.imsave('savetest.jpg',im)
