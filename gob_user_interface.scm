; copy source files up to working directory (b/c Fluent simply cannot handle directories)
(if (unix?)
	(ti-menu-load-string "! cp -r src/* include/* .")

	(begin
		(ti-menu-load-string "! copy src/*")
		(ti-menu-load-string "! copy include/*")
	)
)

; setup egz colormap
(load "colormap_selection.scm")
(ti-menu-load-string "file/read-colormap colormaps/explosive_plots.colormap\n")
(ti-menu-load-string "file/read-colormap colormaps/viridis.colormap\n")

; allocate and initialize UDMs
(ti-menu-load-string "define/user-defined/user-defined-memory 6\n")
(ti-menu-load-string "solve/initialize/initialize-flow yes\n")

; compile and load UDF library
(ti-menu-load-string "define/user-defined/use-built-in-compiler yes\n")
(ti-menu-load-string "define/user-defined/compiled-functions compile longwallgobs yes fits.c udf_main.c utils.c \"\" fits.h udf_explosive_mix.h udf_inertia.h udf_permeability.h udf_porosity.h udf_vsi.h utils.h \"\"\n")
(ti-menu-load-string "define/user-defined/compiled-functions load longwallgobs\n")

; set adjust function hook
(ti-menu-load-string "define/user-defined/function-hooks/adjust \"demo_calc::longwallgobs\"")

(ti-menu-load-string "define/models/species/species-transport yes methane-air")
(ti-menu-load-string "solve/set/number-of-iterations 1")
(ti-menu-load-string (string-append "solve/initialize/set-defaults species-0 0"))
(ti-menu-load-string (string-append "solve/initialize/set-defaults species-1 0"))
(ti-menu-load-string "solve/initialize/compute-defaults all-zones")
(ti-menu-load-string "solve/initialize/initialize-flow yes\n")

; compile string parser
(ti-menu-load-string "! g++ -o parser parser.cpp -static\n")

