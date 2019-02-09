import time

start = time.time()

import argparse
import os
import cv2
import pickle
import sys

from operator import itemgetter
import numpy as np
np.set_printoptions(precision=2)
import pandas as pd

import openface

from sklearn.svm import SVC
from sklearn.preprocessing import LabelEncoder

fileDir = os.path.dirname(os.path.realpath(__file__))
modelDir = os.path.join(fileDir, './openface/', 'models')
openfaceModelDir = os.path.join(modelDir, 'openface')

def getRep(imgPath):
    start = time.time()
    bgrImg = cv2.imread(imgPath)

    if bgrImg is None:
        raise Exception("Unable to load image: {}".format(rgbImg.shape))

    rgbImg = cv2.cvtColor(bgrImg,cv2.COLOR_BGR2RGB)

    rep = net.forward(rgbImg)

    return rep

def infer(imgPath):
    with open(args.classifierModel, 'rb') as f:
        if sys.version_info[0] < 3:
            (le, clf) = pickle.load(f)
        else:
            (le,clf) = pickle.load(f, encoding='latin1')
    rep = getRep(imgPath)
    rep = rep.reshape(1,-1)
    predictions = clf.predict_proba(rep).ravel()
    maxI = np.argmax(predictions)
    person = le.inverse_transform(maxI)
    confidence = predictions[maxI]
    print("{} {:.2f}".format(person.decode('utf-8'), confidence))

if __name__ == '__main__':

    parser = argparse.ArgumentParser()


    parser.add_argument(
        '--networkModel',
        type=str,
        help="Path to Torch network model.",
        default=os.path.join(
            openfaceModelDir,
            'nn4.small2.v1.t7'))
    parser.add_argument('--imgDim', type=int,
                        help="Default image dimension.", default=96)
    parser.add_argument('--cuda', action='store_true')
    parser.add_argument('--verbose', action='store_true')

    subparsers = parser.add_subparsers(dest='mode', help="Mode")
    trainParser = subparsers.add_parser('train',
                                        help="Train a new classifier.")
    trainParser.add_argument('--ldaDim', type=int, default=-1)
    trainParser.add_argument(
        '--classifier',
        type=str,
        choices=[
            'LinearSvm',
            'GridSearchSvm',
            'GMM',
            'RadialSvm',
            'DecisionTree',
            'RandomForest',
            'GaussianNB',
            'DBN'],
        help='The type of classifier to use.',
        default='LinearSvm')
    trainParser.add_argument(
        'workDir',
        type=str,
        help="The input work directory containing 'reps.csv' and 'labels.csv'. Obtained from aligning a directory with 'align-dlib' and getting the representations with 'batch-represent'.")

    inferParser = subparsers.add_parser(
        'infer', help='Predict who an image contains from a trained classifier.')
    inferParser.add_argument(
        'classifierModel',
        type=str,
        help='The Python pickle representing the classifier. This is NOT the Torch network model, which can be set with --networkModel.')
    inferParser.add_argument('imgs', type=str, nargs='+',
                             help="Input image.")
    inferParser.add_argument('--multi', help="Infer multiple faces in image",
                             action="store_true")

    args = parser.parse_args()
    if args.verbose:
        print("Argument parsing and import libraries took {} seconds.".format(
            time.time() - start))

    if args.mode == 'infer' and args.classifierModel.endswith(".t7"):
        raise Exception("""
            Torch network model passed as the classification model,
            which should be a Python pickle (.pkl)
            See the documentation for the distinction between the Torch
            network and classification models:
                    http://cmusatyalab.github.io/openface/demo-3-classifier/
                    http://cmusatyalab.github.io/openface/training-new-models/
            Use `--networkModel` to set a non-standard Torch network model.""")
    start = time.time()
    net = openface.TorchNeuralNet(args.networkModel, imgDim=args.imgDim,
                                  cuda=args.cuda)
infer(args.imgs[0])