#!/usr/bin/python
# 3Dviewer.py
#
# particle trajectory viewer using glumpy

import numpy as np
from glumpy import app, gl, gloo, glm
from glumpy.transforms import Trackball, Position
from glumpy.graphics.collections import PointCollection
import ptlib._pt_utils as utils
import os

vertex = """
attribute vec4 a_color;
attribute vec3 a_position;
attribute float a_radius;
varying vec4 v_color;
void main (void)
{
    v_color = a_color;
    gl_Position = <transform>;
    gl_PointSize = a_radius;
}
"""

fragment = """
varying vec4 v_color;

void main()
{
    gl_FragColor = v_color;
}
"""

window = app.Window(width=2048, height=1024, color=(1.0, 1.0, 1.0, 1))

filename = 'pt.temp'
ndata = utils.readPTFile(filename)
tiduniq = np.unique(ndata['tid'])
idx = utils._findInxOfTrack(ndata, tiduniq[0])
x = ndata['x'][idx]
y = ndata['y'][idx]
z = ndata['z'][idx]
t = ndata['t'][idx]
dt = np.diff(t)
dt = np.append([0], dt)

# print(dt.min(), dt.max(), dt.mean())
idx = np.where(dt > 0.5)
blue = np.linspace(0.7, 0.0, len(t))
green = np.linspace(0.7, 0.0, len(t))
red = np.ones_like(t) * 1.0
alpha = np.ones_like(t) * 0.5
red[idx] = 0.0
green[idx] = 0.0
blue[idx] = 1.0
alpha[idx] = 1.0
radius = np.ones_like(t)
radius[0] = 10

points = gloo.Program(vertex, fragment, count=len(x))
points['a_position'] = np.dstack((x, y, z))[0]
points['transform'] = Trackball(Position('a_position'))
points['a_color'] = np.dstack((red, green, blue, alpha))[0]
points['a_radius'] = radius
window.attach(points['transform'])
pidx = 0

pointsCol = PointCollection('agg', color='local', size='local')
window.attach(pointsCol['transform'])
# window.attach(pointsCol['viewport'])

if os.path.isfile('active_site.pt'):
    print('... plot with active_site.pt')
    data = np.genfromtxt('active_site.pt', dtype=[('x', 'f8'), ('y', 'f8'), ('z', 'f8')])
    apoint = gloo.Program(vertex, fragment, count=len(data))
    apoint['a_position'] = np.dstack((data['x'], data['y'], data['z']))[0]
    apoint['a_color'] = (0, 0.5, 0, 0.8)
    apoint['a_radius'] = 5
    apoint['transform'] = Trackball(Position('a_position'))
    window.attach(apoint['transform'])


@window.event
def on_draw(dt):
    global pidx, x, y, z
    window.clear()
    points.draw(gl.GL_LINE_STRIP)
    # pointsCol.draw()
    # if pidx < len(x):
    #     pointsCol.append((x[pidx], y[pidx], z[pidx]), color=(0.0, 0.0, 0.5, 0.8), size=5)
    # pidx += 1
    # print(dt, pidx, x[pidx])
    if os.path.isfile('active_site.pt'):
        apoint.draw(gl.GL_POINTS)

gl.glEnable(gl.GL_DEPTH_TEST)
gl.glEnable(gl.GL_LINE_SMOOTH)

app.run()

