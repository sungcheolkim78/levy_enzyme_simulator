# pointViewer.py
#
# using glumpy library, show 3D positions of walker

import numpy as np
from glumpy import app, gl, gloo, glm
from glumpy.transforms import Trackball, Position
import ptlib._pt_utils as utils
import os

vertex = """
#version 120
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float linewidth;
uniform float antialias;
attribute vec4  fg_color;
attribute vec4  bg_color;
attribute float radius;
attribute vec3  position;
varying float v_pointsize;
varying float v_radius;
varying vec4  v_fg_color;
varying vec4  v_bg_color;
void main (void)
{
    v_radius = radius;
    v_fg_color = fg_color;
    v_bg_color = bg_color;
    gl_Position = <transform>;
    gl_PointSize = 2 * (v_radius + linewidth + 1.5*antialias);
}
"""

#    gl_Position = projection * view * model * vec4(position,1.0);

fragment = """
#version 120
uniform float linewidth;
uniform float antialias;
varying float v_radius;
varying vec4  v_fg_color;
varying vec4  v_bg_color;
float marker(vec2 P, float size)
{
   const float SQRT_2 = 1.4142135623730951;
   float x = SQRT_2/2 * (P.x - P.y);
   float y = SQRT_2/2 * (P.x + P.y);
   float r1 = max(abs(x)- size/2, abs(y)- size/10);
   float r2 = max(abs(y)- size/2, abs(x)- size/10);
   float r3 = max(abs(P.x)- size/2, abs(P.y)- size/10);
   float r4 = max(abs(P.y)- size/2, abs(P.x)- size/10);
   return min( min(r1,r2), min(r3,r4));
}
void main()
{
    float r = (v_radius + linewidth + 1.5*antialias);
    float t = linewidth/2.0 - antialias;
//    float signed_distance = length(gl_PointCoord.xy - vec2(0.5,0.5)) * 2 * r - v_radius;
    float signed_distance = marker((gl_PointCoord.xy - vec2(0.5,0.5))*r*2, 2*v_radius);
    float border_distance = abs(signed_distance) - t;
    float alpha = border_distance/antialias;
    alpha = exp(-alpha*alpha);
    // Inside shape
    if( signed_distance < 0 ) {
        // Fully within linestroke
        if( border_distance < 0 ) {
            gl_FragColor = v_fg_color;
        } else {
            gl_FragColor = mix(v_bg_color, v_fg_color, alpha);
        }
    // Outside shape
    } else {
        // Fully within linestroke
        if( border_distance < 0 ) {
            gl_FragColor = v_fg_color;
        } else if( abs(signed_distance) < (linewidth/2.0 + antialias) ) {
            gl_FragColor = vec4(v_fg_color.rgb, v_fg_color.a * alpha);
        } else {
            discard;
        }
    }
}
"""

window = app.Window(width=2048, height=1024, color=(1, 1, 1, 1))

ndata = utils.readPTFile("pt.temp")
tiduniq = np.unique(ndata['tid'])
if 0 in tiduniq:
    idx = utils._findInxOfTrack(ndata, 0)
    x = ndata['x'][idx]
    y = ndata['y'][idx]
    z = ndata['z'][idx]

n = len(x)
program = gloo.Program(vertex, fragment, count=n)
view = np.eye(4, dtype=np.float32)
glm.translate(view, 0, 0, -45)

program['position'] = np.dstack((x, y, z))[0]
program['radius'] = np.ones(n)*5
program['fg_color'] = 0.8, 0.8, 0.8, 1
# colors = np.random.uniform(0.75, 1.00, (n, 4))
colors = np.ones((n, 4))
colors[:, 0] = np.linspace(0.00, 0.80, n)
colors[:, 1] = 0
colors[:, 2] = 0
colors[:, 3] = 1
program['bg_color'] = colors
program['linewidth'] = 1.0
program['antialias'] = 1.0
program['view'] = view
program['transform'] = Trackball(Position("position"))
window.attach(program['transform'])

if os.path.isfile('active_site.pt'):
    print("... plot with active_site.pt")
    data = np.genfromtxt("active_site.pt", dtype=[('x', 'f8'), ('y', 'f8'), ('z', 'f8')])
    ndata = len(data['x'])
    pro2 = gloo.Program(vertex, fragment, count=ndata)
    pro2['position'] = np.dstack((data['x'], data['y'], data['z']))[0]
    pro2['radius'] = np.ones_like(data['x'])*3
    pro2['fg_color'] = 0, 0, 0, 1
    colors = np.zeros((ndata, 4))
    colors[:, 0:1] = 0.5
    colors[:, 2] = 1.0
    colors[:, 3] = 0.8
    pro2['bg_color'] = colors
    pro2['linewidth'] = 1.0
    pro2['view'] = view
    pro2['transform'] = Trackball(Position("position"))
    window.attach(pro2['transform'])


@window.event
def on_draw(dt):
    window.clear()
    program.draw(gl.GL_POINTS)
    if os.path.isfile('active_site.pt'):
        pro2.draw(gl.GL_POINTS)

gl.glEnable(gl.GL_DEPTH_TEST)
gl.glPolygonOffset(1, 1)
gl.glEnable(gl.GL_LINE_SMOOTH)

app.run()
