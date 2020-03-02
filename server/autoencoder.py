from fastai.vision import ImageImageList, open_image, Image
from fastai.layers import PixelShuffle_ICNR, conv_layer, Flatten, ResizeBatch
from torch import nn


class Autoencoder(nn.Module):
    def __init__(self):
        super(Autoencoder, self).__init__()
        self.print_shape = True

        self.encoder = nn.Sequential(
            conv_layer(3, 8),  # 8, 64, 64
            nn.AvgPool2d(2, ceil_mode=True),  # 8, 32, 32
            conv_layer(8, 8),  # 8, 32, 32
            nn.AvgPool2d(2, ceil_mode=True),  # 8, 16, 16 -> 2048
            Flatten(),
            nn.Linear(8*16*16, 128))
        self.decoder = nn.Sequential(
            nn.Linear(128, 8*16*16),
            ResizeBatch(8, 16, 16),
            PixelShuffle_ICNR(8, 8),  # 8*32*32
            nn.ReLU(True),
            conv_layer(8, 8),
            PixelShuffle_ICNR(8, 8),  # 8*64*64
            conv_layer(8, 3))

    def forward(self, x):
        x = self.encoder(x)
        if self.print_shape:
            print("Shape after bottleneck:", x.shape)
            self.print_shape = False
        x = self.decoder(x)
        return x


class CellAutoencoderItemList(ImageImageList):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.current_image_filename = None
        self.current_image = None

    def open(self, item):
        filename, pos = item
        if self.current_image_filename != filename:
            self.current_image = open_image(filename)
            self.current_image_filename = filename
        # pos is (x, y) center position of a cell on the provided image,
        # we will take a 64x64px patch from this location:
        x = int(pos[0])
        y = int(pos[1])
        # note: fast.ai Image dimensions are (c, y, x)
        return Image(self.current_image.data[:, y-32:y+32, x-32:x+32])
