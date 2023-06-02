; Note: Make sure that the folder "colormaps" is in your working directory
; (load "\colormap_selection.scm") 
; https://sjmgarnier.github.io/viridis/

; To see built in colormap options using the console: preferences/graphics/colormap-settings/colormap 
; To change colormap: preferences/graphics/colormap-settings/colormap/<colormap> e.g. command preferences/graphics/colormap-settings/colormap/field-velocity
; Use "q" or "quit" in the prompt to move back to the previously occupied menu, if needed

; TODO:
; Currently requires creating a new contour or go to Contours>Colormap Options...>Currently Defined>
; if the option was previously selected and loaded to be able to apply the change
; Ideas: 
;   -Preload all options so that they're available. Issue: don't know how to change back to default field-velocity after this 
;   -Find out how change the Currently Defined selection 
;   -Add a console message for the instructions
; Maybe add to larger interface
; Add instructions to readme

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
(make-new-rpvar 'gob/field-velocity-button #t 'boolean)
(make-new-rpvar 'gob/blackbody-button #f 'boolean)
(make-new-rpvar 'gob/blackbody-extended-button #f 'boolean)
(make-new-rpvar 'gob/cividis-button #f 'boolean)
(make-new-rpvar 'gob/coolwarm-bent-button #f 'boolean)
(make-new-rpvar 'gob/coolwarm-smooth-button #f 'boolean)
(make-new-rpvar 'gob/inferno-button #f 'boolean)
(make-new-rpvar 'gob/kindlmann-button #f 'boolean)
(make-new-rpvar 'gob/kindlmann-extended-button #f 'boolean)
(make-new-rpvar 'gob/magma-button #f 'boolean)
(make-new-rpvar 'gob/plasma-button #f 'boolean)
(make-new-rpvar 'gob/viridis-button #f 'boolean)

