import cv2
import sys

img = cv2.imread(sys.argv[1])

ycrcb = cv2.cvtColor(img, cv2.COLOR_BGR2YCR_CB)
channels = cv2.split(ycrcb)

clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
clahe.apply(channels[0], channels[0])

cv2.merge(channels, ycrcb)
cv2.cvtColor(ycrcb, cv2.COLOR_YCR_CB2BGR, img)

cv2.imwrite(sys.argv[1], img)
