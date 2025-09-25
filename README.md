# Interpretor-for-Simple-Graphic-Language
A simple interpretor for an even simpler graphic language, that can interpret instructions to create an image

HOW TO USE : 
you must configure a .ipi file for the instructions :
It must contain the size of the image as one integer on the first line of the file (the image must have the same height and width as of now)
The rest of the file will contain the instructions as a single string of characters. The effect of every character is listed below.
Once your .ipi file is configured, you can also pick between the sample ones, you just execute the program using a terminal.

This graphic language uses buckets to paint opacity and color, in which you can add any number of opacity values and colors which are averaged when you want to draw.
You have two options to draw, draw a line between a marked position and the cursor position or fill an area, the fill tool works as many fill tools, by filling an area of adjacents pixels of the same color.

Character effects :
n - Adds the color black to the color bucket
r - Adds the color red to the color bucket
g - Adds the color green to the color bucket
b - Adds the color blue to the color bucket
y - Adds the color yellow to the color bucket
m - Adds the color magenta to the color bucket
c - Adds the color cyan to the color bucket
w - Adds the color white to the color bucket
t - Adds the transparent opacity to the opacity bucket
o - Adds the opaque opacity to the opacity bucket
i - Empties the color and opacity buckets
v - Move the current cursor position for one step according in the current direction
h - Rotates the cursor clockwise
a - Rotates the cursor counter-clockwise
p - Updates the marked position to the current cursor position
l - Draws a line bewtween the marked position and the current cursor position
f - Fills the area around the current cursor position
s - Attempts to add a new calc to the calc stack, note that the calc stack is limited to 10 calcs, if the stack is full does nothing
e - Attempts to fuse the first two calcs
j - Attemps to cut the second calc using the opacity mask of the first calc in the stack, removing the first calc
Any other character will be ignored, including the capps version of the characters listed above.
