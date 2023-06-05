; Note: Make sure that the folder "colormaps" is in your working directory
; (load "\colormap_selection.scm") 
; https://sjmgarnier.github.io/viridis/

; To see built in colormap options using the console: preferences/graphics/colormap-settings/colormap 
; To change colormap: preferences/graphics/colormap-settings/colormap/<colormap> 
; e.g. preferences/graphics/colormap-settings/colormap/field-velocity
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
(make-new-rpvar 'longwallgobs/field-velocity-button #t 'boolean)
(make-new-rpvar 'longwallgobs/blackbody-button #f 'boolean)
(make-new-rpvar 'longwallgobs/blackbody-extended-button #f 'boolean)
(make-new-rpvar 'longwallgobs/cividis-button #f 'boolean)
(make-new-rpvar 'longwallgobs/coolwarm-bent-button #f 'boolean)
(make-new-rpvar 'longwallgobs/coolwarm-smooth-button #f 'boolean)
(make-new-rpvar 'longwallgobs/inferno-button #f 'boolean)
(make-new-rpvar 'longwallgobs/kindlmann-button #f 'boolean)
(make-new-rpvar 'longwallgobs/kindlmann-extended-button #f 'boolean)
(make-new-rpvar 'longwallgobs/magma-button #f 'boolean)
(make-new-rpvar 'longwallgobs/plasma-button #f 'boolean)
(make-new-rpvar 'longwallgobs/viridis-button #f 'boolean) ; Viridis is the Matplotlib default colormap

