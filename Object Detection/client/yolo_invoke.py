from YoloOrchestrator import Yolo
import argparse


def main():
    """
    Main entry point should go here.
    """
    # # Construct the argument parse and parse the arguments
    # ap = argparse.ArgumentParser()
    # ap.add_argument(
    #     "-i",
    #     "--image",
    #     required=True,
    #     help="path to input image"
    # )
    # ap.add_argument(
    #     "-a",
    #     "--asset",
    #     required=True,
    #     help="Base path to directory containing YOLO model files"
    # )
    # ap.add_argument(
    #     "-c",
    #     "--confidence",
    #     type=float,
    #     default=.5,
    #     help="Minimum probability to filter weak detections"
    # )
    # ap.add_argument(
    #     "-t",
    #     "--threshold",
    #     type=float,
    #     default=.3,
    #     help="Threshold when applying non-maxima suppression"
    # )
    # args = vars(ap.parse_args())

    yolo = Yolo(yolo_asset_dir="../model", confidence=.5, threshold=.3)

    # yolo.process_video("http://192.168.101.1:8000/stream.mjpg", True)
    yolo.process_image("fixed.jpg", parent_dir="../client/images/not_found/")


if __name__ == "__main__":
    main()
    print("Recognition stopped")
