;;;; This file is used to read and set custom colormaps in Ansys Fluent
; Authors: 
; Emmaleigh Hawkins
; Vincent Marias
; Will Meinert
; Daniel Nau

; Using This File
;
; Make sure that the folder titled "colormaps" containing all of the .colormap files is in your working directory 
;
; Load this file in Ansys Fluent
; Option 1: Go to File>Read>Scheme...>colormap_selection.scm 
; Option 2: Type the relative path into the console: (load "colormap_selection.scm") 
; Option 3: Type the absolute path into the console: (load "C:/<path>/colormap_selection.scm")
; Note: If this is done multiple times, the ribbon cannot be removed unless Ansys Fluent is restarted
;
; In the top ribbon menu, select the "Colormap Selection" tab and the dropdown to open the menu
; Select your colormap and then click "OK" for it to apply
; A command will automatically be applied and it can be seen in the console
; 
; To have a colormap selection apply to an existing model, a new contour must be created
;
; To load this file automatically, the following command must be added to your .fluent file:
; (ti-menu-load-string "file read-journal colormap_selection.scm")

; Using Ansys Fluent Predefined Colormaps
; To see built in colormap options using the console: preferences/graphics/colormap-settings/colormap 
; To change colormap: preferences/graphics/colormap-settings/colormap/<colormap> 
; e.g. preferences/graphics/colormap-settings/colormap/field-velocity
; Use "q" or "quit" in the prompt to move back to the previously occupied menu, if needed

; Adding Ansys Fluent Predefined Colormaps to Selection Box
;
; Step 1: Add this file to Fluent
; Option 1: Go to File>Read>Scheme...>colormap_selection.scm 
; Option 2: Type the absolute path into the console: (load "C:/<path>/colormap_selection.scm" )
;
; Step 2: In Ansys Fluent, change the currently selected colormap
; To see colormap options, type into the console: preferences/graphics/colormap-settings/colormap
; To select a colormap, type into the console: preferences/graphics/colormap-settings/colormap/<colormap> 
; e.g. preferences/graphics/colormap-settings/colormap/field_velocity
; Use "q" or "quit" in the prompt to move back to the previously occupied menu, if needed
;
; Step 3: Save the colormap file to the colormaps folder
; Into the console, type: file/write-colormap colormaps/<file name>.colormap
; e.g. file/write-colormap colormaps/gray.colormap
;
; Step 4: Add button for colormap into this .scm file
; In the code below, there are 6 locations that must be edited. They can be searched for by finding "NEW COLORMAP"
; Uncomment these lines (comments start with semicolons) 
; Replace what is inside the angle brackets (<>) with the appropriate names
;
; Step 5: Add this .scm file to Fluent again
; Option 1: Go to File>Read>Scheme...>colormap_selection.scm 
; Option 2: Type the relative path into the console: (load "colormap_selection.scm") 
; Option 3: Type the absolute path into the console: (load "C:/<path>/colormap_selection.scm")
; Note: The previous ribbon cannot be removed from Fluent unless Fluent is restarted

; Adding Downloaded Colormaps to Selection Box
; 
; Step 1: Add the .colormap file to the colormaps folder
;
; Step 2: Add button for colormap into this .scm file
; In the code below, there are 6 locations that must be edited. They can be searched for by finding "NEW COLORMAP"
; Uncomment these lines (comments start with semicolons) 
; Replace what is inside the angle brackets (<>) with the appropriate names
;
; Step 3: Add this .scm file to Fluent again
; Option 1: Go to File>Read>Scheme...>colormap_selection.scm 
; Option 2: Type the relative path into the console: (load "colormap_selection.scm") 
; Option 3: Type the absolute path into the console: (load "C:/<path>/colormap_selection.scm")
; Note: The previous ribbon cannot be removed from Fluent unless Fluent is restarted

; Acknowledgements 
; The Scheme commands for reading and writing .colormap files along with custom colormaps
; were created/provided by James Wright and can be found here:
; https://github.com/jrwrigh/fluent_colormaps/tree/master 

; Ideas: 
; Maybe add to larger interface
; Add instructions to readme
; Research display/objects/display, display/re-render, and other commands to see if colormap can automatically be applied

