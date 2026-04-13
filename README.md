# GraveRun! - 2D Animation & Gameplay Demo
## Project Overview
**GraveRun!** is an interactive 2D endless runner where the player must escape a chasing zombie while navigating a procedurally generated graveyard. The project focuses on real-time animation, physics-based movement, and dynamic gameplay systems using C++ and OpenGL.

## Project Objectives

The objective of this project was to develop an **interactive 2D digital animation demo**. Beyond the visual composition, the development focused on implementing custom shaders and real-time logic. 

The project was designed to meet and exceed the following technical criteria:

* **Artistic Direction & Visual Impact**: High emphasis on the scene's aesthetic coherence and overall visual engagement.
* **Physics-Based Animation**: Implementation of animations driven by **kinematics and dynamics** simulations rather than simple pre-defined paths.
* **Core Gameplay Mechanics**: Integration of functional gameplay elements, including defined **win/loss conditions** for the player.
* **Particle Systems**: Development of real-time particle effects to enhance the environmental feedback and visual polish.

While code organization was flexible for the assignment, this repository follows a **modular structure** to demonstrate industry-standard practices in game engine architecture.
*Developed as part of the Fundamentals of Computer Graphics course in Computer Engineering in Alma Mater Studiorum, Bologna, Italy.*

## Gameplay Mechanics

The game features a side-scrolling environment where the objective is to survive as long as possible. Players earn points by covering distance and collecting **glitter items** scattered throughout the path.

### Obstacles
To survive, the player must react to two distinct types of hazards:
* **The Headstone**: A static obstacle that requires a timed jump to clear.
* **The Zombie Hand**: An emerging hazard that sprouts from the ground; the player must navigate around or over it without making contact.

### Difficulty Scaling
To ensure a challenging experience, the game implements a **dynamic difficulty system**. The run is divided into **50-meter milestones**. Every time a milestone is reached, the side-scrolling speed increases, demanding faster reflexes for both obstacle avoidance and item collection.

### Real-Time Statistics
Player performance is tracked in real-time. Metrics such as meters traveled and items collected are updated and displayed via the **Command-Line Interface (CLI)** for immediate feedback.

<p align="center">
  <img src="img/cmd_stats.png" width="450" alt="Game Statistics CLI">
  <br><i>Fig. 2 - Real-time statistics tracking distance, collection, and speed updates.</i>
</p>

## Controls

The movement system allows for precise control over the character's position and speed:

| Key | Action |
| :--- | :--- |
| **W** | Jump (Parabolic trajectory) |
| **A** | Step Backward |
| **D** | Step Forward |
| **Shift** | Sprint |
| **R** | Restart Game |

## Graphical Implementation & Asset Creation

All game elements and complex geometries in **GraveRun!** were procedurally generated from simple geometric primitives. The project utilizes helper functions such as `draw_plan()` and `draw_triangle()` (defined in `init_geometrie.cpp`). I extended the provided codebase with custom helpers, including `draw_circle()` and `draw_quad()`, to support more diverse and detailed shapes.

### Asset & Scenery Breakdown

The visual style focuses on a stylized, block-based aesthetic, utilizing transparency and gradients to add depth:

* **Dynamic Sky**: Instead of a flat color, the sky features a vertical gradient achieved through color interpolation between deep red and dark purple.
* **Character Design**:
    * **Protagonist**: A stylized female figure constructed from a series of assembled rectangles forming the head, torso, and limbs, with details like black hair and blue eyes.
    * **The Zombie**: Built with a similar modular block structure but utilizing a desaturated palette (grays and sickly greens) to emphasize its undead nature.
* **Environmental Assets**:
    * **Trees**: Stylized bare structures composed of quadrilaterals forming the trunk and branches.
    * **Mountains**: Polygonal shapes created by joining triangles to produce non-uniform slopes. Dual-tone coloring (base vs. peak) is used to enhance the sense of depth.
    * **Clouds**: Formed by overlapping two circles, scaled along the Y-axis for a more organic look. An **alpha channel ($< 1$)** is used to provide realistic transparency.
* **Interactive & Gameplay Elements**:
    * **Headstones**: Composed of a rectangular base and a semi-circular top, featuring a cross-shaped detail.
    * **Zombie Hands**: Detailed assemblies of multiple rectangles representing the palm, fingers, and nails, emerging from a circular "hole" primitive.
    * **Collectibles (Glitter)**: Diamond-shaped items created by joining two triangles.
    * **Particles**: Individual white square primitives used for real-time visual effects.
 
<table align="center" width="75%">
  <tr>
    <td align="center" width="33%">
      <img src="img/personaggio.png" width="100%" alt="Protagonist"/><br />
      <sub><b>Main Protagonist</b></sub>
    </td>
    <td align="center" width="33%">
      <img src="img/zombie.png" width="100%" alt="Zombie"/><br />
      <sub><b>Chasing Zombie</b></sub>
    </td>
    <td align="center" width="33%">
      <img src="img/montagna.png" width="100%" alt="Mountain"/><br />
      <sub><b>Background Mountain</b></sub>
    </td>
  </tr>
  <tr>
    <td align="center" width="33%">
      <img src="img/mano_zombie.png" width="100%" alt="Zombie Hand"/><br />
      <sub><b>Emerging Zombie Hand</b></sub>
    </td>
    <td align="center" width="33%">
      <img src="img/lapide.png" width="100%" alt="Headstone"/><br />
      <sub><b>Headstone Obstacle</b></sub>
    </td>
    <td align="center" width="33%">
      <img src="img/alberi.png" width="100%" alt="Trees"/><br />
      <sub><b>Scenery Trees</b></sub>
    </td>
  </tr>
</table>