; Create a menu for various colormap options 
; Colormap Selection Definition
(define colormap-selection-box
	; Let Statement, Local Varaible Declarations
	(let ((dialog-box #f)
		(table)
			(longwallgobs/colormap_button_box)
			(longwallgobs/field-velocity-button)	
            (longwallgobs/blackbody-button)
            (longwallgobs/blackbody-extended-button)
            (longwallgobs/cividis-button)
            (longwallgobs/coolwarm-bent-button)
            (longwallgobs/coolwarm-smooth-button)
            (longwallgobs/inferno-button)
            (longwallgobs/kindlmann-button)
            (longwallgobs/kindlmann-extended-button)
            (longwallgobs/magma-button)
            (longwallgobs/plasma-button)
			(longwallgobs/viridis-button)
	)
		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			(cx-set-toggle-button longwallgobs/field-velocity-button (rpgetvar 'longwallgobs/field-velocity-button))
			(cx-set-toggle-button longwallgobs/blackbody-button (rpgetvar 'longwallgobs/blackbody-button))
            (cx-set-toggle-button longwallgobs/blackbody-extended-button (rpgetvar 'longwallgobs/blackbody-extended-button))
            (cx-set-toggle-button longwallgobs/cividis-button (rpgetvar 'longwallgobs/cividis-button))
            (cx-set-toggle-button longwallgobs/coolwarm-bent-button (rpgetvar 'longwallgobs/coolwarm-bent-button))
            (cx-set-toggle-button longwallgobs/coolwarm-smooth-button (rpgetvar 'longwallgobs/coolwarm-smooth-button))
            (cx-set-toggle-button longwallgobs/inferno-button (rpgetvar 'longwallgobs/inferno-button))
            (cx-set-toggle-button longwallgobs/kindlmann-button (rpgetvar 'longwallgobs/kindlmann-button))
            (cx-set-toggle-button longwallgobs/kindlmann-extended-button (rpgetvar 'longwallgobs/kindlmann-extended-button))
            (cx-set-toggle-button longwallgobs/magma-button (rpgetvar 'longwallgobs/magma-button))
            (cx-set-toggle-button longwallgobs/plasma-button (rpgetvar 'longwallgobs/plasma-button))
            (cx-set-toggle-button longwallgobs/viridis-button (rpgetvar 'longwallgobs/viridis-button))
		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)
            ; Set variables based on button selection
			(rpsetvar 'longwallgobs/field-velocity-button (cx-show-toggle-button longwallgobs/field-velocity-button))
            (rpsetvar 'longwallgobs/blackbody-button (cx-show-toggle-button longwallgobs/blackbody-button))
            (rpsetvar 'longwallgobs/blackbody-extended-button (cx-show-toggle-button longwallgobs/blackbody-extended-button))
            (rpsetvar 'longwallgobs/cividis-button (cx-show-toggle-button longwallgobs/cividis-button))
            (rpsetvar 'longwallgobs/coolwarm-bent-button (cx-show-toggle-button longwallgobs/coolwarm-bent-button))
            (rpsetvar 'longwallgobs/coolwarm-smooth-button (cx-show-toggle-button longwallgobs/coolwarm-smooth-button))
            (rpsetvar 'longwallgobs/inferno-button (cx-show-toggle-button longwallgobs/inferno-button))
            (rpsetvar 'longwallgobs/kindlmann-button (cx-show-toggle-button longwallgobs/kindlmann-button))
            (rpsetvar 'longwallgobs/kindlmann-extended-button (cx-show-toggle-button longwallgobs/kindlmann-extended-button))
            (rpsetvar 'longwallgobs/magma-button (cx-show-toggle-button longwallgobs/magma-button))
			(rpsetvar 'longwallgobs/plasma-button (cx-show-toggle-button longwallgobs/plasma-button))
            (rpsetvar 'longwallgobs/viridis-button (cx-show-toggle-button longwallgobs/viridis-button))
            ; Set colormap based on radio button selection
            ; (if (equal? (rpgetvar 'longwallgobs/plasma-button) #t) (display (rpgetvar 'longwallgobs/plasma-button)) (display (rpgetvar 'longwallgobs/plasma-button)) )
            ; (if (equal? (rpgetvar 'longwallgobs/plasma-button) #t) (ti-menu-load-string "file/read-colormap colormaps/plasma.colormap") () )
            ; (if (equal? (rpgetvar 'longwallgobs/field-velocity-button) #t) (ti-menu-load-string "preferences/graphics/colormap-settings/colormap/field-velocity\nGo to Contours>Colormap Options...>Currently Defined>field-velocity to apply")) 
            (if (equal? (rpgetvar 'longwallgobs/field-velocity-button) #t) (display "Go to Contours>Colormap Options...>Currently Defined>field-velocity to apply\n")) 
            (if (equal? (rpgetvar 'longwallgobs/blackbody-button) #t) (ti-menu-load-string "file/read-colormap colormaps/blackbody.colormap\n")) 
            (if (equal? (rpgetvar 'longwallgobs/blackbody-extended-button) #t) (ti-menu-load-string "file/read-colormap colormaps/blackbody_extended.colormap\n")) 
            (if (equal? (rpgetvar 'longwallgobs/cividis-button) #t) (ti-menu-load-string "file/read-colormap colormaps/cividis.colormap\n")) 
            (if (equal? (rpgetvar 'longwallgobs/coolwarm-bent-button) #t) (ti-menu-load-string "file/read-colormap colormaps/coolwarm_bent.colormap\n")) 
            (if (equal? (rpgetvar 'longwallgobs/coolwarm-smooth-button) #t) (ti-menu-load-string "file/read-colormap colormaps/coolwarm_smooth.colormap\n")) 
            (if (equal? (rpgetvar 'longwallgobs/inferno-button) #t) (ti-menu-load-string "file/read-colormap colormaps/inferno.colormap\n")) 
            (if (equal? (rpgetvar 'longwallgobs/kindlmann-button) #t) (ti-menu-load-string "file/read-colormap colormaps/kindlmann.colormap\n")) 
            (if (equal? (rpgetvar 'longwallgobs/kindlmann-extended-button) #t) (ti-menu-load-string "file/read-colormap colormaps/kindlmann_extended.colormap\n")) 
            (if (equal? (rpgetvar 'longwallgobs/magma-button) #t) (ti-menu-load-string "file/read-colormap colormaps/magma.colormap\n")) 
            (if (equal? (rpgetvar 'longwallgobs/plasma-button) #t) (ti-menu-load-string "file/read-colormap colormaps/plasma.colormap\n"))
            (if (equal? (rpgetvar 'longwallgobs/viridis-button) #t) (ti-menu-load-string "file/read-colormap colormaps/viridis.colormap\n")) 
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
					(set! longwallgobs/field-velocity-button (cx-create-toggle-button longwallgobs/colormap_button_box "Field Velocity (Default)"))
					(set! longwallgobs/blackbody-button (cx-create-toggle-button longwallgobs/colormap_button_box "Black Body"))
					(set! longwallgobs/blackbody-extended-button (cx-create-toggle-button longwallgobs/colormap_button_box "Extended Black Body"))
					(set! longwallgobs/cividis-button (cx-create-toggle-button longwallgobs/colormap_button_box "Cividis"))
					(set! longwallgobs/coolwarm-bent-button (cx-create-toggle-button longwallgobs/colormap_button_box "Smooth Cool Warm"))
					(set! longwallgobs/coolwarm-smooth-button (cx-create-toggle-button longwallgobs/colormap_button_box "Bent Cool Warm"))
					(set! longwallgobs/inferno-button (cx-create-toggle-button longwallgobs/colormap_button_box "Inferno"))
					(set! longwallgobs/kindlmann-button (cx-create-toggle-button longwallgobs/colormap_button_box "Kindlmann"))
					(set! longwallgobs/kindlmann-extended-button (cx-create-toggle-button longwallgobs/colormap_button_box "Extended Kindlmann"))
					(set! longwallgobs/magma-button (cx-create-toggle-button longwallgobs/colormap_button_box "Magma"))
					(set! longwallgobs/plasma-button (cx-create-toggle-button longwallgobs/colormap_button_box "Plasma"))
					(set! longwallgobs/viridis-button (cx-create-toggle-button longwallgobs/colormap_button_box "Viridis"))
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