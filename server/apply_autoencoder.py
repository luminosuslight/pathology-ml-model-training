from fastai.vision import *


class NeuralNetworkAutoencoder(object):
    progress = 0.0

    def __init__(self, model_path: str):
        """
        :param model_path: the path containing the 'export.pkl' file
        """
        self.learn = load_learner(model_path)
        self.learn.model = self.learn.model[:len(self.learn.model) / 2]

    def get_feature_vectors(self, img_path, cell_positions):
        img = open_image(img_path)
        feature_vectors = []

        for i, pos in enumerate(cell_positions):
            NeuralNetworkAutoencoder.progress = i / len(cell_positions)
            p, prediction, b = self.learn.predict(img.data[pos[0] - 32:pos[0] + 32, pos[1] - 32:pos[1] + 32])
            feature_vectors.append(prediction)

        NeuralNetworkAutoencoder.progress = 0.0
        return feature_vectors

    def destroy(self):
        self.learn.destroy()
