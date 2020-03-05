import os
import time
from pathlib import Path
from typing import Union, Optional

import cv2
import numpy as np


class Yolo:
    def __init__(
            self,
            yolo_asset_dir: str,
            confidence: float = 0.5,
            threshold: float = 0.3
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
                hypotheses. In such a case, if the overlapping ratio is above
                the threshold value, these object window hypotheses will be
                combined together into a single bounding box to be rendered.
        """
        self.confidence = confidence
        self.threshold = threshold
        self.yolo_asset_dir = yolo_asset_dir

        # Load the class labels to the YOLO model that was trained on
        self.labels_path = os.path.sep.join(
            [self.yolo_asset_dir, "rpi.names"]
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
            [self.yolo_asset_dir, "rpi.weights"]
        )
        self.config_path = os.path.sep.join([self.yolo_asset_dir, "rpi.cfg"])

        # Load our YOLO object detector trained on the provided model
        self.net = cv2.dnn.readNetFromDarknet(
            self.config_path,
            self.weight_path
        )

        # Determine only the *output* layer names that we need from YOLO
        self.ln = self.net.getLayerNames()
        self.ln = [self.ln[i[0] - 1]
                   for i in self.net.getUnconnectedOutLayers()]

        # Debug flag, comment if you do not want to print debug strings
        self.is_debug = True

        self.output_video_path = "videos/"
        self.output_image_found_path = "images/found/"
        self.output_image_not_found_path = "images/not_found/"

        # Create the required output folders
        if not os.path.exists(self.output_video_path):
            os.makedirs(self.output_video_path)

        if not os.path.exists(self.output_image_found_path):
            os.makedirs(self.output_image_found_path)

        if not os.path.exists(self.output_image_not_found_path):
            os.makedirs(self.output_image_not_found_path)

    def process_image(self, image: Union[str, np.ndarray],
                      filename: Optional[str]) -> str:
        """
        Processes a given image by performing object detection on it. Should
        an object be detected, a bounding box will be drawn around it,
        annotated with the confidence level, and label id will also be
        annotated on the image.
        If an object be detected, the processed image will be created in the
        ~/images/found/image_name. The image file name will follow the
        structure of [timestamp]_[label_ids]_found_[original_file_name].jpg.
        If an object is not detected, the processed image will be created in
        the ~/images/not_found/image_name. The image file name will follow the
        structure of [timestamp]_not_found_[original_file_name].jpg.
        Args:
            image (Union[str, np.ndarray]):
                Absolute path to the image to be processed.
        Returns:
            None
        """
        # Determine the image input type and ensure that it is of the
        # numpy.ndarray type
        if isinstance(image, str):
            image = cv2.imread(image)
            image_file_name = Path(image).stem
        elif isinstance(image, np.ndarray):
            image_file_name = "vid_source"
        else:
            print("Invalid image input type detected. Exiting...")
            exit(2)

        # Get the image width and height
        (im_height, im_width) = image.shape[:2]

        # Construct a blob from the input frame and then perform a forward
        # pass of the YOLO object detector, giving us the bounding boxes
        # and associated probabilities.
        # Here we supply the spatial size that the Convolutional Neural
        # Network expects. YOLO requires a size of (416, 416)
        blob = cv2.dnn.blobFromImage(image, 1 / 255.0, (416, 416),
                                     swapRB=True, crop=False)
        start = time.time()
        self.net.setInput(blob)
        layer_outputs = self.net.forward(self.ln)
        end = time.time()

        # Show timing information on YOLO
        if self.is_debug:
            print("[INFO] YOLO took {:.6f} seconds".format(end - start))

        # Initialize our lists of detected bounding boxes, confidences,
        # and class IDs, respectively
        boxes = []
        confidences = []
        class_ids = []

        # Loop over each of the layer outputs
        for output in layer_outputs:
            # Loop over each of the detections
            for detection in output:
                # Extract the class ID and confidence (i.e., probability)
                # of the current object detection
                scores = detection[5:]
                class_id = np.argmax(scores)
                confidence = scores[class_id]

                # Filter out weak predictions by ensuring the detected
                # probability is greater than the minimum probability
                if confidence > self.confidence:
                    # Scale the bounding box coordinates back relative to
                    # the size of the image, keeping in mind that YOLO
                    # actually returns the center (x, y)-coordinates of
                    # the bounding box followed by the boxes' width and
                    # height
                    box = detection[0:4] * np.array(
                        [im_width, im_height, im_width, im_height]
                    )
                    (center_x, center_y, box_width, box_height) = box.astype(
                        "int")

                    # Use the center (x, y)-coordinates to derive the top
                    # and and left corner of the bounding box
                    x = int(center_x - (box_width / 2))
                    y = int(center_y - (box_height / 2))

                    # Update our list of bounding box coordinates,
                    # confidences, and class IDs
                    boxes.append([x, y, int(box_width), int(box_height)])
                    confidences.append(float(confidence))
                    class_ids.append(class_id)

        # Apply non-maxima suppression to suppress weak, overlapping
        # bounding boxes
        idxs = cv2.dnn.NMSBoxes(
            boxes,
            confidences,
            self.confidence,
            self.threshold
        )

        # Ensure at least one detection exists
        if len(idxs) > 0:
            detected_label_ids = []
            # Loop over the indexes we are keeping
            for i in idxs.flatten():
                # Extract the bounding box coordinates
                (x, y) = (boxes[i][0], boxes[i][1])
                (w, h) = (boxes[i][2], boxes[i][3])

                # Draw a bounding box rectangle and label on the frame
                color = [int(c) for c in self.colors[class_ids[i]]]
                cv2.rectangle(image, (x, y), (x + w, y + h), color, 2)
                text = "{}: {:.4f}".format(self.labels[class_ids[i]],
                                           confidences[i])
                print(text)
                cv2.putText(
                    image,
                    text,
                    (x, y - 5),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.5,
                    color,
                    2
                )
                detected_label_ids.append(self.labels[class_ids[i]])

            # Determine output directory
            if not filename:
                filename = "{timestamp}_{label_id}_found_{" \
                           "original_file_name}.jpg".format(
                    timestamp=int(time.time()),
                    label_id="-".join(detected_label_ids),
                    original_file_name=image_file_name
                )
            else:
                # TODO: Resolve file name over here
                # Need to determine the coordinates base on the bbox coordinates
                # (x, y) <- center of bounding box
                # (x, y) <- dimension of bounding box
                pass

            # Write detected image to found directory
            cv2.imwrite(
                self.output_image_found_path + filename,
                image
            )

            return filename
        else:
            # No objects detected
            # Write image with nothing detected to not_found directory
            # cv2.imwrite(
            #     self.output_image_not_found_path +
            #     "{timestamp}_not_found_{original_file_name}.jpg".format(
            #         timestamp=int(time.time()),
            #         original_file_name=image_file_name
            #     ),
            #     image
            # )

            return None

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
                If the flag is set (true) the reconstructed video will be
                saved to ~/videos/[timestamp]_out.avi.
        Returns:
            None
        """
        v_writer = None
        epoch_ts = int(time.time())

        v_cap = cv2.VideoCapture(video_path)
        while v_cap.isOpened():
            # Read the next frame from the file
            (grabbed, frame) = v_cap.read()

            # If the frame was not grabbed, then we have reached the end of
            # the stream
            if not grabbed:
                break

            # Proccess frame as image
            self.process_image(frame)

            # Check if the video writer is None
            if v_writer is None:
                # NOTE: This might not be working properly yet
                # Initialize our video writer
                fourcc = cv2.VideoWriter_fourcc(*"mp4v")
                v_writer = cv2.VideoWriter(
                    "%s/%s_out.mp4" % (self.output_video_path, epoch_ts),
                    fourcc,
                    3,
                    (frame.shape[1], frame.shape[0]),
                    True
                )

            # Check if video should be reconstructed
            if reconstruct_vid:
                v_writer.write(frame)

        # Release the file pointers
        print("[INFO] cleaning up...")
        v_writer.release()
        v_cap.release()
