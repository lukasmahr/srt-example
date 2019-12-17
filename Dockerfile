FROM archlinux
RUN pacman --noconfirm -Sy gstreamer gst-plugins-bad gst-plugins-good gst-plugins-ugly gst-plugins-base gst-libav gstreamer-vaapi gcc pkgconf libva-intel-driver
ADD srt-example.c .
RUN gcc -Wall srt-example.c -o test $(pkg-config --cflags --libs gstreamer-1.0)
ENTRYPOINT [ "./test" ]