import os

import cv2
import numpy as np


class Yolo:
    def __init__(
            self,
            yolo_asset_dir: str,
            confidence: float = 0.5,
            threshold: float = 0.3,
    ):
        """
        Initialise the YoloOrchestrator with the required input parameters.

        Args:
            yolo_asset_dir (str):
                Base path to YOLO directory containing the three files
                describing the trained model. The three files should have the
                following file names, yolov3.names, yolov3.weights,
                and yolov3.cfg.

            confidence (float):
                Minimum confidence level. Only objects with a
                confidence level that is more than this confidence value will
                have a bounding box drawn around it. This value will be used
                to filter out weak detections.

            threshold (float):
                Threshold value that will be used when applying non-maxima
                suppression (NMS).

                NMS will be used to transform a smooth response map that
                triggers many imprecise object window hypotheses in, ideally,
                a single bounding-box for each detected object.

                This threshold represents the overlapping ratio on these
                imprecise object window hypotheses, and will be used as a
                gauge by NMS to combine these overlapping object window
                hypotheses. In such a case, if the overlapping ration is
                above the threshold value, these object window hypotheses
                will be combined together into a single bounding box that to
                be rendered.
        """
        self.confience = confidence
        self.threshold = threshold
        self.yolo_asset_dir = yolo_asset_dir

        # Load the class labels to the YOLO model that was trained on
        self.labels_path = os.path.sep.join(
            [self.yolo_asset_dir, "yolov3.names"]
        )
        self.labels = open(self.labels_path).read().strip().split("\n")

        # The class labels will be associated to a random color with each label
        # The assigned colors will be used to  annotate the resulting bounding
        # boxes and labels
        np.random.seed(42)
        self.colors = np.random.randint(0,
                                        255,
                                        size=(len(self.labels), 3),
                                        dtype="uint8")

        # Derive the paths to the YOLO weights and model configuration
        self.weight_path = os.path.sep.join(
            [self.yolo_asset_dir, "yolov3.weights"]
        )
        self.config_path = os.path.sep.join([self.yolo_asset_dir, "yolov3.cfg"])

        # Load our YOLO object detector trained on the provided model
        self.net = cv2.dnn.readNetFromDarknet(
            self.config_path,
            self.weight_path
        )

    def process_image(self, image_path: str):
        """
        Processes a given image by performing object detection on it. Should
        an object be detected, a bounding box will be drawn around it,
        annotated with the confidence level, and label id will also be
        annotated on the image.

        If an object be detected, the processed image will be created in the
        ~/images/found/path. The image file name will follow the structure of
        [timestamp]_[label_id]_found_[original_file_name].jpg.

        If an object is not detected, the processed image will be created in
        the ~/images/not_found/path. The image file name will follow the
        structure of [timestamp]_not_found_[original_file_name].jpg.

        Args:
            image_path (str):
                Absolute path to the image to be processed.

        Returns:
            None
        """
        # TODO: Implement this (Read the docstring to see what needs to be done)
        pass

    def process_video(self, video_path: str, reconstruct_vid: bool):
        """
        Processes a given video by segmenting the video into frames and
        performing object detection on each of the frames. Should an object
        be detected, a bounding box will be drawn around it, annotated with
        the confidence level, and label id will also be annotated on the image.

        If an object be detected, the processed frame will be created in the
        ~/images/found/path. The image file name will follow the structure of
        [timestamp]_[label_id]_found_[original_file_name].jpg.

        If an object is not detected, the processed frame will be created in
        the ~/images/not_found/path. The image file name will follow the
        structure of [timestamp]_not_found_[original_file_name].jpg.

        Args:
            video_path (str):
                Absolute path to the video to be processed.

            reconstruct_vid (bool):
                Boolean flag to indicate whether a video (based on the
                original provided video) with the bounding boxes (with
                confidence level, and label id) drawn around detected objects
                should be created.

        Returns:
            None
        """
        # TODO: Implement this (Read the docstring to see what needs to be done)
        pass
