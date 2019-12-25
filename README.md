This Team Again Project
====================================

Build the demo
--------------

You need the V4L2 development package and makefile. On Debian/Ubuntu:
```
sudo apt install libv4l-dev
```
```
sudo apt install make
```


Then:
```
make
```

Calling `./grab` acquires one image from `/dev/video0` and saves it as
`frame.ppm`.


