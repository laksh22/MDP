import argparse
import io
import logging
import socketserver
import sys
from http import server
from threading import Condition

import picamera

# Declare constants
WIDTH = 640
HEIGHT = 480
DEFAULT_PORT = 8000
DEFAULT_FRAME_RATE = 30
DEFAULT_ROTATION = 0
PAGE = """\
<html>
    <head>
    <title>Raspberry Pi Camera</title>
    </head>
    <body>
        <center><h1>Raspberry Pi Camera</h1></center>
        <center><img src="stream.mjpg" width="%s" height="%s"></center>
    </body>
</html>
""" % (WIDTH, HEIGHT)


class StreamingOutput(object):
    def __init__(self):
        self.frame = None
        self.buffer = io.BytesIO()
        self.condition = Condition()

    def write(self, buf):
        if buf.startswith(b'\xff\xd8'):
            # New frame, copy the existing buffer's content and notify all
            # clients it's available
            self.buffer.truncate()
            with self.condition:
                self.frame = self.buffer.getvalue()
                self.condition.notify_all()
            self.buffer.seek(0)
        return self.buffer.write(buf)


class StreamingHandler(server.BaseHTTPRequestHandler):
    def do_GET(self):
        logging.info(
            "Client connected from IP address {}".format(self.client_address))
        if self.path == "/":
            self.send_response(301)
            self.send_header("Location", "/index.html")
            self.end_headers()
        elif self.path == "/index.html":
            content = streamServer.page.encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "text/html")
            self.send_header("Content-Length", len(content))
            self.end_headers()
            self.wfile.write(content)
        elif self.path == "/stream.mjpg":
            self.send_response(200)
            self.send_header("Age", 0)
            self.send_header("Cache-Control", "no-cache, private")
            self.send_header("Pragma", "no-cache")
            self.send_header("Content-Type",
                             "multipart/x-mixed-replace; boundary=FRAME")
            self.end_headers()
            try:
                while True:
                    # with streamServer.ouput.condition:
                    streamServer.output.condition.acquire()
                    streamServer.output.condition.wait()
                    streamServer.output.condition.release()
                    frame = streamServer.output.frame
                    self.wfile.write(b'--FRAME\r\n')
                    self.send_header("Content-Type", "image/jpeg")
                    self.send_header("Content-Length", len(frame))
                    self.end_headers()
                    self.wfile.write(frame)
                    self.wfile.write(b'\r\n')
            except Exception as e:
                logging.info(
                    "Removed streaming client {}: {}".format(
                        self.client_address, e))
        else:
            logging.warning("Unrecognized request from client @ {}: {}".format(
                self.client_address, self.path))
            self.send_error(404)
            self.end_headers()


class StreamingServer(socketserver.ThreadingMixIn, server.HTTPServer):
    allow_reuse_address = True
    daemon_threads = True


class PiCameraStreamServer(object):
    def __init__(self, resolution="%sx%s" % (WIDTH, HEIGHT)):
        self.output = StreamingOutput()
        self.server = None

        # Default resolution
        self.resolution = "%sx%s" % (WIDTH, HEIGHT)
        # See if user provided a valid resolution string
        try:
            hori = int(resolution[:resolution.index('x')])
            vert = int(resolution[resolution.index('x') + 1:])

            # Generate new page file for /index.html requests based on
            # user-entered resolution
            self.page = PAGE.replace(
                str(WIDTH), str(hori)
            ).replace(str(HEIGHT), str(vert))
            self.resolution = "%sx%s" % (WIDTH, HEIGHT)
        except ValueError:
            logging.warning(
                "Resolution argument must have string format: '[Vertical "
                "Resolution]x[Horizontal Resolution]'. Defaulting to 640x480.")
            hori = 640
            vert = 480
        except Exception:
            logging.warning(
                "Failed to set user-entered resolution. Defaulting to 640x480.")
            hori = 640
            vert = 480


if __name__ == "__main__":
    userResolution = None
    userFramerate = None
    userRotation = None
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-p",
        "--port",
        default=DEFAULT_PORT,
        type=int,
        help="Port number to listen for incoming connections on (default: 8000)"
    )
    parser.add_argument(
        "-res",
        "--resolution",
        default="640x480",
        type=str,
        help="Pi Camera stream resolution"
    )
    parser.add_argument(
        "-f",
        "--framerate",
        default=DEFAULT_FRAME_RATE,
        type=int,
        help="Pi Camera stream framerate"
    )
    parser.add_argument(
        '-rot',
        '--rotation',
        default=DEFAULT_ROTATION,
        type=float,
        help='Angle to rotate Pi Camera stream')
    args = parser.parse_args()

    try:
        if args.port:
            userPort = int(args.port)
        else:
            userPort = DEFAULT_PORT

        if args.resolution:
            userResolution = args.resolution
        else:
            userResolution = "%sx%s" % (WIDTH, HEIGHT)

        if args.framerate:
            userFramerate = int(args.framerate)
        else:
            userFramerate = DEFAULT_FRAME_RATE

        if args.rotation:
            userRotation = int(args.rotation)
        else:
            userRotation = DEFAULT_ROTATION
    except ValueError as e:
        logging.exception(
            "Value error occurred while parsing command line args: {}".format(
                e))
        print("Value error occurred while parsing command line args: {}".format(
            e))
        sys.exit(2)

    try:
        streamServer = PiCameraStreamServer()
        with picamera.PiCamera(resolution=userResolution,
                               framerate=userFramerate) as camera:
            camera.rotation = userRotation
            camera.start_recording(streamServer.output, format="mjpeg")
            print(
                "Camera has started recording: port={}, resolution={}, "
                "framerate={}, rotation={}".format(
                    userPort, userResolution, userFramerate, userRotation))
            try:
                print("Serving video stream...")
                streamServer.server = StreamingServer(("0.0.0.0", userPort),
                                                      StreamingHandler)
                streamServer.server.serve_forever()
                print("Closing video stream.")
            finally:
                camera.stop_recording()
    except KeyboardInterrupt:
        sys.exit(0)
