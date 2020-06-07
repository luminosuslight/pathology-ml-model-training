# Model Training and Application User Guide

## Model Training

The following section describes how the three training phases are actually implemented in the tool. While this looks like a lot of steps, the whole process takes only about 20 minutes until the training starts.

### Phase A: The actual steps to train a basic model in the application will look like the following:


-  **Annotate few example cells on a real image, assisted by traditional algorithms:** Create a __View__ and drag'n'drop an image showing the DAPI staining onto the application. Activate the 'Interactice Watershed Mode' in its settings. Pan and zoom in the view to find a set of cells that are representative of the others. Create a __Dataset__ and connect a __Visualize__ block to it. Click on the center of a cell, hold down the mouse button and drag to its border. The outline of the cell should be found automatically as a region grow algorithm is applied inside the grey circle that appeared. After annotating a few cells, it is also possible to just click on the center of a cell that has a clear outline to let the program guess it, based on the best outline within the range of cell sizes previously annotated. Annotate 10 to 20 adjacent cells, ideally small as well as large cells and cells with different kinds of shapes.
    
-  **Randomly generate similar cell instances:** Create a __Random Cell Generator__ and connect the __Dataset__ from the last step to the example input. Connect a new __Dataset__ and click 'Generate'. This will randomly create cell positions and shapes, based on the provided examples. Repeat this step with a lower cell count for the validation data.
    
-  **Render artificial input images:** Until now, the cells don't have a visual appearance. Connect the new __Dataset__ to a __Cell Renderer__, select 'DAPI' and click 'Save'. This will draw the given cells using random gradients on an image. Repeat this step for the validation data.
    
-  **Render target images:** To train the network, a target is required to tell the network what it should predict and to calculate the loss against. For the basic network a mask of the cells areas is needed for the semantic segmentation and an image showing a dot at the center of each cell is used to localize the individual cells. Create a 'Mask' and 'Center' image by selecting the types on the __Cell Renderer__ and clicking save. Repeat this for the validation data.
    
-  **Pre-process training data:** Create a __Training Data Preprocessing__ block and connect the artificial DAPI channel to all of its input channels. Connect the mask image to the first target channel and the center image to the second. Set the patch count to 5000 and click 'Generate All'. This will randomly crop equivalent patches from the input and target image, augment the input patches with noise and brightness changes and store all the patches in a single file that can be read by the backend. Repeat this for the validation data but with only 100 patches.
    
-  **Train the network:** Create a __CNN Training__ block and connect the just created training and validation data to the respective inputs. Enter a descriptive name for the model, ideally containing the required inputs and the type of the result. Select a maximum number of epochs; the training will stop automatically if the validation loss is increasing before that. Click 'Run'. A new block will appear with an indicator showing the progress of the training. After a few hours the training will be done and model can be used for inference.


### Phase B: This is how the model is then fine tuned with real images:


-  **Apply basic model to a real image:** See \cref{sec:model-application__ on how to do this.
-  **Fine tune the result:** Pan and zoom to look for bad segmentations by hand or try to filter potentially false predictions for example on a plot showing the radius and the pixel value of a cell in the CNN output image. Delete incorrect segmentations by entering the 'Add' mode in the view and clicking on the center of the wrong cell. Replace it with a correct one by either activating the 'Interactive Watershed Mode' for an image as in phase A or by just adding a cell center and applying the region grow algorithm afterwards again. The latter works best if the semantic segmentation is already correct but not the cell centers.
-  **Render new target images:** The input image in this phase is the real DAPI channel and doesn't have to be generated, but we still need the target images. Connect the __Dataset__ with the fine tuned results to a __Cell Renderer__. Generate a 'Mask' and 'Center' image as before.
-  **Pre-process training data:** As in phase A, create a __Training Data Preprocessing__ block, but this time connect the real DAPI channel to all of its input channels. Connect the 'Mask' and 'Center' channel as before. Connect a __Region__ block and select a large part of the image. Generate 1000 patches for the training data and repeat the step with a smaller, not overlapping region and only 100 patches for the validation data.
-  **Train the network:** Create a __CNN Training__ block, connect the training and validation data and enter a name. Now connect the __Model__ block from phase A to the base model input. That means, that the training will start with the weights from the first model. Enter a small amount of epochs to not overfit on this specific kind of image. Start the training by hitting 'Run'.


### Phase C: In the last phase the model will be retrained to output additional features. This can be done as follows:


-  **Apply fine tuned model to a real image:** See the explanation in \cref{sec:model-application__ on how to do this.
-  **Select cells of interest:** Find all cells with a certain characteristic that the network should learn to detect and select them by clicking them in the view after entering the 'Select' mode.
-  **Create new feature:** Add a __Feature Setter__ block and connect it to the output of your __Visualize__ block. Enter a name for the new feature and select a value for the selected cells, the default value for all other cells is 0.0. Click on 'Set' to store the new feature in the __Dataset__.
-  **Render new target images:** Create a __Cell Renderer__ block and connect the __Dataset__ to it. Select the new feature and hit 'Save'. The 'Mask' and 'Center' images from phase B can be reused.
-  **Pre-process training data:** Create a __Training Data Preprocessing__ block, connect the real DAPI image to the input channels, the mask to the first target channel and the center image to the second target channel. In addition, connect the new feature image to the third target channel. The network will learn to output predictions for this feature in the third output channel. Connect a __Region__ block and create the training data using a large region and the validation data using a smaller region and less patches.
-  **Train the network:** Connect the training and validation data to a __CNN Training__ block, enter a name and the epoch count, connect the model from phase B as the base model and click on 'Run'.


## Model Application

This is how the final model can be applied to an unseen image to get the cell segmentation:


-  **Define the input:** Create a __CNN Inference__ block and connect the required input images to its three channels. Most of the time the DAPI channel is used for all of them.
-  **Selection a region:** Optionally create a __Region__ block and connect it to the __CNN Inference__ block to only apply the neural network to a part of the input. This can speed up the process for example to quickly test a new model.
-  **Apply the model:** Connect a previously created model. If no model is connect, the default one is used. Hit 'Run' and wait for the inference to complete.
-  **Run region grow algorithm:** After the back end completed the inference, two new blocks will appear: the output of the neural network in the form of an image and a new __Dataset__. The latter one will already contain the positions of all connected components on the second channel of the output, most of the time the centers of the cells. Connect this __Dataset__ to a __Visualize__ block and to the 'Cells' input of a __Region Grow__ block. Connect the CNN output image to the mask input. Hit 'Run' and you should see the algorithm in action as it finds the final boundaries of the cells by expanding from the given cell centers up to the end of the mask.
 
