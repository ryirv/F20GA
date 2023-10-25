## Fries by Teo Taylor

In this coursework, we discussed what theme of 3D models we could go for. We collaboratively decided to go with a fast food theme. We decided as a group on what models we should each do, and I took up the task of modelling the fries.


## Creation process
I initially started out with a sketch and initially planned to have each of the fries as a cuboid, with a few bends in the edges here and there for imperfections. However, after looking at some of the early examples of 3D models other members in the group were doing, I decided to make my fries more high fidelity.

### Modelling
I initially started with the box/fries holder. I started with a plane and used the knife tool to cut off faces so that the box wasn't completely square. I then used the Poly Build tool to pull out new faces outwards, and then connected each of those faces by selecting two edges and pressing 'f' on the keyboard to create a new face between them.

After, I extruded the edges up to make the height of the box. The edges were uneven on the z (up/down) axis, so I followed this tutorial to show me how to make them level: https://youtu.be/k28DKsddg6Y

The next part involved cutting the rounded rectangle hole in the box. Admittedly, I had trouble figuring out doing it; I am used to Autodesk Fusion where you could create 2D sketches and then apply 3D transformations to them (like extruding). To do this, first I created 2 circles and a rectangle plane to create the rounded rectangle shape, and then I tried using the knife projection tool to cut into the box. It took a few attempts, but I managed to get there in the end, and after applying the projection, I simply deleted the faces to form the hole.

After that, I created a tall cuboid which I was going to model into the fries. One of the members in the group showed me how to subdivide the cuboid so that I could use the sculpting tools. From there, I duplicated each cuboid and used the elastic deform tool to shape each one of the fries. From there, I repeated the process of duplicating and shaping each cuboid until the box was full of fries.

### Texturing
After that, I proceeded to texturing. This was a learning experience from me since I didn't understand how Blender worked with texturing, but suffice it to say, I learned that each 3D object can have a material, each material can have a shader attached to it, and you can hook up texture data to the shader in the Shading tab.

For texturing the fries, I found an image online of a nacho chip texture, which looked close enough to the texture of fries. Initially, when texturing the fries, I used another tutorial online (https://youtu.be/lWKPeifANIQ) which involved texture painting. However, as I later learned a different approach to texturing, I adopted this approach instead. This is why the fries texture is internal within the file but the logo is external.

I modified the UV map of the fries to ensure the texture wasn't stretched on the fries and to ensure that as much of the detail was shown on the fries as possible. I then simply applied a solid red colour to the fries box.

To apply the logo, I created a quad parallel to the front of the box, and positioned it in the middle. After importing the logo created by Dean, I then went to the shader tab and created an "Image texture" node, and connected up the texture's output colour to the shader's input colour. The logo was showing nicely at the front.

After this, I made a few finishing touches (like fixing some mispositioned fries) and then I pushed the final version of my Blender model to Github.

