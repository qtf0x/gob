; allocate and initialize UDMs
(ti-menu-load-string "define/user-defined/user-defined-memory 7")
(ti-menu-load-string "solve/initialize/initialize-flow")

; compile and load UDF library
(ti-menu-load-string "define/user-defined/use-built-in-compiler yes")
(ti-menu-load-string "define/user-defined/compiled-functions compile libudf yes gob.c """)
(ti-menu-load-string "define/user-defined/compiled-functions load libudf")

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
(make-new-rpvar 'gob/mine_C_radio_button #t 'boolean)
(make-new-rpvar 'gob/mine_E_radio_button #f 'boolean)
(make-new-rpvar 'gob/mine_T_radio_button #f 'boolean)
(make-new-rpvar 'gob/panel-width 0 'real)
(make-new-rpvar 'gob/panel-length 0 'real)
(make-new-rpvar 'gob/panel-xoffset 0 'real)
(make-new-rpvar 'gob/panel-yoffset 0 'real)
(make-new-rpvar 'gob/egz_radio_button #f 'boolean)
; Declare variables for Optional Settings Box
(make-new-rpvar 'gob/resistance-scalar 10 'real)
(make-new-rpvar 'gob/maximum-resistance 5.0E6 'real)
(make-new-rpvar 'gob/minimum-resistance 1.45E5 'real)
(make-new-rpvar 'gob/maximum-porosity 0.40 'real)
(make-new-rpvar 'gob/initial-porosity 0.25778 'real)
(make-new-rpvar 'gob/maximum-vsi 0.40 'real)
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
(make-new-rpvar 'longwallgobs/single_part_mesh_id 0 'integer)
; Declare variables for zone info lists
(make-new-rpvar 'longwallgobs/zone_names_selected '() 'list)
(make-new-rpvar 'longwallgobs/zone_names '() 'list)
; (make-new-rpvar 'longwallgobs/zone_ids zone_ids 'list)

; Model Type and Required Settings Definition
(define model-type-and-required-settings-box
	;Let Statement, Local Variable Declarations
	(let ((dialog-box #f)
		; Required settings
		(table)
			(gob/mine_button_box)
			(gob/mine_C_radio_button)
			(gob/mine_E_radio_button)
			(gob/mine_T_radio_button)

			(gob/egz_button_box)
			(gob/egz_radio_button)

			(gob/required_param_table)
			(gob/panel-width)
			(gob/panel-length)
			(gob/panel-xoffset)
			(gob/panel-yoffset)

		; Optional settings
		(table2)
			(gob/optional_param_table)
			(gob/resistance-scalar)
			(gob/maximum-resistance)
			(gob/minimum-resistance)
			(gob/maximum-porosity)
			(gob/initial-porosity)
			(gob/maximum-vsi)

		; Zone selection
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

            (longwallgobs/zone_names)

		)

		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			; Required settings
			(cx-set-toggle-button gob/mine_C_radio_button (rpgetvar 'gob/mine_C_radio_button))
			(cx-set-toggle-button gob/mine_E_radio_button (rpgetvar 'gob/mine_E_radio_button))
			(cx-set-toggle-button gob/mine_T_radio_button (rpgetvar 'gob/mine_T_radio_button))

			(cx-set-toggle-button gob/egz_radio_button (rpgetvar 'gob/egz_radio_button))

			(cx-set-real-entry gob/panel-width (rpgetvar 'gob/panel-width))
			(cx-set-real-entry gob/panel-length (rpgetvar 'gob/panel-length))
			(cx-set-real-entry gob/panel-xoffset (rpgetvar 'gob/panel-xoffset))
			(cx-set-real-entry gob/panel-yoffset (rpgetvar 'gob/panel-yoffset))

			; Optional settings
			(cx-set-real-entry gob/resistance-scalar (rpgetvar 'gob/resistance-scalar))
			(cx-set-real-entry gob/maximum-resistance (rpgetvar 'gob/maximum-resistance))
			(cx-set-real-entry gob/minimum-resistance (rpgetvar 'gob/minimum-resistance))
			(cx-set-real-entry gob/maximum-porosity (rpgetvar 'gob/maximum-porosity))
			(cx-set-real-entry gob/initial-porosity (rpgetvar 'gob/initial-porosity))
			(cx-set-real-entry gob/maximum-vsi (rpgetvar 'gob/maximum-vsi))

			; Zone selection
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
			; Required settings
			(rpsetvar 'gob/mine_C_radio_button (cx-show-toggle-button gob/mine_C_radio_button))
			(rpsetvar 'gob/mine_E_radio_button (cx-show-toggle-button gob/mine_E_radio_button))
			(rpsetvar 'gob/mine_T_radio_button (cx-show-toggle-button gob/mine_T_radio_button))

			(rpsetvar 'gob/egz_radio_button (cx-show-toggle-button gob/egz_radio_button))

			(rpsetvar 'gob/panel-width (cx-show-real-entry gob/panel-width))
			(rpsetvar 'gob/panel-length (cx-show-real-entry gob/panel-length))
			(rpsetvar 'gob/panel-xoffset (cx-show-real-entry gob/panel-xoffset))
			(rpsetvar 'gob/panel-yoffset (cx-show-real-entry gob/panel-yoffset))

			; Optional settings
			(rpsetvar 'gob/resistance-scalar (cx-show-real-entry gob/resistance-scalar))
			(rpsetvar 'gob/maximum-resistance (cx-show-real-entry gob/maximum-resistance))
			(rpsetvar 'gob/minimum-resistance (cx-show-real-entry gob/minimum-resistance))
			(rpsetvar 'gob/maximum-porosity (cx-show-real-entry gob/maximum-porosity))
			(rpsetvar 'gob/initial-porosity (cx-show-real-entry gob/initial-porosity))
			(rpsetvar 'gob/maximum-vsi (cx-show-real-entry gob/maximum-vsi))

			; Zone selection
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

        (define (button-cb . args)
            ; Get zone id for each zone
			(if (equal? (cx-show-toggle-button longwallgobs/startup_room_center_radio_button) #t) (rpsetvar 'longwallgobs/startup_room_center_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0))))
			(if (equal? (cx-show-toggle-button longwallgobs/startup_room_corner_radio_button) #t) (rpsetvar 'longwallgobs/startup_room_corner_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			(if (equal? (cx-show-toggle-button longwallgobs/mid_panel_center_radio_button) #t) (rpsetvar 'longwallgobs/mid_panel_center_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			(if (equal? (cx-show-toggle-button longwallgobs/mid_panel_gateroad_radio_button) #t) (rpsetvar 'longwallgobs/mid_panel_gateroad_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			(if (equal? (cx-show-toggle-button longwallgobs/working_face_center_radio_button) #t) (rpsetvar 'longwallgobs/working_face_center_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			(if (equal? (cx-show-toggle-button longwallgobs/working_face_corner_radio_button) #t) (rpsetvar 'longwallgobs/working_face_corner_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 
			(if (equal? (cx-show-toggle-button longwallgobs/single_part_mesh_radio_button) #t) (rpsetvar 'longwallgobs/single_part_mesh_id (zone-name->id (list-ref (cx-show-list-selections longwallgobs/zone_names) 0)))) 

			; Set up zone selected based on what mine model is selected
			(if (equal? (rpgetvar 'gob/mine_C_radio_button) #t) (for-each (lambda (zone_name) (ti-menu-load-string (string-append "/define/boundary-conditions/fluid " (string-append zone_name " no no no no no 0 no 0 no 0 no 0 no 0 no 1 none no no no yes no no 1 no 0 no 0 no 0 no 1 no 0 yes yes yes \"udf\" \"set_1perm_1_VSI::libudf\" yes yes udf \"set_1perm_2_VSI::libudf\" yes yes \"udf\" \"set_1perm_3_VSI::libudf\" no yes yes \"udf\" \"set_inertia_1_VSI::libudf\" yes yes \"udf\" \"set_inertia_2_VSI::libudf\" yes yes \"udf\" \"set_inertia_3_VSI::libudf\" 0 0 yes yes \"udf\" \"set_1poro_VSI::libudf\" constant 1 no")))) (cx-show-list-selections longwallgobs/zone_names)))
			(if (equal? (rpgetvar 'gob/mine_E_radio_button) #t) (for-each (lambda (zone_name) (ti-menu-load-string (string-append "/define/boundary-conditions/fluid " (string-append zone_name " no no no no no 0 no 0 no 0 no 0 no 0 no 1 none no no no yes no no 1 no 0 no 0 no 0 no 1 no 0 yes yes yes \"udf\" \"set_2perm_1_VSI::libudf\" yes yes udf \"set_2perm_2_VSI::libudf\" yes yes \"udf\" \"set_2perm_3_VSI::libudf\" no yes yes \"udf\" \"set_inertia_1_VSI::libudf\" yes yes \"udf\" \"set_inertia_2_VSI::libudf\" yes yes \"udf\" \"set_inertia_3_VSI::libudf\" 0 0 yes yes \"udf\" \"set_2poro_VSI::libudf\" constant 1 no")))) (cx-show-list-selections longwallgobs/zone_names)))
			(if (equal? (rpgetvar 'gob/mine_T_radio_button) #t) (for-each (lambda (zone_name) (ti-menu-load-string (string-append "/define/boundary-conditions/fluid " (string-append zone_name " no no no no no 0 no 0 no 0 no 0 no 0 no 1 none no no no yes no no 1 no 0 no 0 no 0 no 1 no 0 yes yes yes \"udf\" \"set_3perm_1_VSI::libudf\" yes yes udf \"set_3perm_2_VSI::libudf\" yes yes \"udf\" \"set_3perm_3_VSI::libudf\" no yes yes \"udf\" \"set_inertia_1_VSI::libudf\" yes yes \"udf\" \"set_inertia_2_VSI::libudf\" yes yes \"udf\" \"set_inertia_3_VSI::libudf\" 0 0 yes yes \"udf\" \"set_3poro_VSI::libudf\" constant 1 no")))) (cx-show-list-selections longwallgobs/zone_names)))
		)

		(lambda args
			(if (not dialog-box)
				(let ()
					; Required settings
					(set! dialog-box (cx-create-panel "Required Settings" apply-cb update-cb))
					(set! table (cx-create-table dialog-box "" 'row 0 'col 0))

					(set! gob/mine_button_box (cx-create-button-box table "Mine Model" 'radio-mode #t 'col 0))
					(set! gob/mine_C_radio_button (cx-create-toggle-button gob/mine_button_box "Mine C Model"))
					(set! gob/mine_E_radio_button (cx-create-toggle-button gob/mine_button_box "Mine E Model"))
					(set! gob/mine_T_radio_button (cx-create-toggle-button gob/mine_button_box "Mine T Model"))

					(set! gob/egz_button_box (cx-create-button-box table "" 'radio-mode #f 'row 1 'col 0))
					(set! gob/egz_radio_button (cx-create-toggle-button gob/egz_button_box "Explosive Gas Zone Colorization"))

					(set! gob/required_param_table (cx-create-table table "Required Settings" 'row 0 'col 1))
					(set! gob/panel-width (cx-create-real-entry gob/required_param_table "Panel Width" 'row 0))
					(set! gob/panel-length (cx-create-real-entry gob/required_param_table "Panel Length" 'row 1))
					(set! gob/panel-xoffset (cx-create-real-entry gob/required_param_table "Panel X Offset" 'row 2))
					(set! gob/panel-yoffset (cx-create-real-entry gob/required_param_table "Panel Y Offset" 'row 3))

					; Optional settings
					(set! table2 (cx-create-table dialog-box "" 'below table 'right-of 0))

					(set! gob/optional_param_table (cx-create-table table2 "Optional Settings" 'row 0 'col 1))
					(set! gob/resistance-scalar (cx-create-real-entry gob/optional_param_table "Resistance Scalar" 'row 0 'col 0))
					(set! gob/maximum-resistance (cx-create-real-entry gob/optional_param_table "Maximum Resistance" 'row 0 'col 1))
					(set! gob/minimum-resistance (cx-create-real-entry gob/optional_param_table "Minimum Resistance" 'row 0 'col 2))
					(set! gob/maximum-porosity (cx-create-real-entry gob/optional_param_table "Maximum Porosity" 'row 1 'col 0))
					(set! gob/initial-porosity (cx-create-real-entry gob/optional_param_table "Initial Porosity" 'row 1 'col 1))
					(set! gob/maximum-vsi (cx-create-real-entry gob/optional_param_table "Maximum VSI" 'row 1 'col 2))

					; Zone selection
					(set! table3 (cx-create-table dialog-box "" 'below 0 'right-of table))

					(set! longwallgobs/zone_button_box (cx-create-button-box table3 "Zone Type" 'radio-mode #t 'col 0))

					(set! longwallgobs/startup_room_center_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Startup Center"))
					(set! longwallgobs/startup_room_corner_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Startup Headgate"))
					(set! longwallgobs/mid_panel_center_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Mid-Panel Headgate"))
					(set! longwallgobs/mid_panel_gateroad_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Mid-Panel Tailgate"))
					(set! longwallgobs/working_face_center_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Working Center"))
					(set! longwallgobs/working_face_corner_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Working Headgate"))
					(set! longwallgobs/single_part_mesh_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Single-Part Mesh"))

					(set! longwallgobs/apply_button (cx-create-button table3 "Apply" 'activate-callback button-cb 'row 1 'col 1))

					(set! longwallgobs/zone_names (cx-create-list table3 "Zone Selection" 'visible-lines 9 'multiple-selections #f 'row 0 'col 1))
				
				) ;End Of Let Statement
			) ;End Of If Statement
			
			;Call To Open Dialog Box
			(cx-show-panel dialog-box)
		) ;End Of Args Function
	) ;End Of Let Statement
) ;End Of model-type-and-required-settings-box Definition

(cx-add-menu "Model Mine Gob" #f)
(cx-add-hitem "Model Mine Gob" "Permeability and Porosity" #f #f #t #f)
;Menu Item Added To Above Created "New Menu->New Submenu" Submenu In Fluent
(cx-add-item "Permeability and Porosity" "Mine Model and Required Settings" #\U #f #t model-type-and-required-settings-box)
(cx-add-item "Permeability and Porosity" "Optional Settings" #\U #f #t optional-settings-box)
(cx-add-item "Permeability and Porosity" "Zone Selection" #\U #f #t zone-selection-box)
