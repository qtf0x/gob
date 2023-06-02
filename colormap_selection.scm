; Note: Make sure that the folder "colormaps" is in your working directory
; (load "\colormap_selection.scm") 
; https://sjmgarnier.github.io/viridis/

; Issue: Can't get command to load new colormap to execute solely from .scm file
; Currently, the command is put into the console and then "enter" executes command 
; Options:
; 1. Keep it how it is
; 2. Edit .fluent file and then the user edits the colormap in the contour menu
; 3. Find another way

; TODO:
; Add additional colormap options to GUI
; Find better way to execute command
; Maybe add to larger interface
; Add instructions to readme

; Taken from https://github.com/jrwrigh/fluent_colormaps/tree/master
; ------- START
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
; ------- END 

; Define variables for menu
(make-new-rpvar 'gob/field-velocity-button #t 'boolean)
(make-new-rpvar 'gob/plasma-button #f 'boolean)

; Create a menu for various colormap options 
; Colormap Selection Definition
(define colormap-selection-box
	; Let Statement, Local Varaible Declarations
	(let ((dialog-box #f)
		(table)
			(gob/colormap_button_box)
			(gob/field-velocity-button)	
			(gob/plasma-button)
	)
		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			(cx-set-toggle-button gob/field-velocity-button (rpgetvar 'gob/field-velocity-button))
			(cx-set-toggle-button gob/plasma-button (rpgetvar 'gob/plasma-button))
		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)
			(rpsetvar 'gob/field-velocity-button (cx-show-toggle-button gob/field-velocity-button))
			(rpsetvar 'gob/plasma-button (cx-show-toggle-button gob/plasma-button))
            ;(if (equal? (rpgetvar 'gob/plasma-button) #t) (display (rpgetvar 'gob/plasma-button)) (display (rpgetvar 'gob/plasma-button)) )
            ; (if (equal? (rpgetvar 'gob/plasma-button) #t) (ti-menu-load-string "file/read-colormap colormaps/plasma.colormap") () )
            (if (equal? (rpgetvar 'gob/plasma-button) #t) (ti-menu-load-string "file/read-colormap colormaps/plasma.colormap\n")) ; Load plasma colormap if radiobutton selected
		)

		(lambda args
			(if (not dialog-box)
				(let ()
				
					(set! dialog-box (cx-create-panel "Colormap Selection" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))

					(set! gob/colormap_button_box (cx-create-button-box table "Colormap" 'radio-mode #t 'col 0))

					(set! gob/field-velocity-button (cx-create-toggle-button gob/colormap_button_box "Field Velocity (Default)"))
					(set! gob/plasma-button (cx-create-toggle-button gob/colormap_button_box "Plasma"))

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