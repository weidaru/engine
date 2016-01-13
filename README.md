Engine
======
Toy toy toy!!!  

TODO:  
1. Build particle system.  
Steps:  
* Spawn and recycle particles. Texture. CPU? Compute Shader?
* Update speed and positions. Physics. Texture. In one Pass? CPU? Compute Shader?
* Sort by depth. Maybe?
* Render. Instancing (draw from back to front if sorted). Geometry shader.  


First try (Top to down. Static particles. Simple move.):  
* Initialize some particles at top of screen.
* Update position with constant speed.
* Render with instancing and geometry shader.  

2. Clean up enum.
