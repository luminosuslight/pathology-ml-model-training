import PyDIP as dip
import cbor2
from skimage import io
import cv2
import PIL
import numpy as np

img = dip.ImageReadTIFF('NucleiLabels.tif') 
chain_codes = dip.GetImageChainCodes(img)
print("Nuclei found:", len(chain_codes))
centroids = []
for c in chain_codes:
    centroids.append(c.Polygon().Centroid())

centroids = np.asarray(centroids)

nuclei_data = {'xPositions': tuple(centroids[:, 0]), 'yPositions': tuple(centroids[:, 1])}

with open('nuclei.cbor', 'wb') as fp:
    cbor2.dump(nuclei_data, fp)
    
im = io.imread('NucleiLabels.tif')

im[im > 0] = 255
im[im <= 0] = 0
im = im.astype(np.uint8)

PIL.Image.fromarray(im).save('mask.png')
