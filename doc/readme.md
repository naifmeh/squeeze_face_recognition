# Documentation for CNN project on embedded system

## CNN :

A Convolutional Neural Network is a derived form of Artificial Neural Networks
that is particularly fitted for patterns recognition. Such neural networks are very
intensive in ressource use, which makes it difficult for them to be used on small, ressource
limited embedded systems.
The aim of the project is to implement a lightweight CNN using C++, optimizing the heavy
computations and use a light model suchas SqueezeNet in order to proceed to face reognition on
embedded systems.
It is to be noted that the training phase is to be performed on GPU powered laptop as it would take
hours or days to train it on a limited CPU, or even on the most powerful CPUs actually developed.

What is the theory behind the CNN ?
-------------------------------------------------------------------------
The CNN architecture is essentially composed of convolution layers followed by
pooling layer. Further description of those terms are to be discussed later on.
The CNN takes as an input chunks of pixels and output a probability number comprised
between 0 and 1 for every registered labels.
The convolution plus pooling layers are usually followed by fully connected (FC)
layers.
If a classification is necessary, as in our project, where we need to classify either faces
or objects, a final softmax layer is necessary.

Activation function
----------------------

An activation function allows non linearity in a ANN. It´s features are composed of :
* non linearity
* differentiable everywhere
* Extended
* Monotone
* Identity in x=0

Commonly used activation functions in ANNs are :
* Identify functions
* Heaviside
* Sigmoid

Convolution
----------------------
Convolution is a mathematical operation to merge two sets of informations. It is very used in
the field of electrical engineering, where the convolution can output similarities between two signals.
In the CNN, a convolution is applied on ourinput data, using a convolution filter, in order to produce a feature map. The filter is equally named kernel. It is usually a matrix or a tensor of smaller size than the original input.
If the kernel is of size 3x3, the convolution is called 3x3 convolution.
The convolution is obtained by sliding the filter over the input. At every location, we do element wise
matrix multiplication and sum the result, which goes on the feature map.
The area that is feature by the kernel is called *receptive field*.
The kernel is then slided to the right by a number of rows named the *stride*.

Convolutions are most commonly performed in 3D on colored images rather than 2D (luminance images). The kernel should accordingly be of 3x3x3 size in order to perform a 3X3X3 convolution.

The convolutions layer are often of different convolution size and are then stacked to form a full feature map.

When dealing with high diçensional inputs, like images, it is totally impractical to connect neurons to all neurons in the previous layer.
This way of proceeding is called the receptive field of the neuron (analog to the filter size).
The number of connection on a l+1 neuron, connected to a n neuron is equal to the dimension of the filter.



Ex: Having a 32x32x3 and then using a filter of size 5x5x3, the difference with the previous description of a 2D conv is that we will sum the result of the 3 depth-wise convolutions, resulting in a feature map of 32x32x1.
If 5 conv layers would have been used, regardless of the kernel dimension, we would end up with a 32x32x5 feature map.


Non-linearity layers
--------------------------

A neural network, composed of multiple layers with many neurons, can be represented as a unique neuron
if we had a linearity at each layer. In order to avoid this, which would make our network useless at performing decisionss, it is necessary to add non linearity. As soon as the convolutional layers are done, we need to pass the result of the convolution operation through relu activation function.

Stride and padding
--------------------------

