#!/usr/bin/env python3
"""
3Dviewer.py

particle trajectory viewer using glumpy

Date: 20170814 - update for key input
Date: 20170904 - update for input argument, cleanup using collection-point
"""

import sys
import numpy as np
from glumpy import app, gl
from glumpy.graphics.collections import PointCollection, SegmentCollection
from glumpy.ext import png
from glumpy.transforms import Trackball, Position
from glumpy.app.movie import record
import ptlib._pt_utils as utils

__author__ = 'Sung-Cheol Kim'
__version__ = '1.1.0'


def reset(t):
    t.theta = 0.0
    t.phi = 0.0
    t.zoom = 30.0


def addData(fn, points, color=(0, 0, 0, -1)):
    try:
        ndata = utils.readPTFile(fn)
    except:
        print("... error reading file: {}".format(fn))
        exit(1)

    tuniq = np.unique(ndata['t'])
    tiduniq = np.unique(ndata['tid'])
    trackNumber = len(tiduniq)
    maxtime = len(tuniq)
    print("... iteration number={}, track number={}".format(maxtime, trackNumber))

    if color[-1] < 0:
        cs = np.random.uniform(0.1, 0.5, (len(tiduniq), 4))
        cs[:, 3] = -color[-1]
    else:
        cs = color

    t = ndata['t']
    idx = np.where(t == tuniq[0])
    x = ndata['x'][idx]
    y = ndata['y'][idx]
    z = ndata['z'][idx]
    r = ndata['r'][idx]
    rmax = r.max()
    rmin = r.min()
    if rmax == rmin:
        s = r + 5.0
    else:
        s = (r - rmin)/(rmax - rmin)*10.0+2.0
    # print(rmax, rmin, s)
    points.append(np.dstack((x, y, z))[0], color=cs, size=s)

    return ndata


def setData(ndata, tuniq, time):
    idx = np.where(ndata['t'] == tuniq[time])
    x = ndata['x'][idx]
    y = ndata['y'][idx]
    z = ndata['z'][idx]
    return np.dstack((x, y, z))[0]


def setRadius(ndata, tuniq, time):
    idx = np.where(ndata['t'] == tuniq[time])
    r = ndata['r'][idx]
    rmax = r.max()
    rmin = r.min()
    if rmax == rmin:
        s = r + 5.0
    else:
        s = (r - rmin)/(rmax - rmin)*10.0+2.0
    return s


if __name__ == "__main__":
    # window = app.Window(width=2048, height=1024, color=(1, 1, 1, 1))
    window = app.Window(width=800, height=800, color=(1, 1, 1, 1))
    # window = app.Window(color=(1, 1, 1, 1))
    points1 = PointCollection("agg", color="local", size="local")
    points2 = PointCollection("agg", color="local", size="local")
    coord = SegmentCollection("agg+", color="local", linewidth="local")
    framebuffer = np.zeros((window.height, window.width*3), dtype=np.uint8)

    toggleStop = False
    time = 0
    save_flag = False

    # prepare pt data
    if len(sys.argv) < 3:
        print("... Usage: 3DViewer.py file1 file2 [-save]")
        exit(1)
    if len(sys.argv) == 4:
        if sys.argv[-1].find("save"):
            print("... save as movie")
            save_flag = True
        else:
            save_flag = False

    filename = sys.argv[1]

    ndata1 = addData(sys.argv[1], points1, color=(0.2, 0.2, 0.2, -1.0))
    tuniq1 = np.unique(ndata1['t'])
    ndata2 = addData(sys.argv[2], points2, color=(0.2, 0.8, 0.2, 0.7))
    tuniq2 = np.unique(ndata2['t'])
    maxtime = len(tuniq1)

    # prepare coordinate
    P0 = np.array([[0, 0, 0], [0, 0, 0], [0, 0, 0]])
    P1 = np.array([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
    CP = [(0.9, 0.1, 0.1, 1), (0.1, 0.9, 0.1, 1), (0.1, 0.1, 0.9, 1)]
    coord.append(P0, P1, color=CP, linewidth=[1, 1, 1])
    coord['antialias'] = 1

    transform = Trackball(Position())
    points1["transform"] = transform
    points2["transform"] = transform
    coord['transform'] = transform

    window.attach(points1["transform"])
    window.attach(points1["viewport"])
    window.attach(points2["transform"])
    window.attach(points2["viewport"])
    window.attach(coord["transform"])
    window.attach(coord["viewport"])

    @window.event
    def on_draw(dt):
        global time, maxtime, toggleStop
        window.clear()

        if not toggleStop:
            time += 1

        if time >= maxtime:
            time = 0

        points1["position"] = setData(ndata1, tuniq1, time)
        points1["size"] = setRadius(ndata1, tuniq1, time)
        points2["position"] = setData(ndata2, tuniq2, time)
        # print("t={}".format(time))

        points1.draw()
        points2.draw()
        coord.draw()

    @window.event
    def on_key_press(symbol, modifiers):
        global toggleStop, time
        # print('Key pressed (symbol=%s, modifiers=%s)' % (symbol, modifiers))
        if symbol == 83:
            gl.glReadPixels(0, 0, window.width, window.height, gl.GL_RGB, gl.GL_UNSIGNED_BYTE, framebuffer)
            screen_filename = filename[:-3]+'_'+str(time)+'.png'
            png.from_array(framebuffer, 'RGB').save(screen_filename)
            print('... screen capture: {}'.format(screen_filename))
        if symbol == app.window.key.ENTER:
            reset(transform)
        if symbol == app.window.key.SPACE:
            if toggleStop:
                toggleStop = False
            else:
                toggleStop = True

    reset(transform)
    framerate = 30
    duration = maxtime/framerate

    if save_flag:
        with record(window, filename[:-3]+'.mp4', fps=framerate):
            app.run(framerate=framerate, duration=duration)
    else:
        app.run()