; Create a menu for various colormap options 
; Colormap Selection Definition
(define colormap-selection-box
	; Let Statement, Local Varaible Declarations
	(let ((dialog-box #f)
		(table)
			(gob/colormap_button_box)
			(gob/field-velocity-button)	
            (gob/blackbody-button)
            (gob/blackbody-extended-button)
            (gob/cividis-button)
            (gob/coolwarm-bent-button)
            (gob/coolwarm-smooth-button)
            (gob/inferno-button)
            (gob/kindlmann-button)
            (gob/kindlmann-extended-button)
            (gob/magma-button)
            (gob/plasma-button)
			(gob/viridis-button)
	)
		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			(cx-set-toggle-button gob/field-velocity-button (rpgetvar 'gob/field-velocity-button))
			(cx-set-toggle-button gob/blackbody-button (rpgetvar 'gob/blackbody-button))
            (cx-set-toggle-button gob/blackbody-extended-button (rpgetvar 'gob/blackbody-extended-button))
            (cx-set-toggle-button gob/cividis-button (rpgetvar 'gob/cividis-button))
            (cx-set-toggle-button gob/coolwarm-bent-button (rpgetvar 'gob/coolwarm-bent-button))
            (cx-set-toggle-button gob/coolwarm-smooth-button (rpgetvar 'gob/coolwarm-smooth-button))
            (cx-set-toggle-button gob/inferno-button (rpgetvar 'gob/inferno-button))
            (cx-set-toggle-button gob/kindlmann-button (rpgetvar 'gob/kindlmann-button))
            (cx-set-toggle-button gob/kindlmann-extended-button (rpgetvar 'gob/kindlmann-extended-button))
            (cx-set-toggle-button gob/magma-button (rpgetvar 'gob/magma-button))
            (cx-set-toggle-button gob/plasma-button (rpgetvar 'gob/plasma-button))
            (cx-set-toggle-button gob/viridis-button (rpgetvar 'gob/viridis-button))
		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)
            ; Set variables based on button selection
			(rpsetvar 'gob/field-velocity-button (cx-show-toggle-button gob/field-velocity-button))
            (rpsetvar 'gob/blackbody-button (cx-show-toggle-button gob/blackbody-button))
            (rpsetvar 'gob/blackbody-extended-button (cx-show-toggle-button gob/blackbody-extended-button))
            (rpsetvar 'gob/cividis-button (cx-show-toggle-button gob/cividis-button))
            (rpsetvar 'gob/coolwarm-bent-button (cx-show-toggle-button gob/coolwarm-bent-button))
            (rpsetvar 'gob/coolwarm-smooth-button (cx-show-toggle-button gob/coolwarm-smooth-button))
            (rpsetvar 'gob/inferno-button (cx-show-toggle-button gob/inferno-button))
            (rpsetvar 'gob/kindlmann-button (cx-show-toggle-button gob/kindlmann-button))
            (rpsetvar 'gob/kindlmann-extended-button (cx-show-toggle-button gob/kindlmann-extended-button))
            (rpsetvar 'gob/magma-button (cx-show-toggle-button gob/magma-button))
			(rpsetvar 'gob/plasma-button (cx-show-toggle-button gob/plasma-button))
            (rpsetvar 'gob/viridis-button (cx-show-toggle-button gob/viridis-button))
            ; Set colormap based on radio button selection
            ;(if (equal? (rpgetvar 'gob/plasma-button) #t) (display (rpgetvar 'gob/plasma-button)) (display (rpgetvar 'gob/plasma-button)) )
            ; (if (equal? (rpgetvar 'gob/plasma-button) #t) (ti-menu-load-string "file/read-colormap colormaps/plasma.colormap") () )
            ; (if (equal? (rpgetvar 'gob/field-velocity-button) #t) (ti-menu-load-string "preferences/graphics/colormap-settings/colormap/field-velocity\nGo to Contours>Colormap Options...>Currently Defined>field-velocity to apply")) 
            (if (equal? (rpgetvar 'gob/field-velocity-button) #t) (display "Go to Contours>Colormap Options...>Currently Defined>field-velocity to apply\n")) 
            (if (equal? (rpgetvar 'gob/blackbody-button) #t) (ti-menu-load-string "file/read-colormap colormaps/blackbody.colormap\n")) 
            (if (equal? (rpgetvar 'gob/blackbody-extended-button) #t) (ti-menu-load-string "file/read-colormap colormaps/blackbody_extended.colormap\n")) 
            (if (equal? (rpgetvar 'gob/cividis-button) #t) (ti-menu-load-string "file/read-colormap colormaps/cividis.colormap\n")) 
            (if (equal? (rpgetvar 'gob/coolwarm-bent-button) #t) (ti-menu-load-string "file/read-colormap colormaps/coolwarm_bent.colormap\n")) 
            (if (equal? (rpgetvar 'gob/coolwarm-smooth-button) #t) (ti-menu-load-string "file/read-colormap colormaps/coolwarm_smooth.colormap\n")) 
            (if (equal? (rpgetvar 'gob/inferno-button) #t) (ti-menu-load-string "file/read-colormap colormaps/inferno.colormap\n")) 
            (if (equal? (rpgetvar 'gob/kindlmann-button) #t) (ti-menu-load-string "file/read-colormap colormaps/kindlmann.colormap\n")) 
            (if (equal? (rpgetvar 'gob/kindlmann-extended-button) #t) (ti-menu-load-string "file/read-colormap colormaps/kindlmann_extended.colormap\n")) 
            (if (equal? (rpgetvar 'gob/magma-button) #t) (ti-menu-load-string "file/read-colormap colormaps/magma.colormap\n")) 
            (if (equal? (rpgetvar 'gob/plasma-button) #t) (ti-menu-load-string "file/read-colormap colormaps/plasma.colormap\n"))
            (if (equal? (rpgetvar 'gob/viridis-button) #t) (ti-menu-load-string "file/read-colormap colormaps/viridis.colormap\n")) 
        )

		(lambda args
			(if (not dialog-box)
				(let ()
                    ; Tab for box
					(set! dialog-box (cx-create-panel "Colormap Selection" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))
                    ; Box to put radio buttons in
					(set! gob/colormap_button_box (cx-create-button-box table "Colormap" 'radio-mode #t 'col 0))
                    ; Radio buttons for the table
					(set! gob/field-velocity-button (cx-create-toggle-button gob/colormap_button_box "Field Velocity (Default)"))
					(set! gob/blackbody-button (cx-create-toggle-button gob/colormap_button_box "Black Body"))
					(set! gob/blackbody-extended-button (cx-create-toggle-button gob/colormap_button_box "Extended Black Body"))
					(set! gob/cividis-button (cx-create-toggle-button gob/colormap_button_box "Cividis"))
					(set! gob/coolwarm-bent-button (cx-create-toggle-button gob/colormap_button_box "Smooth Cool Warm"))
					(set! gob/coolwarm-smooth-button (cx-create-toggle-button gob/colormap_button_box "Bent Cool Warm"))
					(set! gob/inferno-button (cx-create-toggle-button gob/colormap_button_box "Inferno"))
					(set! gob/kindlmann-button (cx-create-toggle-button gob/colormap_button_box "Kindlmann"))
					(set! gob/kindlmann-extended-button (cx-create-toggle-button gob/colormap_button_box "Extended Kindlmann"))
					(set! gob/magma-button (cx-create-toggle-button gob/colormap_button_box "Magma"))
					(set! gob/plasma-button (cx-create-toggle-button gob/colormap_button_box "Plasma"))
					(set! gob/viridis-button (cx-create-toggle-button gob/colormap_button_box "Viridis"))
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