; Taken from https://github.com/jrwrigh/fluent_colormaps/tree/master
; ------------------------- START -------------------------
(define (write-cmap fn)
  (let ((port (open-output-file (cx-expand-filename fn))) (cmap (cxgetvar 'def-cmap)))
    (write (cons cmap (cx-get-cmap cmap)) port)
    (newline port)
    (close-output-port port)))

(define (read-cmap fn)
  (if (file-exists? fn)
    (let ((cmap (read (open-input-file (cx-expand-filename fn)))))
        (cx-add-cmap (car cmap) (cons (length (cdr cmap)) (cdr cmap)))
        (cxsetvar 'def-cmap (car cmap)))
    (cx-error-dialog
    (format #f "Macro file ~s not found." fn))))

(define (ti-write-cmap)
  (let ((fn (read-filename "colormap filename" "cmap.scm")))
    (if (ok-to-overwrite? fn)
        (write-cmap fn))))

(define (ti-read-cmap)
  (read-cmap (read-filename "colormap filename" "cmap.scm")))

(ti-menu-insert-item!
  file-menu
  (make-menu-item "read-colormap" #t ti-read-cmap
  "Read a colormap from a file."))

(ti-menu-insert-item!
  file-menu
  (make-menu-item "write-colormap" #t ti-write-cmap "Write a colormap to a file."))
; ------------------------- END ------------------------- 

; Define variables for menu
(make-new-rpvar 'longwallgobs/field_velocity_button #t 'boolean)
(make-new-rpvar 'longwallgobs/explosive_plots_button #f 'boolean)
(make-new-rpvar 'longwallgobs/blackbody_button #f 'boolean)
(make-new-rpvar 'longwallgobs/blackbody_extended_button #f 'boolean)
(make-new-rpvar 'longwallgobs/cividis_button #f 'boolean)
(make-new-rpvar 'longwallgobs/coolwarm_bent_button #f 'boolean)
(make-new-rpvar 'longwallgobs/coolwarm_smooth_button #f 'boolean)
(make-new-rpvar 'longwallgobs/inferno_button #f 'boolean)
(make-new-rpvar 'longwallgobs/kindlmann_button #f 'boolean)
(make-new-rpvar 'longwallgobs/kindlmann_extended_button #f 'boolean)
(make-new-rpvar 'longwallgobs/magma_button #f 'boolean)
(make-new-rpvar 'longwallgobs/plasma_button #f 'boolean)
(make-new-rpvar 'longwallgobs/viridis_button #f 'boolean) ; Viridis is the Matplotlib default colormap
; (make-new-rpvar 'longwallgobs/<button name> #f 'boolean) ; NEW COLORMAP

; Create a menu for various colormap options 
; Colormap Selection Definition
(define colormap-selection-box
	; Let Statement, Local Variable Declarations
	(let ((dialog-box #f)
		(table)
			(longwallgobs/colormap_button_box)
      (longwallgobs/field_velocity_button)	
      (longwallgobs/explosive_plots_button)
      (longwallgobs/blackbody_button)
      (longwallgobs/blackbody_extended_button)
      (longwallgobs/cividis_button)
      (longwallgobs/coolwarm_bent_button)
      (longwallgobs/coolwarm_smooth_button)
      (longwallgobs/inferno_button)
      (longwallgobs/kindlmann_button)
      (longwallgobs/kindlmann_extended_button)
      (longwallgobs/magma_button)
      (longwallgobs/plasma_button)
			(longwallgobs/viridis_button)
      ; (longwallgobs/<button name>) ; NEW COLORMAP
	  )
		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			(cx-set-toggle-button longwallgobs/field_velocity_button (rpgetvar 'longwallgobs/field_velocity_button))
      (cx-set-toggle-button longwallgobs/explosive_plots_button (rpgetvar 'longwallgobs/explosive_plots_button))
			(cx-set-toggle-button longwallgobs/blackbody_button (rpgetvar 'longwallgobs/blackbody_button))
      (cx-set-toggle-button longwallgobs/blackbody_extended_button (rpgetvar 'longwallgobs/blackbody_extended_button))
      (cx-set-toggle-button longwallgobs/cividis_button (rpgetvar 'longwallgobs/cividis_button))
      (cx-set-toggle-button longwallgobs/coolwarm_bent_button (rpgetvar 'longwallgobs/coolwarm_bent_button))
      (cx-set-toggle-button longwallgobs/coolwarm_smooth_button (rpgetvar 'longwallgobs/coolwarm_smooth_button))
      (cx-set-toggle-button longwallgobs/inferno_button (rpgetvar 'longwallgobs/inferno_button))
      (cx-set-toggle-button longwallgobs/kindlmann_button (rpgetvar 'longwallgobs/kindlmann_button))
      (cx-set-toggle-button longwallgobs/kindlmann_extended_button (rpgetvar 'longwallgobs/kindlmann_extended_button))
      (cx-set-toggle-button longwallgobs/magma_button (rpgetvar 'longwallgobs/magma_button))
      (cx-set-toggle-button longwallgobs/plasma_button (rpgetvar 'longwallgobs/plasma_button))
      (cx-set-toggle-button longwallgobs/viridis_button (rpgetvar 'longwallgobs/viridis_button))
      ; (cx-set-toggle-button longwallgobs/<button name> (rpgetvar 'longwallgobs/<button name>)) ; NEW COLORMAP
		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)
      ; Set variables based on button selection
      (rpsetvar 'longwallgobs/field_velocity_button (cx-show-toggle-button longwallgobs/field_velocity_button))
      (rpsetvar 'longwallgobs/explosive_plots_button (cx-show-toggle-button longwallgobs/explosive_plots_button))
      (rpsetvar 'longwallgobs/blackbody_button (cx-show-toggle-button longwallgobs/blackbody_button))
      (rpsetvar 'longwallgobs/blackbody_extended_button (cx-show-toggle-button longwallgobs/blackbody_extended_button))
      (rpsetvar 'longwallgobs/cividis_button (cx-show-toggle-button longwallgobs/cividis_button))
      (rpsetvar 'longwallgobs/coolwarm_bent_button (cx-show-toggle-button longwallgobs/coolwarm_bent_button))
      (rpsetvar 'longwallgobs/coolwarm_smooth_button (cx-show-toggle-button longwallgobs/coolwarm_smooth_button))
      (rpsetvar 'longwallgobs/inferno_button (cx-show-toggle-button longwallgobs/inferno_button))
      (rpsetvar 'longwallgobs/kindlmann_button (cx-show-toggle-button longwallgobs/kindlmann_button))
      (rpsetvar 'longwallgobs/kindlmann_extended_button (cx-show-toggle-button longwallgobs/kindlmann_extended_button))
      (rpsetvar 'longwallgobs/magma_button (cx-show-toggle-button longwallgobs/magma_button))
      (rpsetvar 'longwallgobs/plasma_button (cx-show-toggle-button longwallgobs/plasma_button))
      (rpsetvar 'longwallgobs/viridis_button (cx-show-toggle-button longwallgobs/viridis_button))
      ; (rpsetvar 'longwallgobs/<button name> (cx-show-toggle-button longwallgobs/<button name>)) ; NEW COLORMAP 
      ; Set colormap based on radio button selection
      ; (if (equal? (rpgetvar 'longwallgobs/plasma_button) #t) (display (rpgetvar 'longwallgobs/plasma_button)) (display (rpgetvar 'longwallgobs/plasma_button)) )
      ; (if (equal? (rpgetvar 'longwallgobs/plasma_button) #t) (ti-menu-load-string "file/read-colormap colormaps/plasma.colormap") () )
      ; (if (equal? (rpgetvar 'longwallgobs/field_velocity_button) #t) (ti-menu-load-string "preferences/graphics/colormap-settings/colormap/field_velocity\nGo to Contours>Colormap Options...>Currently Defined>field_velocity to apply")) 
      (if (equal? (rpgetvar 'longwallgobs/field_velocity_button) #t) (ti-menu-load-string "file/read-colormap colormaps/field_velocity.colormap\n")) 
      (if (equal? (rpgetvar 'longwallgobs/explosive_plots_button) #t) (ti-menu-load-string "file/read-colormap colormaps/explosive_plots.colormap\n"))
      (if (equal? (rpgetvar 'longwallgobs/blackbody_button) #t) (ti-menu-load-string "file/read-colormap colormaps/blackbody.colormap\n")) 
      (if (equal? (rpgetvar 'longwallgobs/blackbody_extended_button) #t) (ti-menu-load-string "file/read-colormap colormaps/blackbody_extended.colormap\n")) 
      (if (equal? (rpgetvar 'longwallgobs/cividis_button) #t) (ti-menu-load-string "file/read-colormap colormaps/cividis.colormap\n")) 
      (if (equal? (rpgetvar 'longwallgobs/coolwarm_bent_button) #t) (ti-menu-load-string "file/read-colormap colormaps/coolwarm_bent.colormap\n")) 
      (if (equal? (rpgetvar 'longwallgobs/coolwarm_smooth_button) #t) (ti-menu-load-string "file/read-colormap colormaps/coolwarm_smooth.colormap\n")) 
      (if (equal? (rpgetvar 'longwallgobs/inferno_button) #t) (ti-menu-load-string "file/read-colormap colormaps/inferno.colormap\n")) 
      (if (equal? (rpgetvar 'longwallgobs/kindlmann_button) #t) (ti-menu-load-string "file/read-colormap colormaps/kindlmann.colormap\n")) 
      (if (equal? (rpgetvar 'longwallgobs/kindlmann_extended_button) #t) (ti-menu-load-string "file/read-colormap colormaps/kindlmann_extended.colormap\n")) 
      (if (equal? (rpgetvar 'longwallgobs/magma_button) #t) (ti-menu-load-string "file/read-colormap colormaps/magma.colormap\n")) 
      (if (equal? (rpgetvar 'longwallgobs/plasma_button) #t) (ti-menu-load-string "file/read-colormap colormaps/plasma.colormap\n"))
      (if (equal? (rpgetvar 'longwallgobs/viridis_button) #t) (ti-menu-load-string "file/read-colormap colormaps/viridis.colormap\n")) 
      ; (if (equal? (rpgetvar 'longwallgobs/<button name>) #t) (ti-menu-load-string "file/read-colormap colormaps/<file name>\n")) ; NEW COLORMAP
    )

		(lambda args
			(if (not dialog-box)
				(let ()
          ; Tab for box
					(set! dialog-box (cx-create-panel "Colormap Selection" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))
          ; Box to put radio buttons in
					(set! longwallgobs/colormap_button_box (cx-create-button-box table "Colormaps" 'radio-mode #t 'col 0))
          ; Radio buttons for the table
					(set! longwallgobs/field_velocity_button (cx-create-toggle-button longwallgobs/colormap_button_box "Field Velocity (Default)"))
          (set! longwallgobs/explosive_plots_button (cx-create-toggle-button longwallgobs/colormap_button_box "Explosive Plots"))
					(set! longwallgobs/blackbody_button (cx-create-toggle-button longwallgobs/colormap_button_box "Black Body"))
					(set! longwallgobs/blackbody_extended_button (cx-create-toggle-button longwallgobs/colormap_button_box "Extended Black Body"))
					(set! longwallgobs/cividis_button (cx-create-toggle-button longwallgobs/colormap_button_box "Cividis"))
					(set! longwallgobs/coolwarm_bent_button (cx-create-toggle-button longwallgobs/colormap_button_box "Smooth Cool Warm"))
					(set! longwallgobs/coolwarm_smooth_button (cx-create-toggle-button longwallgobs/colormap_button_box "Bent Cool Warm"))
					(set! longwallgobs/inferno_button (cx-create-toggle-button longwallgobs/colormap_button_box "Inferno"))
					(set! longwallgobs/kindlmann_button (cx-create-toggle-button longwallgobs/colormap_button_box "Kindlmann"))
					(set! longwallgobs/kindlmann_extended_button (cx-create-toggle-button longwallgobs/colormap_button_box "Extended Kindlmann"))
					(set! longwallgobs/magma_button (cx-create-toggle-button longwallgobs/colormap_button_box "Magma"))
					(set! longwallgobs/plasma_button (cx-create-toggle-button longwallgobs/colormap_button_box "Plasma"))
					(set! longwallgobs/viridis_button (cx-create-toggle-button longwallgobs/colormap_button_box "Viridis"))
          ; (set! longwallgobs/<button name> (cx-create-toggle-button longwallgobs/colormap_button_box "<name to be displayed>")) ; NEW COLORMAP
				) ;End Of Let Statement
			) ;End Of If Statement
			;Call To Open Dialog Box
			(cx-show-panel dialog-box)
		) ;End Of Args Function
	) ;End Of Let Statement
) ; End of Zone Selection Definition

(cx-add-menu "Colormap Selection" #f)
(cx-add-item "Colormap Selection" "Options" #\U #f #t colormap-selection-box)