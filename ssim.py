import cv2, numpy as np

cover = cv2.imread("resources/pepper.bmp", cv2.IMREAD_COLOR)
stego = cv2.imread("out/pepper_lsbi.bmp", cv2.IMREAD_COLOR)

# 1) raw absolute difference
diff = cv2.absdiff(cover, stego)

# 2) enhance differences (scale & stretch to full range)
scale = 32  # try 16–64
enh = np.clip(diff.astype(np.int16) * scale, 0, 255).astype(np.uint8)
enh = cv2.normalize(enh, None, 0, 255, cv2.NORM_MINMAX)  # contrast stretch

cv2.imwrite("pepper_lsbi_diff_enhanced.png", enh)
