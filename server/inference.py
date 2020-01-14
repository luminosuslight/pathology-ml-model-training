import fastai
from fastai.vision import *
from fastai.callbacks import *
import cv2
import PIL
import numpy as np

import math
import io
from copy import deepcopy

print("Fast.ai:", fastai.version.__version__)
print("OpenCV:", cv2.__version__)


class NeuralNetwork(object):

    def __init__(self, model_path: str) -> object:
        """
        :param model_path: the path containing the 'export.pkl' file
        """
        self.learn = load_learner(model_path)
        self.progress = 0.0

    def get_output_and_centers(self, img_path):
        img = open_image(img_path)
        print("Input image:", img)
        output_img_raw_data = img_to_buffer(self.predict_image(img))
        centers = get_cell_centers(output_img_raw_data)
        return output_img_raw_data, centers

    def predict_image(self, img):
        result = deepcopy(img)
        x_patches = math.ceil(img.shape[1] / 128)
        y_patches = math.ceil(img.shape[2] / 128)
        print(f"Predicting full image by splitting it up into {x_patches * y_patches} patches...")
        for px in range(x_patches):
            self.progress = px / x_patches
            print(f"Progress: {int(self.progress * 100)}%")
            for py in range(y_patches):
                x = px * 128
                y = py * 128
                ex = min(x + 256, img.shape[1])
                ey = min(y + 256, img.shape[2])
                try:
                    p, prediction, b = self.learn.predict(img.data[:, x:ex, y:ey])
                    if x < 128 or ex == img.shape[1] or y < 128 or ey == img.shape[2]:
                        # this is at the border, use full prediction:
                        # FIXME: there will be a border 256px from the left and bottom
                        result.data[:, x:ex, y:ey] = prediction
                    else:
                        # use only middle part to avoid border artifacts:
                        result.data[:, x + 64:x + 64 + 128, y + 64:y + 64 + 128] = prediction[:, 64:64 + 128,
                                                                                   64:64 + 128]
                except RuntimeError:
                    print("An error occurred during prediction of patch at", x, y, ex, ey)
                    result.data[:, x:ex, y:ey] = 0.0
        self.progress = 0.0
        return result


def get_cell_centers(image_buffer):
    print("Finding cell centers...")
    image_arr = np.fromstring(image_buffer, dtype='uint8')
    cv_img = cv2.imdecode(image_arr, cv2.IMREAD_UNCHANGED)
    print("CV image shape:", cv_img.shape)
    _, center_channel, nuclei_mask = cv2.split(cv_img)  # bgr format
    _, center_binary = cv2.threshold(center_channel, 127, 255, cv2.THRESH_BINARY)
    connectivity = 8  # You need to choose 4 or 8 for connectivity type
    num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(center_binary, connectivity, cv2.CV_32S)
    print("Nucleus Count: ", num_labels)
    nuclei_data = {'xPositions': tuple(centroids[:, 0]), 'yPositions': tuple(centroids[:, 1])}
    return nuclei_data


def img_to_buffer(img):
    x = np.minimum(np.maximum(image2np(img.data * 255), 0), 255).astype(np.uint8)
    with io.BytesIO() as output:
        PIL.Image.fromarray(x).save(output, format="PNG")
        contents = output.getvalue()
    return contents
