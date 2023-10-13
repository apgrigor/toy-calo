settings.outformat = "png";
settings.prc = false;
settings.render = 4;
size(200,200);

import three;
import grid3;

currentprojection = orthographic(12, -15, 9);

limits((0,0,0), (9,9,9));

pen backpen = grey;

grid3(ZYZgrid(pos = Relative(0)), pgrid = invisible, pGrid = backpen);
grid3(ZXZgrid(pos = Relative(9)), pgrid = invisible, pGrid = backpen);
grid3(XYXgrid(pos = Relative(0)), pgrid = invisible, pGrid = backpen);

draw(xscale3(9) * yscale3(9) * zscale3(4) * unitcube, purple + opacity(0.4));
draw(shift((0,0,4)) * xscale3(9) * yscale3(9) * zscale3(5) * unitcube, orange + opacity(0.4));

grid3(ZYZgrid(pos = Relative(9)), pgrid = invisible, pGrid = black);
grid3(XYXgrid(pos = Relative(9)), pgrid = invisible, pGrid = black);
grid3(ZXZgrid(pos = Relative(0)), pgrid = invisible, pGrid = black);

real offset =  0.5;
triple offvx = (0, -offset, -offset);
triple offvy = (offset, 0, -offset);
triple offvz = (-offset, -offset, 0);

draw(((0,0,0) + offvx) -- ((9,0,0) + offvx), L = Label("$\eta$", align = S), Arrow3);
draw(((9,0,0) + offvy) -- ((9,9,0) + offvy), L = Label("$\phi$", align = S), Arrow3);
draw(((0,0,0) + offvz) -- ((0,0,9) + offvz), L = Label("$t$", align = W), Arrow3);