; https://stackoverflow.com/questions/29737958/scheme-how-to-find-a-position-of-a-char-in-a-string
(define (string-search-forward char-list char pos)
  (cond ((null? char-list) #f)              ; list was empty
        ((char=? char (car char-list)) pos) ; we found it!
        (else (string-search-forward (cdr char-list) char (+ pos 1))))) ; char was not found

; utility function to insert a string in another before the first instance of "."
(define (string-insert n i)
	(if (not (string-search-forward (string->list n) #\. 0))
		(string-append n i)

		(string-append
			(substring n 0 (string-search-forward (string->list n) #\. 0))
			i
			(substring n (string-search-forward (string->list n) #\. 0) (string-length n))
		)
	)
)

; get names of fluid zones
(define zone_names
	(map symbol->string
		(filter
			(lambda (name)
				(and
				(equal?
					(symbol->string
						(zone-type (get-zone name))
					)
					"fluid"
				)
				(substring?
					"gob"
					(symbol->string
						name
					)
				)
				)
			)
			(inquire-zone-names)
		)
	)
)

(define delete
  (lambda (item list)
    (cond
     ((equal? item (car list)) (cdr list))
     (else (cons (car list) (delete item (cdr list)))))))

; RP Variable Create Function
(define (make-new-rpvar name default type)
	(if (not (rp-var-object name))
		(rp-var-define name default type #f)))

; RP variable declarations
; Declare variables for Model Type and Required Settings Box
(make-new-rpvar 'mine_c_radio_button #t 'boolean)
(make-new-rpvar 'mine_e_radio_button #f 'boolean)
(make-new-rpvar 'mine_t_radio_button #f 'boolean)

; I hate Fluent so much
(make-new-rpvar 'mine_c #t 'boolean)
(make-new-rpvar 'mine_e #f 'boolean)
(make-new-rpvar 'mine_t #f 'boolean)


; Declare variables for Explosive Gas Zone option
(make-new-rpvar 'longwallgobs/egz_radio_button #f 'boolean)

(make-new-rpvar 'longwallgobs/methane 0 'real)
(make-new-rpvar 'longwallgobs/oxygen 0 'real)

; Declare variables for Optional Settings Box
(make-new-rpvar 'longwallgobs/resist_scaler 1 'real)
(make-new-rpvar 'longwallgobs/max_resistance 5.0E6 'real)
(make-new-rpvar 'longwallgobs/min_resistance 1.45E5 'real)
(make-new-rpvar 'longwallgobs/max_porosity 0.40 'real)
(make-new-rpvar 'longwallgobs/initial_porosity 1 'real)
(make-new-rpvar 'longwallgobs/max_vsi 0.40 'real)
(make-new-rpvar 'longwallgobs/min_inertial_resistance 0 'real)
(make-new-rpvar 'longwallgobs/max_inertial_resistance 1.3E5 'real)

; Declare Variables for Zone Selection box
(make-new-rpvar 'longwallgobs/startup_room_center_radio_button #t 'boolean)
(make-new-rpvar 'longwallgobs/startup_room_corner_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/mid_panel_center_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/mid_panel_gateroad_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/working_face_center_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/working_face_corner_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/single_part_mesh_radio_button #f 'boolean)
; Declare variables for zone IDs
(make-new-rpvar 'longwallgobs/startup_room_center_id -1 'integer)
(make-new-rpvar 'longwallgobs/startup_room_corner_id -1 'integer)
(make-new-rpvar 'longwallgobs/mid_panel_center_id -1 'integer)
(make-new-rpvar 'longwallgobs/mid_panel_gateroad_id -1 'integer)
(make-new-rpvar 'longwallgobs/working_face_center_id -1 'integer)
(make-new-rpvar 'longwallgobs/working_face_corner_id -1 'integer)
(make-new-rpvar 'longwallgobs/single_part_mesh_id -1 'integer)
; Declare variables for zone info lists
(make-new-rpvar 'longwallgobs/zone_names_selected '() 'list)
(make-new-rpvar 'longwallgobs/zone_names '() 'list)
; Declare variables for zone dimensions
(make-new-rpvar 'longwallgobs/startup_room_center_min_x 0 'real)
(make-new-rpvar 'longwallgobs/startup_room_center_max_x 0 'real)
(make-new-rpvar 'longwallgobs/startup_room_center_min_y 0 'real)
(make-new-rpvar 'longwallgobs/startup_room_center_max_y 0 'real)

(make-new-rpvar 'longwallgobs/startup_room_corner_min_x 0 'real)
(make-new-rpvar 'longwallgobs/startup_room_corner_max_x 0 'real)
(make-new-rpvar 'longwallgobs/startup_room_corner_min_y 0 'real)
(make-new-rpvar 'longwallgobs/startup_room_corner_max_y 0 'real)

(make-new-rpvar 'longwallgobs/mid_panel_center_min_x 0 'real)
(make-new-rpvar 'longwallgobs/mid_panel_center_max_x 0 'real)
(make-new-rpvar 'longwallgobs/mid_panel_center_min_y 0 'real)
(make-new-rpvar 'longwallgobs/mid_panel_center_max_y 0 'real)

(make-new-rpvar 'longwallgobs/mid_panel_gateroad_min_x 0 'real)
(make-new-rpvar 'longwallgobs/mid_panel_gateroad_max_x 0 'real)
(make-new-rpvar 'longwallgobs/mid_panel_gateroad_min_y 0 'real)
(make-new-rpvar 'longwallgobs/mid_panel_gateroad_max_y 0 'real)

(make-new-rpvar 'longwallgobs/working_face_center_min_x 0 'real)
(make-new-rpvar 'longwallgobs/working_face_center_max_x 0 'real)
(make-new-rpvar 'longwallgobs/working_face_center_min_y 0 'real)
(make-new-rpvar 'longwallgobs/working_face_center_max_y 0 'real)

(make-new-rpvar 'longwallgobs/working_face_corner_min_x 0 'real)
(make-new-rpvar 'longwallgobs/working_face_corner_max_x 0 'real)
(make-new-rpvar 'longwallgobs/working_face_corner_min_y 0 'real)
(make-new-rpvar 'longwallgobs/working_face_corner_max_y 0 'real)

(make-new-rpvar 'longwallgobs/single_part_mesh_min_x 0 'real)
(make-new-rpvar 'longwallgobs/single_part_mesh_max_x 0 'real)
(make-new-rpvar 'longwallgobs/single_part_mesh_min_y 0 'real)
(make-new-rpvar 'longwallgobs/single_part_mesh_max_y 0 'real)
(define longwallgobs/all_zones_selected '())

; Model Type and Required Settings Definition
(define model_type_and_required_settings_box
	;Let Statement, Local Variable Declarations
	(let ((panel #f)
		; Required Settings
        (tframe)
        (ttab1)
        (ttab2)
        (ttab3)
		(table)
        (longwallgobs/mine_button_box)
        (mine_c_radio_button)
        (mine_e_radio_button)
        (mine_t_radio_button)

        (longwallgobs/egz_button_box)
        (longwallgobs/egz_radio_button)


		(longwallgobs/gas_table)
		(longwallgobs/methane)
		(longwallgobs/oxygen)
		(longwallgobs/init_gas)

		; Optional Settings
		(table2)
        (longwallgobs/optional_param_table)
        (longwallgobs/resist_scaler)
        (longwallgobs/max_resistance)
        (longwallgobs/min_resistance)
        (longwallgobs/max_porosity)
        (longwallgobs/initial_porosity)
        (longwallgobs/max_vsi)
		(longwallgobs/min_inertial_resistance)
		(longwallgobs/max_inertial_resistance)

		; Zone Selection
		(table3)
        (longwallgobs/zone_button_box)
        (longwallgobs/startup_room_center_radio_button)	
        (longwallgobs/startup_room_corner_radio_button)
        (longwallgobs/mid_panel_center_radio_button)	
        (longwallgobs/mid_panel_gateroad_radio_button)	
        (longwallgobs/working_face_center_radio_button)	
        (longwallgobs/working_face_corner_radio_button)	
        (longwallgobs/single_part_mesh_radio_button)
        (longwallgobs/apply_button)
        (longwallgobs/clear_button)
        ;(longwallgobs/startup_room_center_id)
        (longwallgobs/zone_names)
		)

		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			; Required Settings
			(cx-set-toggle-button mine_c_radio_button (rpgetvar 'mine_c_radio_button))
			(cx-set-toggle-button mine_e_radio_button (rpgetvar 'mine_e_radio_button))
			(cx-set-toggle-button mine_t_radio_button (rpgetvar 'mine_t_radio_button))

			(cx-set-toggle-button longwallgobs/egz_radio_button (rpgetvar 'longwallgobs/egz_radio_button))
 
			(cx-set-real-entry longwallgobs/methane (rpgetvar 'longwallgobs/methane))
			(cx-set-real-entry longwallgobs/oxygen (rpgetvar 'longwallgobs/oxygen))

			; Optional Settings
			(cx-set-real-entry longwallgobs/resist_scaler (rpgetvar 'longwallgobs/resist_scaler))
			(cx-set-real-entry longwallgobs/max_resistance (rpgetvar 'longwallgobs/max_resistance))
			(cx-set-real-entry longwallgobs/min_resistance (rpgetvar 'longwallgobs/min_resistance))
			(cx-set-real-entry longwallgobs/max_porosity (rpgetvar 'longwallgobs/max_porosity))
			(cx-set-real-entry longwallgobs/initial_porosity (rpgetvar 'longwallgobs/initial_porosity))
			(cx-set-real-entry longwallgobs/max_vsi (rpgetvar 'longwallgobs/max_vsi))
			(cx-set-real-entry longwallgobs/min_inertial_resistance (rpgetvar 'longwallgobs/min_inertial_resistance))
			(cx-set-real-entry longwallgobs/max_inertial_resistance (rpgetvar 'longwallgobs/max_inertial_resistance))


			; Zone Selection
			(cx-set-toggle-button longwallgobs/startup_room_center_radio_button (rpgetvar 'longwallgobs/startup_room_center_radio_button))
			(cx-set-toggle-button longwallgobs/startup_room_corner_radio_button (rpgetvar 'longwallgobs/startup_room_corner_radio_button))
			(cx-set-toggle-button longwallgobs/mid_panel_center_radio_button (rpgetvar 'longwallgobs/mid_panel_center_radio_button))
			(cx-set-toggle-button longwallgobs/mid_panel_gateroad_radio_button (rpgetvar 'longwallgobs/mid_panel_gateroad_radio_button))
			(cx-set-toggle-button longwallgobs/working_face_center_radio_button (rpgetvar 'longwallgobs/working_face_center_radio_button))
			(cx-set-toggle-button longwallgobs/working_face_corner_radio_button (rpgetvar 'longwallgobs/working_face_corner_radio_button))
			(cx-set-toggle-button longwallgobs/single_part_mesh_radio_button (rpgetvar 'longwallgobs/single_part_mesh_radio_button))
			(cx-set-list-items longwallgobs/zone_names zone_names)
			
		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)
			; Required Settings
			(rpsetvar 'mine_c_radio_button (cx-show-toggle-button mine_c_radio_button))
			(rpsetvar 'mine_e_radio_button (cx-show-toggle-button mine_e_radio_button))
			(rpsetvar 'mine_t_radio_button (cx-show-toggle-button mine_t_radio_button))

			; seriously, Fluent is the worst thing ever created wtf
			(rpsetvar 'mine_c (cx-show-toggle-button mine_c_radio_button))
			(rpsetvar 'mine_e (cx-show-toggle-button mine_e_radio_button))
			(rpsetvar 'mine_t (cx-show-toggle-button mine_t_radio_button))

			(rpsetvar 'longwallgobs/egz_radio_button (cx-show-toggle-button longwallgobs/egz_radio_button))

			(rpsetvar 'longwallgobs/methane (cx-show-real-entry longwallgobs/methane))
			(rpsetvar 'longwallgobs/oxygen (cx-show-real-entry longwallgobs/oxygen))

			; Optional Settings
			(rpsetvar 'longwallgobs/resist_scaler (cx-show-real-entry longwallgobs/resist_scaler))
			(rpsetvar 'longwallgobs/max_resistance (cx-show-real-entry longwallgobs/max_resistance))
			(rpsetvar 'longwallgobs/min_resistance (cx-show-real-entry longwallgobs/min_resistance))
			(rpsetvar 'longwallgobs/max_porosity (cx-show-real-entry longwallgobs/max_porosity))
			(rpsetvar 'longwallgobs/initial_porosity (cx-show-real-entry longwallgobs/initial_porosity))
			(rpsetvar 'longwallgobs/max_vsi (cx-show-real-entry longwallgobs/max_vsi))
			(rpsetvar 'longwallgobs/min_inertial_resistance (cx-show-real-entry longwallgobs/min_inertial_resistance))
			(rpsetvar 'longwallgobs/max_inertial_resistance (cx-show-real-entry longwallgobs/max_inertial_resistance))



			; Zone Selection
			(rpsetvar 'longwallgobs/startup_room_center_radio_button (cx-show-toggle-button longwallgobs/startup_room_center_radio_button))
			(rpsetvar 'longwallgobs/startup_room_corner_radio_button (cx-show-toggle-button longwallgobs/startup_room_corner_radio_button))
			(rpsetvar 'longwallgobs/mid_panel_center_radio_button (cx-show-toggle-button longwallgobs/mid_panel_center_radio_button))
			(rpsetvar 'longwallgobs/mid_panel_gateroad_radio_button  (cx-show-toggle-button longwallgobs/mid_panel_gateroad_radio_button ))
			(rpsetvar 'longwallgobs/working_face_center_radio_button (cx-show-toggle-button longwallgobs/working_face_center_radio_button))
			(rpsetvar 'longwallgobs/working_face_corner_radio_button (cx-show-toggle-button longwallgobs/working_face_corner_radio_button))
			(rpsetvar 'longwallgobs/single_part_mesh_radio_button (cx-show-toggle-button longwallgobs/single_part_mesh_radio_button))
			(rpsetvar 'longwallgobs/zone_names_selected (cx-show-list-selections longwallgobs/zone_names))

			; Get mesh dimensions from Fluent
			(if (> (rpgetvar 'longwallgobs/startup_room_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/startup_room_center_id))) ":1")))) " , x-coordinate yes startup_room_center_min_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/startup_room_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/startup_room_center_id))) ":1")))) " , x-coordinate yes startup_room_center_max_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/startup_room_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/startup_room_center_id))) ":1")))) " , y-coordinate yes startup_room_center_min_y.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/startup_room_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/startup_room_center_id))) ":1")))) " , y-coordinate yes startup_room_center_max_y.txt no yes")))

			(if (> (rpgetvar 'longwallgobs/startup_room_corner_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/startup_room_corner_id))) ":1")))) " , x-coordinate yes startup_room_corner_min_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/startup_room_corner_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/startup_room_corner_id))) ":1")))) " , x-coordinate yes startup_room_corner_max_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/startup_room_corner_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/startup_room_corner_id))) ":1")))) " , y-coordinate yes startup_room_corner_min_y.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/startup_room_corner_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/startup_room_corner_id))) ":1")))) " , y-coordinate yes startup_room_corner_max_y.txt no yes")))

			(if (> (rpgetvar 'longwallgobs/mid_panel_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/mid_panel_center_id))) ":1")))) " , x-coordinate yes mid_panel_center_min_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/mid_panel_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/mid_panel_center_id))) ":1")))) " , x-coordinate yes mid_panel_center_max_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/mid_panel_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/mid_panel_center_id))) ":1")))) " , y-coordinate yes mid_panel_center_min_y.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/mid_panel_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/mid_panel_center_id))) ":1")))) " , y-coordinate yes mid_panel_center_max_y.txt no yes")))

			(if (> (rpgetvar 'longwallgobs/mid_panel_gateroad_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/mid_panel_gateroad_id))) ":1")))) " , x-coordinate yes mid_panel_gateroad_min_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/mid_panel_gateroad_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/mid_panel_gateroad_id))) ":1")))) " , x-coordinate yes mid_panel_gateroad_max_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/mid_panel_gateroad_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/mid_panel_gateroad_id))) ":1")))) " , y-coordinate yes mid_panel_gateroad_min_y.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/mid_panel_gateroad_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/mid_panel_gateroad_id))) ":1")))) " , y-coordinate yes mid_panel_gateroad_max_y.txt no yes")))

			(if (> (rpgetvar 'longwallgobs/working_face_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/working_face_center_id))) ":1")))) " , x-coordinate yes working_face_center_min_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/working_face_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/working_face_center_id))) ":1")))) " , x-coordinate yes working_face_center_max_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/working_face_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/working_face_center_id))) ":1")))) " , y-coordinate yes working_face_center_min_y.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/working_face_center_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/working_face_center_id))) ":1")))) " , y-coordinate yes working_face_center_max_y.txt no yes")))

			(if (> (rpgetvar 'longwallgobs/working_face_corner_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/working_face_corner_id))) ":1")))) " , x-coordinate yes working_face_corner_min_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/working_face_corner_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/working_face_corner_id))) ":1")))) " , x-coordinate yes working_face_corner_max_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/working_face_corner_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/working_face_corner_id))) ":1")))) " , y-coordinate yes working_face_corner_min_y.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/working_face_corner_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/working_face_corner_id))) ":1")))) " , y-coordinate yes working_face_corner_max_y.txt no yes")))

			(if (> (rpgetvar 'longwallgobs/single_part_mesh_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/single_part_mesh_id))) ":1")))) " , x-coordinate yes single_part_mesh_min_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/single_part_mesh_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/single_part_mesh_id))) ":1")))) " , x-coordinate yes single_part_mesh_max_x.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/single_part_mesh_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-min " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/single_part_mesh_id))) ":1")))) " , y-coordinate yes single_part_mesh_min_y.txt no yes")))
			(if (> (rpgetvar 'longwallgobs/single_part_mesh_id) -1) (ti-menu-load-string (string-append (string-append "report/surface-integrals vertex-max " (number->string (surface-name->id (string-insert (symbol->string (zone-id->name (rpgetvar 'longwallgobs/single_part_mesh_id))) ":1")))) " , y-coordinate yes single_part_mesh_max_y.txt no yes")))

			(ti-menu-load-string "! ./parser")
			(load "set_dimensions.scm")

			(%run-udf-apply 1)

			(define surface-append (lambda (zone_name) (surface-name->id(string-insert zone_name ":1"))))
			(make-new-rpvar 'longwallgobs/surface_list '() 'list)
			(rpsetvar 'longwallgobs/surface_list (map surface-append zone_names))
			; delete any previous contours
			(ti-menu-load-string "display/objects/delete explosive-gas-zone")
			(ti-menu-load-string "display/objects/delete volumetric-strain-increment")
			(ti-menu-load-string "display/objects/delete explosive-integral")
			(ti-menu-load-string "display/objects/delete inertial-resistance")
			(ti-menu-load-string "display/objects/delete permeability")
			(ti-menu-load-string "display/objects/delete porosity")

			; create contours
			(if (equal? (cx-show-toggle-button longwallgobs/egz_radio_button) #t) ( begin 
			(ti-menu-load-string "display/objects/create contour explosive-gas-zone color-map color \"explosive_plots\" q field udm-2 range-option auto-range-off minimum 0 maximum 1 q surfaces-list (rpgetvar 'longwallgobs/surface_list)")
			(ti-menu-load-string "display/objects/create contour explosive-integral color-map color \"viridis\" q field udm-3 range-option auto-range-off minimum 0 maximum 1 q surfaces-list (rpgetvar 'longwallgobs/surface_list)")
			(ti-menu-load-string "display/objects/create contour inertial-resistance color-map color \"viridis\" q field udm-5 range-option auto-range-off minimum 0 maximum 1 q surfaces-list (rpgetvar 'longwallgobs/surface_list)")
			(ti-menu-load-string "display/objects/create contour permeability color-map color \"viridis\" q field udm-0 range-option auto-range-off minimum 0 maximum 1 q surfaces-list (rpgetvar 'longwallgobs/surface_list)")
			(ti-menu-load-string "display/objects/create contour porosity color-map color \"viridis\" q field udm-1 range-option auto-range-off minimum 0 maximum 1 q surfaces-list (rpgetvar 'longwallgobs/surface_list)")
			))
			(ti-menu-load-string "display/objects/create contour volumetric-strain-increment color-map color \"viridis\" q field udm-4 range-option auto-range-on q surfaces-list (rpgetvar 'longwallgobs/surface_list)")
			
		)

		; Creating 'apply' button
		(define (select-button-cb . args)
			; Get zone id for each zone
			(if (not (pair? (cx-show-list-selections longwallgobs/zone_names))) (display "No zone selected\n"))
			(if (and (equal? (cx-show-toggle-button longwallgobs/startup_room_center_radio_button) #t) (pair? (cx-show-list-selections longwallgobs/zone_names))) (rpsetvar 'longwallgobs/startup_room_center_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0))))
			(if (and (equal? (cx-show-toggle-button longwallgobs/startup_room_corner_radio_button) #t) (pair? (cx-show-list-selections longwallgobs/zone_names))) (rpsetvar 'longwallgobs/startup_room_corner_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			(if (and (equal? (cx-show-toggle-button longwallgobs/mid_panel_center_radio_button) #t) (pair? (cx-show-list-selections longwallgobs/zone_names))) (rpsetvar 'longwallgobs/mid_panel_center_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			(if (and (equal? (cx-show-toggle-button longwallgobs/mid_panel_gateroad_radio_button) #t) (pair? (cx-show-list-selections longwallgobs/zone_names))) (rpsetvar 'longwallgobs/mid_panel_gateroad_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			(if (and (equal? (cx-show-toggle-button longwallgobs/working_face_center_radio_button) #t) (pair? (cx-show-list-selections longwallgobs/zone_names))) (rpsetvar 'longwallgobs/working_face_center_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			(if (and (equal? (cx-show-toggle-button longwallgobs/working_face_corner_radio_button) #t) (pair? (cx-show-list-selections longwallgobs/zone_names))) (rpsetvar 'longwallgobs/working_face_corner_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			(if (and (equal? (cx-show-toggle-button longwallgobs/single_part_mesh_radio_button) #t) (pair? (cx-show-list-selections longwallgobs/zone_names))) (rpsetvar 'longwallgobs/single_part_mesh_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			
			; Set up zone conditions based on mine selected
			(if (unix?)
				(if (pair? (cx-show-list-selections longwallgobs/zone_names)) (ti-menu-load-string (string-append "/define/boundary-conditions/fluid " (string-append (list-ref (cx-show-list-selections longwallgobs/zone_names) 0) " no no no no no 0 no 0 no 0 no 0 no 0 no 1 no no no yes no no yes yes \"udf\" \"set_perm_1_VSI::longwallgobs\" yes yes \"udf\" \"set_perm_2_VSI::longwallgobs\" yes yes \"udf\" \"set_perm_3_VSI::longwallgobs\" no yes yes \"udf\" \"set_inertia_1_VSI::longwallgobs\" yes yes \"udf\" \"set_inertia_2_VSI::longwallgobs\" yes yes \"udf\" \"set_inertia_3_VSI::longwallgobs\" 0 0 yes yes \"udf\" \"set_poro_VSI::longwallgobs\" constant 1 no"))))
				(if (pair? (cx-show-list-selections longwallgobs/zone_names)) (ti-menu-load-string (string-append "/define/boundary-conditions/fluid " (string-append (list-ref (cx-show-list-selections longwallgobs/zone_names) 0) " no no no no no 0 no 0 no 0 no 0 no 0 no 1 none no no no yes no no 1 no 0 no 0 no 0 no 1 no 0 yes yes yes \"udf\" \"set_perm_1_VSI::longwallgobs\" yes yes \"udf\" \"set_perm_2_VSI::longwallgobs\" yes yes \"udf\" \"set_perm_3_VSI::longwallgobs\" no yes yes \"udf\" \"set_inertia_1_VSI::longwallgobs\" yes yes \"udf\" \"set_inertia_2_VSI::longwallgobs\" yes yes \"udf\" \"set_inertia_3_VSI::longwallgobs\" 0 0 yes yes \"udf\" \"set_poro_VSI::longwallgobs\" constant 1 no"))))
			)
		)

		(define (init-gas-cb . args)
			(ti-menu-load-string "define/models/species/species-transport yes methane-air")
			(ti-menu-load-string "solve/set/number-of-iterations 1")
			(ti-menu-load-string (string-append "solve/initialize/set-defaults species-0 " (number->string  (cx-show-real-entry longwallgobs/methane))))
			(ti-menu-load-string (string-append "solve/initialize/set-defaults species-1 " (number->string  (cx-show-real-entry longwallgobs/oxygen))))
			(ti-menu-load-string "solve/initialize/compute-defaults all-zones")
			(ti-menu-load-string "solve/initialize/initialize-flow yes\n")
		)

		(define (clear-button-cb . args)
			(if (equal? (cx-show-toggle-button longwallgobs/startup_room_center_radio_button) #t) (rpsetvar 'longwallgobs/startup_room_center_id -1))
			(if (equal? (cx-show-toggle-button longwallgobs/startup_room_corner_radio_button) #t) (rpsetvar 'longwallgobs/startup_room_corner_id -1)) 
			(if (equal? (cx-show-toggle-button longwallgobs/mid_panel_center_radio_button) #t) (rpsetvar 'longwallgobs/mid_panel_center_id -1)) 
			(if (equal? (cx-show-toggle-button longwallgobs/mid_panel_gateroad_radio_button) #t) (rpsetvar 'longwallgobs/mid_panel_gateroad_id -1)) 
			(if (equal? (cx-show-toggle-button longwallgobs/working_face_center_radio_button) #t) (rpsetvar 'longwallgobs/working_face_center_id -1)) 
			(if (equal? (cx-show-toggle-button longwallgobs/working_face_corner_radio_button) #t) (rpsetvar 'longwallgobs/working_face_corner_id -1)) 
			(if (equal? (cx-show-toggle-button longwallgobs/single_part_mesh_radio_button) #t) (rpsetvar 'longwallgobs/single_part_mesh_id -1)) 
			
		)

		(lambda args
			(if (not panel)
				(let ()
					; Required Settings
                    (set! panel (cx-create-panel "Mine Model Gob" 'update-callback update-cb 'apply-callback apply-cb))
                    (set! tframe (cx-create-frame-tabbed panel "Tabs" 'border #t 'below 0 'right-of 0))
                    (set! ttab1 (cx-create-tab tframe "Required Settings"))
                    (set! ttab2 (cx-create-tab tframe "Optional Settings"))
                    (set! ttab3 (cx-create-tab tframe "Zone Selection"))

					(set! table (cx-create-table ttab1 ""))

					(set! longwallgobs/mine_button_box (cx-create-button-box table "Mine Model" 'radio-mode #t 'col 0))
					(set! mine_c_radio_button (cx-create-toggle-button longwallgobs/mine_button_box "Mine C Model"))
					(set! mine_e_radio_button (cx-create-toggle-button longwallgobs/mine_button_box "Mine E Model"))
					(set! mine_t_radio_button (cx-create-toggle-button longwallgobs/mine_button_box "Mine T Model"))

					(set! longwallgobs/egz_button_box (cx-create-button-box table "" 'radio-mode #f 'row 1 'col 0))
					(set! longwallgobs/egz_radio_button (cx-create-toggle-button longwallgobs/egz_button_box "Explosive Gas Zone Colorization"))

					(set! longwallgobs/gas_table (cx-create-table table "Gas Concentrations (Mass Fractions)" 'row 0 'col 1))
					(set! longwallgobs/methane (cx-create-real-entry longwallgobs/gas_table "Methane" 'row 0 'col 0))
					(set! longwallgobs/oxygen (cx-create-real-entry longwallgobs/gas_table "Oxygen" 'row 1 'col 0))
					(set! longwallgobs/init_gas (cx-create-button longwallgobs/gas_table "Apply" 'activate-callback init-gas-cb 'row 1 'col 1))

					; Optional Settings
					(set! table2 (cx-create-table ttab2 ""))

					(set! longwallgobs/optional_param_table (cx-create-table table2 "Optional Settings" 'row 0 'col 1))
					(set! longwallgobs/resist_scaler (cx-create-real-entry longwallgobs/optional_param_table "Resistance Scalar" 'row 0 'col 0))
					(set! longwallgobs/max_resistance (cx-create-real-entry longwallgobs/optional_param_table "Max Resistance" 'row 0 'col 1))
					(set! longwallgobs/min_resistance (cx-create-real-entry longwallgobs/optional_param_table "Min Resistance" 'row 0 'col 2))
					(set! longwallgobs/max_porosity (cx-create-real-entry longwallgobs/optional_param_table "Max Porosity" 'row 1 'col 0))
					(set! longwallgobs/initial_porosity (cx-create-real-entry longwallgobs/optional_param_table "Initial Porosity" 'row 1 'col 1))
					(set! longwallgobs/max_vsi (cx-create-real-entry longwallgobs/optional_param_table "Max VSI" 'row 1 'col 2))
					(set! longwallgobs/min_inertial_resistance (cx-create-real-entry longwallgobs/optional_param_table "Min Inertial Resistance" 'row 0 'col 3))
					(set! longwallgobs/max_inertial_resistance (cx-create-real-entry longwallgobs/optional_param_table "Max Inertial Resistance" 'row 1 'col 3))

					; Zone Selection
					(set! table3 (cx-create-table ttab3 ""))

					(set! longwallgobs/zone_button_box (cx-create-button-box table3 "Zone Type" 'radio-mode #t 'col 0))

					(set! longwallgobs/startup_room_center_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Startup Center"))
					(set! longwallgobs/startup_room_corner_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Startup Gateroad"))
					(set! longwallgobs/mid_panel_center_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Mid-Panel Center"))
					(set! longwallgobs/mid_panel_gateroad_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Mid-Panel Gateroad"))
					(set! longwallgobs/working_face_center_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Working Center"))
					(set! longwallgobs/working_face_corner_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Working Gateroad"))
					(set! longwallgobs/single_part_mesh_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Single-Part Mesh"))

					(set! longwallgobs/apply_button (cx-create-button table3 "Select Zone" 'activate-callback select-button-cb 'row 1 'col 0))
					(set! longwallgobs/clear_button (cx-create-button table3 "Clear Zone" 'activate-callback clear-button-cb 'row 1 'col 2))

					(set! longwallgobs/zone_names (cx-create-list table3 "Zone Selection" 'visible-lines 9 'multiple-selections #f 'row 0 'col 1))
				) ;End Of Let Statement
			) ;End Of If Statement
			;Call To Open Dialog Box
			(cx-show-panel panel)
		) ;End Of Args Function
	) ;End Of Let Statement
) ;End Of model_type_and_required_settings_box Definition


(if (not (cx-get-menu-id "Model Mine Gob"))
    (cx-add-menu "Model Mine Gob" #\H)
)

(if (not (cx-get-item-id "Model Mine Gob" "Permeability and Porosity"))
    (begin 
        (cx-add-separator "Model Mine Gob")
        (cx-add-item "Model Mine Gob" "Permeability and Porosity" #\H #f cx-client? model_type_and_required_settings_box)
    )
    (begin 
        (cx-delete-item "Model Mine Gob" "Permeability and Porosity" #\H #f cx-client? model_type_and_required_settings_box)
        (cx-add-separator "Model Mine Gob")
        (cx-add-item "Model Mine Gob" "Permeability and Porosity" #\H #f cx-client? model_type_and_required_settings_box)
    )
)