The stride, as mentionned earlier, specify by how much we move the filter at each step. It defines the size of the resulting feature map.
The use of strides come with a constraint, let´s imagine for example an imput of size 10 (i.e 10x10), using no padding, with a filter size of 3. A stride of 2 would be completely impossible to implement as we would end up with a 4.5 size output.
![](https://latex.codecogs.com/svg.latex?(W-F&plus;2P)/(S&plus;1)&space;=&space;4.5)
As the result is a floating number, it is commonly said that the neurons don´t fit symmetricallya ccross the input. The setting of this particular parameter is therefore consiodered invalid.
The padding, as opposed to the stride, allows us to keep the input size, by adding paddings to surround the input with zeros.
Padding are always used in CNNs.

Pooling layers
----------------------------

Pooling allows to reduce the size of feature maps, which enable us to reduce the number of parameters,
thus shortening training time and response output.
The pooling more commonly used is the max pooling which just takes the maximum value in the pooling window which is of usually of small dimension, with a defined stride.
Pooling keep the depth of the feature map intact.
If we have a feature map of dimension 32x32x10 and a pooling with a 2x2 windows using a stride of 2, we will end up with a 16x16x10 feature map.

In most CNN architectures, pooling is typically performed with a 2x2 window and a stride of 2 while the convolution is performed using a 3x3 window, stride of 1 and using padding.

Hyper-parameters of the CNNs
----------------------------------

Four important parameters are decided on CNNs :
* Filter size, as most of the time, 3x3 kernels are used but some other dimensions can also be used depending on the application.
* Filter count, which is a power of two comprised between 32 and 1024. More filters output a more accurate result, but risk overfitting and obviously use more computation ressources.
A small number of filter are usually used in the initial layers and increased as we go deeper in the network.
* The stride is usually kept at a value of 1 but in order to optimie our network, this parameter is to be discussed.
* Padding: the parameters is also to be discussed as it is undoubtedly being used.

### Parameters sharing

Parameter sharing is most often used in CNNs in order to limit the number of parameters.
For instance, the Krizhevsky et al. architecture, winner of the 2012 ImageNet challenge consisted of 55*55*96 = 290.400 neurons in the first convolutional layer only, and each neuron disposed of 11*11*3 = 363 weights and one bias. This end up giving 290400*3363=105705600 parameters on only the first layer which is extremely high and undoable on most ressource limited systems, regarding the available memory and also the CPU core number along with it´s speed.
Using parameter sharing can reduce significantly the number of parameters by issuing an hypothesis :
* if one feature is useful to compute at (x,y), then it should also be useful to compute at a different positon (x1,y1).
In the Krizhevsky et al. architecture, using parameter sharing in order to constraint the use of the same weihgts and bias in neurons of each depth slice of the layer. Say we have 96 slices, and 96 unique sert of weights, we are going to end up with a total of 96*11*11*3 = 34848 unique weights, which we add to the 96 bias. The number of parameters is drastically reduced compared to the previous approach, and we now have chances of computing it on a low ressource CPU.

Fully connected layers
-------------------------

In CNNs, fully connected layers are always used after the convolutions layers to wrap up the CNN archtecture.
FC layers expect a 1D vectore of numbers, so we have to flatten the output of the ultimate pooling layer which is basically just a rearrangement of 3D vectors in a 1D.

Training
-------------------------

Training is realized using backpropagation, which is to be discussed as we dig further in the functionning of the ANN and it´s perspective of evolutions.
Training in CNNs are more computational heavy as the convolution is pretty ressource eating.


Global vision of the CNNs
------------------------------------------------------------------------------------

The CNN can be seen as a suite of a feature extraction followed by a classification part. Convolution and pooling perform feature extraction.
The classification is performed by the FC layers.

How to implement a CNN ?
-------------------------------------------------------------------------------------

Dropout
-----------------

Dropout allows a subsequent gain of up to 2% in precision. It is used to limit overfitting. The idea behind is that at run time, at each iteration, a neuron is dropped with a probability p, which çeans that all inputs and outputs of this neuron are turned off for this iteration.
At each iteration, the dropped neuron can be reactivated.
p is called the dropout rate and is usually around 0.5, which means that any neuron has 50% of chance to being dropped out.
The dropout is working because it is preventing the network to be dependent of a number of neurons and force independance in each neuron.

Dropout is only applied during training time.

An alternative to dropout is batch normalization which is to be analyzed.

Overfitting
----------------------------

I previously mentionned overfitting, but without going deeper into explanations, which will be rectified in the following :

"With four parameters I can fit an elephant, and with five I can make him wiggle his trunk." John von Neumann

This quote is pretty descriptive of overfitting.
It basically define a model that define the training data way too well and output perfect result for the training data. It is fitted for it and the parameters are modeled after it. However, for a set of new real world data, the accuracy of the model drop out subsequently.
Overfitting happens when the accuracy over the training batch start to get stuck.

Avoiding overfitting
---------------------------------

Data augmentation
---------------------------------

Data augmentation enriches the training data by generating new examples via random transformation of existing ones.
It is done dynamically at training time.
Common transformations are rotation, shifting, resizing, adjusting exposure, etc...

Implementing convolution
-----------------------------------
Implementing this layer essentially consist of resolving dot products between filters and regions of the input. We wish to optimize to maximum this operation as the more the size of the input is high, the more computation we have to observe.
Say we have an image size of H*W and a filter size of k*j. The computational complexity of such a convolution would be of :
![](https://latex.codecogs.com/svg.latex?O(H\cdot&space;W\cdot&space;k\cdot&space;j)).

In order to implement an efficient convolution on ressource limited systems, it is necessary to optimize the computation of the convolution.
Ways of doing so will be discussed in further sections.

### Stacking convolutions

In state-of-art to this day, stacking convolution layer is very used in order to limit computational use of ressources, as two 3x3 convolutions would produce the same output as a 5x5 convolution.
Models such as VGG16 or AlexNet take advantage of stacking convolutions layers, which come along with the advantage of using two ReLu layers resulting in an increased non linearity, giving more power to the model.

### Different types of optimized convolutions :

#### Dilated convolution
A [paper by Fisher Yu and Vladlen Kotlun](https://arxiv.org/abs/1511.07122) has discussed the introduction of one more parameter of the convolution layer, referred as the dilation. A quick summary of the dilation is a filter that have spaces between each cell, a dilation.
Let´s say we have a filter v of size 3. A dilation of 0 would end up with a convolution result of w[0] * x[0] + w[1] * x[1] + w[2] * x[2], while inserting a dilation of 1 would end up with : w[0] * x[0] + w[1] * x[2] + w[2] * x[4].
One might wonder the benefits of introducing such a parameter as it increase the number of hyperparameters of the architecture. However, introducing dilation along with filters with no dilation allows us to merge spatial informations accorss the inputs much more efficiently while reducing the number of layers.

### Backpropagation for convolution layers

Backpropagation is defined by a backward pass for a convolution. The difference with the forward pass is that the weights are actually flipped.



# Implementing using tiny-dnn

Tiny-dnn is a lightweight, optimized deep learning library, consisting uniquely of headers, implemented in c++14, thus allowing it's use on multiple systems; ranging from desktop computers to embedded systems.
Tiny-dnn is open-source, which allows us to modify it's source code for this project, removing the unecessary features that may slow us down.
It is GPU-capable, meaning the training can be realized with both GPU and CPUs. It support openMP multithreading, and AVX architecture optimization, thus introducing a consequent gain of speed, both in training and classifying.
The raspberry pi 3B consist of a quad core CPU, unfortunately not supporting AVX, but however allowing the use of openmp.

### Proceeding :

The process of face recognition is a pretty complex problem, significantly more complex than object or face detection as this turn into only a step of the problem.
In order to put a name to an input face, it is necessary to detect a CNN, or some mainstream machine learning algorithm.
The option to be chosen is the one that will provide the most efficient results in respect to time performance.
The face is then passed into a neural network, whose architecture is to be discussed, outputting face measurements, such as distance between the eyes, distance between nose and ears, etc...
This way of proceeding is decided as comparing faces as we would do for object recognition, such as using the last classifying layer to classify object into classes, would be too time consuming, as the training dataset would generate thousands of classes.
Instrad, the previously described way is much more efficient, as a simple classifer would be able to compare the measurements outputs to the ones we know and issue a percentage of matching.
The only issue in the proceeding would be the training set as millions of labeled images are necessary in order to get an acceptable result with an error rate < 10 %.

#### Which model is to chose ?

The model is to be chosen regarding of the ressources availability of the system. Models such as AlexNet, using multiple millions of parameters are very difficult to execute on a limited system, thus, we should focus on slightly more optimized model and use the previous ones only if the optimized models turn out to be badly suited for our problem.

##### SqueezeNet

#### FaceNet

Make heavy use of the inception module, introduced by Google.

##### Openface and nnX models

![](https://raw.githubusercontent.com/cmusatyalab/openface/master/evaluation/lfw.nn4.small1.v1/roc.png)
Previous image is a benchtest of the nn4 small model.

Openface is a pretty robust API written in LUA and Python.
However, what is of interest for us are the pre-trained models that they provide : specially the nn4 small model, which, compared to their original NN2 model, as described in the FaceNet paper, reduces by half the number of hyperparameters, thus the computation time, which is suitable for our application.
The nn4 model is implemented using Torch, and output 128 measurement of an aligned face as an input.

##### AlexNet

##### VGG


#### Which classifier to chose ?

##### The linear support vector machine

The linear support vector machine is one of the simplest and most efficient machine learning classifier as it had been around since the 60's and have been subject to tremendous ameliorations and optimizations.
It achieves a very good accuracy on datasets with multiple features.

##### The random forest tree classifier

Random forest is a supervised learning algorithm, meaning, we must provide a dataset composed of labels and values in order to train the classifier.
Random forest creates a large number of trees, thus the word Forest in it's name, and use those multiples trees to output a decision by using randomness.
Below is an image describing accurately the functionning of the Random forest :

![](https://cdn-images-1.medium.com/max/800/0*tG-IWcxL1jg7RkT0.png)




##### Training the classifier

As our initial dataset of pictures is predicted to be significantly small, *data augmentation* seems necessary in order to perform an efficient training of the classifier.
An image is previously passed through e first stage of our pipeline, which detect, align and extract a face from an image. It is then fed to the CNN which will output the 128 face measurements, to be stored.
Following the result of every image used for the training, we use the generated set of label and value in order to train the classifier.

Data augmentation consists in generating a subsequent subset of images from a smaller set. One mistake to avoid when performing data augmentation is to not fall into useless techniques.
For instance, it is not very necessary to rotate a face picture by 180 degrees for us, knowing that all our inputs are already aligned pictures. However, flipping, tilting and applying a gaussian blur to the pictures would seem necessary and logical and would allow us to multiply our dataset by a minimum factor of 2. From a small set of 20 pictures, a minimum of 40 sub pictures are to be obtained.


# Code dependencies

* OpenCV > 3.1 (New tracking library required)
* cppkafka -librdkafka (gets isntalled to usr/local/lib, move to /usr/lib to get it to work)
* boost 
* restclient - curl
* rapidjson (headers only)


# Links
[http://cs231n.github.io/convolutional-networks/] (1)
[http://www.jefkine.com/general/2016/09/05/backpropagation-in-convolutional-neural-networks/] (2)
