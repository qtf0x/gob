; Note: Make sure that the folder "colormaps" is in your working directory
; (load "\colormap_selection.scm") 
; https://sjmgarnier.github.io/viridis/

; To see built in colormap options using the console: preferences/graphics/colormap-settings/colormap 
; To change colormap: preferences/graphics/colormap-settings/colormap/<colormap> 
; e.g. preferences/graphics/colormap-settings/colormap/field_velocity
; Use "q" or "quit" in the prompt to move back to the previously occupied menu, if needed

; TODO:
; Currently requires creating a new contour or go to Contours>Colormap Options...>Currently Defined>
; if the option was previously selected and loaded to be able to apply the change
; Ideas: 
;   -Preload all options so that they're available. Issue: don't know how to change back to default field_velocity after this 
;   -Find out how change the Currently Defined selection 
;   -Add a console message for the instructions
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
  (make-menu-item "write-colormap" #t ti-write-cmap
  "Write a colormap to a file."))
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

; Create a menu for various colormap options 
; Colormap Selection Definition
(define colormap-selection-box
	; Let Statement, Local Varaible Declarations
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
	  )
		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			(cx-set-toggle_button longwallgobs/field_velocity_button (rpgetvar 'longwallgobs/field_velocity_button))
      (cx-set-toggle_button longwallgobs/explosive_plots_button (rpgetvar 'explosive_plots_button))
			(cx-set-toggle_button longwallgobs/blackbody_button (rpgetvar 'longwallgobs/blackbody_button))
      (cx-set-toggle_button longwallgobs/blackbody_extended_button (rpgetvar 'longwallgobs/blackbody_extended_button))
      (cx-set-toggle_button longwallgobs/cividis_button (rpgetvar 'longwallgobs/cividis_button))
      (cx-set-toggle_button longwallgobs/coolwarm_bent_button (rpgetvar 'longwallgobs/coolwarm_bent_button))
      (cx-set-toggle_button longwallgobs/coolwarm_smooth_button (rpgetvar 'longwallgobs/coolwarm_smooth_button))
      (cx-set-toggle_button longwallgobs/inferno_button (rpgetvar 'longwallgobs/inferno_button))
      (cx-set-toggle_button longwallgobs/kindlmann_button (rpgetvar 'longwallgobs/kindlmann_button))
      (cx-set-toggle_button longwallgobs/kindlmann_extended_button (rpgetvar 'longwallgobs/kindlmann_extended_button))
      (cx-set-toggle_button longwallgobs/magma_button (rpgetvar 'longwallgobs/magma_button))
      (cx-set-toggle_button longwallgobs/plasma_button (rpgetvar 'longwallgobs/plasma_button))
      (cx-set-toggle_button longwallgobs/viridis_button (rpgetvar 'longwallgobs/viridis_button))
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
      ; Set colormap based on radio button selection
      ; (if (equal? (rpgetvar 'longwallgobs/plasma_button) #t) (display (rpgetvar 'longwallgobs/plasma_button)) (display (rpgetvar 'longwallgobs/plasma_button)) )
      ; (if (equal? (rpgetvar 'longwallgobs/plasma_button) #t) (ti-menu-load-string "file/read-colormap colormaps/plasma.colormap") () )
      ; (if (equal? (rpgetvar 'longwallgobs/field_velocity_button) #t) (ti-menu-load-string "preferences/graphics/colormap-settings/colormap/field_velocity\nGo to Contours>Colormap Options...>Currently Defined>field_velocity to apply")) 
      (if (equal? (rpgetvar 'longwallgobs/field_velocity_button) #t) (display "Go to Contours>Colormap Options...>Currently Defined>field_velocity to apply\n")) 
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
    )

		(lambda args
			(if (not dialog-box)
				(let ()
          ; Tab for box
					(set! dialog-box (cx-create-panel "Colormap Selection" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))
          ; Box to put radio buttons in
					(set! longwallgobs/colormap_button_box (cx-create-button-box table "Colormap" 'radio-mode #t 'col 0))
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
				) ;End Of Let Statement
			) ;End Of If Statement
			;Call To Open Dialog Box
			(cx-show-panel dialog-box)
		) ;End Of Args Function
	) ;End Of Let Statement
) ; End of Zone Selection Definition

(cx-add-menu "Colormap Selection" #f)
;(cx-add-hitem "Colormap Selection" "Test" #f #f #t #f)
;(cx-add-item "Test" "Colors" #\U #f #t colormap-selection-box)
(cx-add-item "Colormap Selection" "" #\U #f #t colormap-selection-box)