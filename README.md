# CS 330 - Computational Graphics and Visualization

This repository contains my coursework for CS 330 at SNHU. The course covers 
foundational concepts in computational graphics and visualization using C++ and 
OpenGL.

## Standalone Assignments

These assignments used provided scenes to practice each concept individually.

**Creating and Transforming 3D Objects** - Building and applying affine 
transformations to basic 3D shapes in OpenGL.

**Projecting a 3D Scene into a 2D View** - Setting up view and projection 
matrices to render a 3D scene onto a 2D display.

**Basic Camera Movement** - Implementing keyboard and mouse input to navigate 
a 3D scene with a virtual camera.

**Textures** - Applying texture images to 3D objects using UV mapping.

**Lighting** - Adding light sources to a scene using the Phong shading model.

**2D Collision (WIP)** - Implementing collision detection in a 2D environment.

## Final Project Milestones

These milestones were built progressively toward the final project using my own scene.

**Milestone 1** - Project proposal describing the selected image and planned shapes.

**Milestone 2** - Creating and transforming the first 3D objects in the scene.

**Milestone 3** - Adding camera navigation and input device controls.

**Milestone 4** - Applying textures to objects in the scene.

**Milestone 5** - Adding lighting using the Phong shading model.

**Final Project** - A fully interactive 3D scene replicating a photograph of my 
dining table. Includes a round table, a laptop, a mug on a coaster, a stack of books, 
a crystal vase centerpiece with a branch and berry plant, place mats, an area 
rug, and a wood floor. Features full camera navigation, multiple light sources, 
and per-object textures and materials.

## Reflection

**How do I approach designing software?**

Working on this project taught me a lot about thinking in terms of structure before writing any code. Early on I was just adding things as I went, but I quickly realized that without a clear plan for how objects would relate to each other, things got messy fast. The biggest design skill I picked up was thinking about reusability upfront. Building the SceneObject base class was a decision I made early that paid off for the entire project. Every time I added a new object I already had a foundation to work from. That kind of thinking, designing systems that can be extended rather than rewritten, is something I will carry into every project going forward.

**How do I approach developing programs?**

Almost everything in this project was built through iteration. I would write something, run it, see what was wrong, and adjust. That cycle repeated hundreds of times across the milestones. My approach also evolved a lot. In the early milestones I was writing everything pretty loosely, but by the time I was deep into the final project I was thinking much more carefully about encapsulation and keeping things modular. The rotation math was a good example of this. I started naively adding degrees and quickly hit a wall, which forced me to actually learn matrix composition properly. That kind of problem driven learning shaped how I approached every challenge after it.

**How can computer science help me in reaching my goals?**

My goal is to become a software engineer and this project gave me practical experience with things that matter in that field, like object oriented design, working with external libraries, debugging low level issues, and writing code that someone else could read and understand. Computational graphics specifically gave me a much deeper understanding of how math translates into real visual output, which is useful whether you are working on games, simulations, UI systems, or anything else that involves rendering. The more I build, the better engineer I become, and projects like this one are a big part of that.
