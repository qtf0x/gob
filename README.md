# gob
gob

## Colormap Selection 

### Using This File

Make sure that the folder titled "colormaps" containing all of the .colormap files is in your working directory. \
The file colormap_selection.scm can also be put into the working directory. 

### Load this file in Ansys Fluent
Option 1: Go to File>Read>Scheme...>colormap_selection.scm \
Option 2: Type the relative path into the console: (load "colormap_selection.scm") \
Option 3: Type the absolute path into the console: (load "C:/<file path>/colormap_selection.scm") \
Note: If this is done multiple times, the ribbon cannot be removed unless Ansys Fluent is restarted 

In the top ribbon menu, select the "Colormap Selection" tab and the dropdown to open the menu. \
Select your colormap and then click "OK" for it to apply. \
A command will automatically be applied and it can be seen in the console. 

To have a colormap selection apply to an existing model, a new contour must be created. 

To load this file automatically, the following command must be added to your .fluent file: \
(ti-menu-load-string "file read-journal colormap_selection.scm") 

### Using Ansys Fluent Predefined Colormaps
To see built in colormap options using the console: preferences/graphics/colormap-settings/colormap \
To change colormap: preferences/graphics/colormap-settings/colormap/<colormap> \
e.g. preferences/graphics/colormap-settings/colormap/field_velocity \
Use "q" or "quit" in the prompt to move back to the previously occupied menu, if needed. 

### Adding Ansys Fluent Predefined Colormaps to Selection Box
Step 1: Add this file to Fluent \
Option 1: Go to File>Read>Scheme...>colormap_selection.scm \
Option 2: Type the absolute path into the console: (load "C:/<path>/colormap_selection.scm" ) 

Step 2: In Ansys Fluent, change the currently selected colormap \
To see colormap options, type into the console: preferences/graphics/colormap-settings/colormap \
To select a colormap, type into the console: preferences/graphics/colormap-settings/colormap/<colormap> \
e.g. preferences/graphics/colormap-settings/colormap/field_velocity \
Use "q" or "quit" in the prompt to move back to the previously occupied menu, if needed 

Step 3: Save the colormap file to the colormaps folder \
Into the console, type: file/write-colormap colormaps/<file name>.colormap \
e.g. file/write-colormap colormaps/gray.colormap 

Step 4: Add button for colormap into this .scm file \
In the code below, there are 6 locations that must be edited. They can be searched for by finding "NEW COLORMAP" \
Uncomment these lines (comments start with semicolons) \
Replace what is inside the angle brackets (<>) with the appropriate names 

Step 5: Add this .scm file to Fluent again \
Option 1: Go to File>Read>Scheme...>colormap_selection.scm \
Option 2: Type the relative path into the console: (load "colormap_selection.scm") \
Option 3: Type the absolute path into the console: (load "C:/<path>/colormap_selection.scm") \
Note: The previous ribbon cannot be removed from Fluent unless Fluent is restarted

### Adding Downloaded Colormaps to Selection Box

Step 1: Add the .colormap file to the colormaps folder 

Step 2: Add button for colormap into this .scm file \
In the code below, there are 6 locations that must be edited. They can be searched for by finding "NEW COLORMAP" \
Uncomment these lines (comments start with semicolons) \
Replace what is inside the angle brackets (<>) with the appropriate names 

Step 3: Add this .scm file to Fluent again \
Option 1: Go to File>Read>Scheme...>colormap_selection.scm \
Option 2: Type the relative path into the console: (load "colormap_selection.scm") \
Option 3: Type the absolute path into the console: (load "C:/<path>/colormap_selection.scm") 

Note: The previous ribbon cannot be removed from Fluent unless Fluent is restarted 

### Acknowledgements 
The Scheme commands for reading and writing .colormap files along with custom colormaps 
were created/provided by James Wright and can be found here: 
https://github.com/jrwrigh/fluent_colormaps/tree/master 