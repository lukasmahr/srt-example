# Example for SRT stats printing using GStreamer
## General
* The working bitrate estimation SRT stream (version 1.4.0) is accessible on port 8887.
* The bitrate estimation that does not (SRT version 1.4.1) work is available on port 8888.

## Running
1. Run `docker-compose up`
2. Run GStreamer pipeline on port 8888: `gst-launch-1.0 srtsrc uri=srt://127.0.0.1:8888 ! tsparse ! tsdemux ! h264parse ! avdec_h264 ! videoconvert ! autovideosink`
3. Run GStreamer pipeline on port 8887: `gst-launch-1.0 srtsrc uri=srt://127.0.0.1:8887 ! tsparse ! tsdemux ! h264parse ! avdec_h264 ! videoconvert ! autovideosink`