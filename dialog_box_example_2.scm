; allocate and initialize UDMs
(ti-menu-load-string "define/user-defined/user-defined-memory 6")
(ti-menu-load-string "solve/initialize/initialize-flow")

; compile and load UDF library
(ti-menu-load-string "define/user-defined/use-built-in-compiler yes")
(ti-menu-load-string "define/user-defined/compiled-functions compile longwallgobs yes fits.c udf_main.c utils.c \"\" fits.h udf_adjust.h udf_explosive_mix.h udf_inertia.h udf_permeability.h udf_porosity.h udf_vsi.h utils.h \"\"")
(ti-menu-load-string "define/user-defined/compiled-functions load longwallgobs")

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

; get ids of fluid zones
(define zone_ids
	(map 
		(lambda (zone_name)
			(zone-name->id zone_name)
		)
		zone_names
	)
)

; RP Variable Create Function
(define (make-new-rpvar name default type)
	(if (not (rp-var-object name))
		(rp-var-define name default type #f)))

; RP variable declarations
; Declare variables for Model Type and Required Settings Box
(make-new-rpvar 'longwallgobs/mine_C_radio_button #t 'boolean)
(make-new-rpvar 'longwallgobs/mine_E_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/mine_T_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/panel_half_width 0 'real)
(make-new-rpvar 'longwallgobs/panel_length 0 'real)
(make-new-rpvar 'longwallgobs/panel_x_offset 0 'real)
(make-new-rpvar 'longwallgobs/panel_y_offset 0 'real)
(make-new-rpvar 'longwallgobs/egz_radio_button #f 'boolean)
; Declare variables for Optional Settings Box
(make-new-rpvar 'longwallgobs/resist_scaler 1 'real)
(make-new-rpvar 'longwallgobs/max_resistance 5.0E6 'real)
(make-new-rpvar 'longwallgobs/min_resistance 1.45E5 'real)
(make-new-rpvar 'longwallgobs/max_porosity 0.40 'real)
(make-new-rpvar 'longwallgobs/initial_porosity 0.25778 'real)
(make-new-rpvar 'longwallgobs/max_vsi 0.40 'real)
; Declare Variables for Zone Selection box
(make-new-rpvar 'longwallgobs/startup_room_center_radio_button #t 'boolean)
(make-new-rpvar 'longwallgobs/startup_room_corner_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/mid_panel_center_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/mid_panel_gateroad_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/working_face_center_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/working_face_corner_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/single_part_mesh_radio_button #f 'boolean)
; Declare variables for zone IDs
(make-new-rpvar 'longwallgobs/startup_room_center_id 0 'integer)
(make-new-rpvar 'longwallgobs/startup_room_corner_id 0 'integer)
(make-new-rpvar 'longwallgobs/mid_panel_center_id 0 'integer)
(make-new-rpvar 'longwallgobs/mid_panel_gateroad_id 0 'integer)
(make-new-rpvar 'longwallgobs/working_face_center_id 0 'integer)
(make-new-rpvar 'longwallgobs/working_face_corner_id 0 'integer)
(make-new-rpvar 'longwallgobs/single_part_mesh_id -1 'integer)
; Declare variables for zone info lists
(make-new-rpvar 'longwallgobs/zone_names_selected '() 'list)
(make-new-rpvar 'longwallgobs/zone_names '() 'list)
; (make-new-rpvar 'longwallgobs/zone_ids zone_ids 'list)

; Model Type and Required Settings Definition
(define model_type_and_required_settings_box
	;Let Statement, Local Variable Declarations
	(let ((dialog-box #f)
		; Required Settings
		(table)
			(longwallgobs/mine_button_box)
			(longwallgobs/mine_C_radio_button)
			(longwallgobs/mine_E_radio_button)
			(longwallgobs/mine_T_radio_button)

			(longwallgobs/egz_button_box)
			(longwallgobs/egz_radio_button)

			(longwallgobs/required_param_table)
			(longwallgobs/panel_half_width)
			(longwallgobs/panel_length)
			(longwallgobs/panel_x_offset)
			(longwallgobs/panel_y_offset)

		; Optional Settings
		(table2)
			(longwallgobs/optional_param_table)
			(longwallgobs/resist_scaler)
			(longwallgobs/max_resistance)
			(longwallgobs/min_resistance)
			(longwallgobs/max_porosity)
			(longwallgobs/initial_porosity)
			(longwallgobs/max_vsi)

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
			(cx-set-toggle-button longwallgobs/mine_C_radio_button (rpgetvar 'longwallgobs/mine_C_radio_button))
			(cx-set-toggle-button longwallgobs/mine_E_radio_button (rpgetvar 'longwallgobs/mine_E_radio_button))
			(cx-set-toggle-button longwallgobs/mine_T_radio_button (rpgetvar 'longwallgobs/mine_T_radio_button))

			(cx-set-toggle-button longwallgobs/egz_radio_button (rpgetvar 'longwallgobs/egz_radio_button))

			(cx-set-real-entry longwallgobs/panel_half_width (rpgetvar 'longwallgobs/panel_half_width))
			(cx-set-real-entry longwallgobs/panel_length (rpgetvar 'longwallgobs/panel_length))
			(cx-set-real-entry longwallgobs/panel_x_offset (rpgetvar 'longwallgobs/panel_x_offset))
			(cx-set-real-entry longwallgobs/panel_y_offset (rpgetvar 'longwallgobs/panel_y_offset))

			; Optional Settings
			(cx-set-real-entry longwallgobs/resist_scaler (rpgetvar 'longwallgobs/resist_scaler))
			(cx-set-real-entry longwallgobs/max_resistance (rpgetvar 'longwallgobs/max_resistance))
			(cx-set-real-entry longwallgobs/min_resistance (rpgetvar 'longwallgobs/min_resistance))
			(cx-set-real-entry longwallgobs/max_porosity (rpgetvar 'longwallgobs/max_porosity))
			(cx-set-real-entry longwallgobs/initial_porosity (rpgetvar 'longwallgobs/initial_porosity))
			(cx-set-real-entry longwallgobs/max_vsi (rpgetvar 'longwallgobs/max_vsi))

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
			(rpsetvar 'longwallgobs/mine_C_radio_button (cx-show-toggle-button longwallgobs/mine_C_radio_button))
			(rpsetvar 'longwallgobs/mine_E_radio_button (cx-show-toggle-button longwallgobs/mine_E_radio_button))
			(rpsetvar 'longwallgobs/mine_T_radio_button (cx-show-toggle-button longwallgobs/mine_T_radio_button))

			(rpsetvar 'longwallgobs/egz_radio_button (cx-show-toggle-button longwallgobs/egz_radio_button))

			(rpsetvar 'longwallgobs/panel_half_width (cx-show-real-entry longwallgobs/panel_half_width))
			(rpsetvar 'longwallgobs/panel_length (cx-show-real-entry longwallgobs/panel_length))
			(rpsetvar 'longwallgobs/panel_x_offset (cx-show-real-entry longwallgobs/panel_x_offset))
			(rpsetvar 'longwallgobs/panel_y_offset (cx-show-real-entry longwallgobs/panel_y_offset))

			; Optional Settings
			(rpsetvar 'longwallgobs/resist_scaler (cx-show-real-entry longwallgobs/resist_scaler))
			(rpsetvar 'longwallgobs/max_resistance (cx-show-real-entry longwallgobs/max_resistance))
			(rpsetvar 'longwallgobs/min_resistance (cx-show-real-entry longwallgobs/min_resistance))
			(rpsetvar 'longwallgobs/max_porosity (cx-show-real-entry longwallgobs/max_porosity))
			(rpsetvar 'longwallgobs/initial_porosity (cx-show-real-entry longwallgobs/initial_porosity))
			(rpsetvar 'longwallgobs/max_vsi (cx-show-real-entry longwallgobs/max_vsi))

			; Zone Selection
			(rpsetvar 'longwallgobs/startup_room_center_radio_button (cx-show-toggle-button longwallgobs/startup_room_center_radio_button))
			(rpsetvar 'longwallgobs/startup_room_corner_radio_button (cx-show-toggle-button longwallgobs/startup_room_corner_radio_button))
			(rpsetvar 'longwallgobs/mid_panel_center_radio_button (cx-show-toggle-button longwallgobs/mid_panel_center_radio_button))
			(rpsetvar 'longwallgobs/mid_panel_gateroad_radio_button  (cx-show-toggle-button longwallgobs/mid_panel_gateroad_radio_button ))
			(rpsetvar 'longwallgobs/working_face_center_radio_button (cx-show-toggle-button longwallgobs/working_face_center_radio_button))
			(rpsetvar 'longwallgobs/working_face_corner_radio_button (cx-show-toggle-button longwallgobs/working_face_corner_radio_button))
			(rpsetvar 'longwallgobs/single_part_mesh_radio_button (cx-show-toggle-button longwallgobs/single_part_mesh_radio_button))
			(rpsetvar 'longwallgobs/zone_names_selected (cx-show-list-selections longwallgobs/zone_names))
		
			(%run-udf-apply 1)
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
			;(if (pair? (cx-show-list-selections longwallgobs/zone_names)) (ti-menu-load-string (string-append "/define/boundary-conditions/fluid " (string-append (list-ref (cx-show-list-selections longwallgobs/zone_names) 0) " no no no no no 0 no 0 no 0 no 0 no 0 no 1 none no no no yes no no 1 no 0 no 0 no 0 no 1 no 0 yes yes yes \"udf\" \"set_perm_1_VSI::libudf\" yes yes udf \"set_perm_2_VSI::libudf\" yes yes \"udf\" \"set_perm_3_VSI::libudf\" no yes yes \"udf\" \"set_inertia_1_VSI::libudf\" yes yes \"udf\" \"set_inertia_2_VSI::libudf\" yes yes \"udf\" \"set_inertia_3_VSI::libudf\" 0 0 yes yes \"udf\" \"set_poro_VSI::libudf\" constant 1 no"))))
			;(if (equal? (cx-show-toggle-button longwallgobs/mine_C_radio_button) #t) (for-each (lambda (zone_name) (ti-menu-load-string (string-append "/define/boundary-conditions/fluid " (string-append zone_name " no no no no no 0 no 0 no 0 no 0 no 0 no 1 none no no no yes no no 1 no 0 no 0 no 0 no 1 no 0 yes yes yes \"udf\" \"set_1perm_1_VSI::libudf\" yes yes udf \"set_1perm_2_VSI::libudf\" yes yes \"udf\" \"set_1perm_3_VSI::libudf\" no yes yes \"udf\" \"set_inertia_1_VSI::libudf\" yes yes \"udf\" \"set_inertia_2_VSI::libudf\" yes yes \"udf\" \"set_inertia_3_VSI::libudf\" 0 0 yes yes \"udf\" \"set_1poro_VSI::libudf\" constant 1 no")))) (cx-show-list-selections longwallgobs/zone_names)))
			;(if (equal? (cx-show-toggle-button longwallgobs/mine_E_radio_button) #t) (for-each (lambda (zone_name) (ti-menu-load-string (string-append "/define/boundary-conditions/fluid " (string-append zone_name " no no no no no 0 no 0 no 0 no 0 no 0 no 1 none no no no yes no no 1 no 0 no 0 no 0 no 1 no 0 yes yes yes \"udf\" \"set_2perm_1_VSI::libudf\" yes yes udf \"set_2perm_2_VSI::libudf\" yes yes \"udf\" \"set_2perm_3_VSI::libudf\" no yes yes \"udf\" \"set_inertia_1_VSI::libudf\" yes yes \"udf\" \"set_inertia_2_VSI::libudf\" yes yes \"udf\" \"set_inertia_3_VSI::libudf\" 0 0 yes yes \"udf\" \"set_2poro_VSI::libudf\" constant 1 no")))) (cx-show-list-selections longwallgobs/zone_names)))
			;(if (equal? (cx-show-toggle-button longwallgobs/mine_T_radio_button) #t) (for-each (lambda (zone_name) (ti-menu-load-string (string-append "/define/boundary-conditions/fluid " (string-append zone_name " no no no no no 0 no 0 no 0 no 0 no 0 no 1 none no no no yes no no 1 no 0 no 0 no 0 no 1 no 0 yes yes yes \"udf\" \"set_3perm_1_VSI::libudf\" yes yes udf \"set_3perm_2_VSI::libudf\" yes yes \"udf\" \"set_3perm_3_VSI::libudf\" no yes yes \"udf\" \"set_inertia_1_VSI::libudf\" yes yes \"udf\" \"set_inertia_2_VSI::libudf\" yes yes \"udf\" \"set_inertia_3_VSI::libudf\" 0 0 yes yes \"udf\" \"set_3poro_VSI::libudf\" constant 1 no")))) (cx-show-list-selections longwallgobs/zone_names)))

		)

		(define (clear-button-cb . args)
			(if (equal? (cx-show-toggle-button longwallgobs/startup_room_center_radio_button) #t) (rpsetvar 'longwallgobs/startup_room_center_id 0))
			(if (equal? (cx-show-toggle-button longwallgobs/startup_room_corner_radio_button) #t) (rpsetvar 'longwallgobs/startup_room_corner_id 0)) 
			(if (equal? (cx-show-toggle-button longwallgobs/mid_panel_center_radio_button) #t) (rpsetvar 'longwallgobs/mid_panel_center_id 0)) 
			(if (equal? (cx-show-toggle-button longwallgobs/mid_panel_gateroad_radio_button) #t) (rpsetvar 'longwallgobs/mid_panel_gateroad_id 0)) 
			(if (equal? (cx-show-toggle-button longwallgobs/working_face_center_radio_button) #t) (rpsetvar 'longwallgobs/working_face_center_id 0)) 
			(if (equal? (cx-show-toggle-button longwallgobs/working_face_corner_radio_button) #t) (rpsetvar 'longwallgobs/working_face_corner_id 0)) 
			(if (equal? (cx-show-toggle-button longwallgobs/single_part_mesh_radio_button) #t) (rpsetvar 'longwallgobs/single_part_mesh_id -1)) 
		)

		(lambda args
			(if (not dialog-box)
				(let ()
					; Required Settings
					(set! dialog-box (cx-create-panel "Required Settings" apply-cb update-cb))
					(set! table (cx-create-table dialog-box "" 'row 0 'col 0))

					(set! longwallgobs/mine_button_box (cx-create-button-box table "Mine Model" 'radio-mode #t 'col 0))
					(set! longwallgobs/mine_C_radio_button (cx-create-toggle-button longwallgobs/mine_button_box "Mine C Model"))
					(set! longwallgobs/mine_E_radio_button (cx-create-toggle-button longwallgobs/mine_button_box "Mine E Model"))
					(set! longwallgobs/mine_T_radio_button (cx-create-toggle-button longwallgobs/mine_button_box "Mine T Model"))

					(set! longwallgobs/egz_button_box (cx-create-button-box table "" 'radio-mode #f 'row 1 'col 0))
					(set! longwallgobs/egz_radio_button (cx-create-toggle-button longwallgobs/egz_button_box "Explosive Gas Zone Colorization"))

					(set! longwallgobs/required_param_table (cx-create-table table "Required Settings" 'row 0 'col 1))
					(set! longwallgobs/panel_half_width (cx-create-real-entry longwallgobs/required_param_table "Panel Half Width" 'row 0))
					(set! longwallgobs/panel_length (cx-create-real-entry longwallgobs/required_param_table "Panel Length" 'row 1))
					(set! longwallgobs/panel_x_offset (cx-create-real-entry longwallgobs/required_param_table "Panel X Offset" 'row 2))
					(set! longwallgobs/panel_y_offset (cx-create-real-entry longwallgobs/required_param_table "Panel Y Offset" 'row 3))

					; Optional Settings
					(set! table2 (cx-create-table dialog-box "" 'below table 'right-of 0))

					(set! longwallgobs/optional_param_table (cx-create-table table2 "Optional Settings" 'row 0 'col 1))
					(set! longwallgobs/resist_scaler (cx-create-real-entry longwallgobs/optional_param_table "Resistance Scalar" 'row 0 'col 0))
					(set! longwallgobs/max_resistance (cx-create-real-entry longwallgobs/optional_param_table "Max Resistance" 'row 0 'col 1))
					(set! longwallgobs/min_resistance (cx-create-real-entry longwallgobs/optional_param_table "Min Resistance" 'row 0 'col 2))
					(set! longwallgobs/max_porosity (cx-create-real-entry longwallgobs/optional_param_table "Max Porosity" 'row 1 'col 0))
					(set! longwallgobs/initial_porosity (cx-create-real-entry longwallgobs/optional_param_table "Initial Porosity" 'row 1 'col 1))
					(set! longwallgobs/max_vsi (cx-create-real-entry longwallgobs/optional_param_table "Max VSI" 'row 1 'col 2))

					; Zone Selection
					(set! table3 (cx-create-table dialog-box "" 'below 0 'right-of table))

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
			(cx-show-panel dialog-box)
		) ;End Of Args Function
	) ;End Of Let Statement
) ;End Of model_type_and_required_settings_box Definition

(cx-add-menu "Model Mine Gob" #f)
(cx-add-hitem "Model Mine Gob" "Permeability and Porosity" #f #f #t #f)
;Menu Item Added To Above Created "New Menu->New Submenu" Submenu In Fluent
(cx-add-item "Permeability and Porosity" "Mine Model Settings" #\U #f #t model_type_and_required_settings_box)
