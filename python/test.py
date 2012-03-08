from scipy import ndimage,signal,misc
import numpy as np

img = ndimage.imread('familycropfull.jpg')
img_guess = ndimage.imread('familycropfull.jpg')
im2 = ndimage.interpolation.rotate(img,180)
psf = signal.fftconvolve(img,im2,mode='same')
k = 10 
n = 6 
m = 3 

for l in xrange(k):
	print 'l = '+str(l)
	for m in xrange(n):
		conv = signal.fftconvolve(img,psf,mode='same')
		division = np.divide(img,conv)
		mirror = ndimage.interpolation.rotate(img,180)
		conv = signal.fftconvolve(mirror,division,mode='same')
		psf = np.multiply(conv,psf) 
	misc.imsave('psf-'+str(l)+'.jpg',psf)
	for i in xrange(m):
		conv = signal.fftconvolve(img_guess,psf,mode='same')
		div = np.divide(img,conv)
		mirror = ndimage.interpolation.rotate(psf,180)
		corr = signal.fftconvolve(div,mirror,mode='same')
		img_guess = np.multiply(img_guess,corr)
	misc.imsave('fixed-'+str(l)+'.jpg',img_guess)

misc.imsave('fixed.jpg',img_guess)
#print result
#misc.imsave('convolved.jpg',result)
