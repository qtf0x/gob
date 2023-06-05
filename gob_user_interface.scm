; allocate and initialize UDMs
(ti-menu-load-string "define/user-defined/user-defined-memory 7")
(ti-menu-load-string "solve/initialize/initialize-flow")

; compile and load UDF library
(ti-menu-load-string "define/user-defined/use-built-in-compiler yes")
(ti-menu-load-string "define/user-defined/compiled-functions compile libudf yes longwallgobs.c """)
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
					"longwallgobs"
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
(make-new-rpvar 'longwallgobs/panel-width 0 'real)
(make-new-rpvar 'longwallgobs/panel-length 0 'real)
(make-new-rpvar 'longwallgobs/panel-xoffset 0 'real)
(make-new-rpvar 'longwallgobs/panel-yoffset 0 'real)
(make-new-rpvar 'longwallgobs/egz_radio_button #f 'boolean)
; Declare variables for Optional Settings Box
(make-new-rpvar 'longwallgobs/resistance-scalar 10 'real)
(make-new-rpvar 'longwallgobs/maximum-resistance 5.0E6 'real)
(make-new-rpvar 'longwallgobs/minimum-resistance 1.45E5 'real)
(make-new-rpvar 'longwallgobs/maximum-porosity 0.40 'real)
(make-new-rpvar 'longwallgobs/initial-porosity 0.25778 'real)
(make-new-rpvar 'longwallgobs/maximum-vsi 0.40 'real)
; Declare Variables for Zone Selection box
(make-new-rpvar 'longwallgobs/startup_center_radio_button #t 'boolean)
(make-new-rpvar 'longwallgobs/startup_headgate_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/startup_tailgate_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/gob_center_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/mid_headgate_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/mid_tailgate_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/working_center_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/working_headgate_radio_button #f 'boolean)
(make-new-rpvar 'longwallgobs/working_tailgate_radio_button #f 'boolean)
; Declare variables for zone info lists
(make-new-rpvar 'longwallgobs/zone_names_selected '() 'list)
; (make-new-rpvar 'longwallgobs/zone_ids zone_ids 'list)

; Model Type and Required Settings Definition
(define model-type-and-required-settings-box
	;Let Statement, Local Variable Declarations
	(let ((dialog-box #f)
		(table)
			(longwallgobs/mine_button_box)
			(longwallgobs/mine_C_radio_button)
			(longwallgobs/mine_E_radio_button)
			(longwallgobs/mine_T_radio_button)

			(longwallgobs/egz_button_box)
			(longwallgobs/egz_radio_button)

			(longwallgobs/required_param_table)
			(longwallgobs/panel-width)
			(longwallgobs/panel-length)
			(longwallgobs/panel-xoffset)
			(longwallgobs/panel-yoffset)
		)

		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			(cx-set-toggle-button longwallgobs/mine_C_radio_button (rpgetvar 'longwallgobs/mine_C_radio_button))
			(cx-set-toggle-button longwallgobs/mine_E_radio_button (rpgetvar 'longwallgobs/mine_E_radio_button))
			(cx-set-toggle-button longwallgobs/mine_T_radio_button (rpgetvar 'longwallgobs/mine_T_radio_button))

			(cx-set-toggle-button longwallgobs/egz_radio_button (rpgetvar 'longwallgobs/egz_radio_button))

			(cx-set-real-entry longwallgobs/panel-width (rpgetvar 'longwallgobs/panel-width))
			(cx-set-real-entry longwallgobs/panel-length (rpgetvar 'longwallgobs/panel-length))
			(cx-set-real-entry longwallgobs/panel-xoffset (rpgetvar 'longwallgobs/panel-xoffset))
			(cx-set-real-entry longwallgobs/panel-yoffset (rpgetvar 'longwallgobs/panel-yoffset))
		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)
			(rpsetvar 'longwallgobs/mine_C_radio_button (cx-show-toggle-button longwallgobs/mine_C_radio_button))
			(rpsetvar 'longwallgobs/mine_E_radio_button (cx-show-toggle-button longwallgobs/mine_E_radio_button))
			(rpsetvar 'longwallgobs/mine_T_radio_button (cx-show-toggle-button longwallgobs/mine_T_radio_button))

			(rpsetvar 'longwallgobs/egz_radio_button (cx-show-toggle-button longwallgobs/egz_radio_button))

			(rpsetvar 'longwallgobs/panel-width (cx-show-real-entry longwallgobs/panel-width))
			(rpsetvar 'longwallgobs/panel-length (cx-show-real-entry longwallgobs/panel-length))
			(rpsetvar 'longwallgobs/panel-xoffset (cx-show-real-entry longwallgobs/panel-xoffset))
			(rpsetvar 'longwallgobs/panel-yoffset (cx-show-real-entry longwallgobs/panel-yoffset))
		
			(%run-udf-apply 1)
		)

		(lambda args
			(if (not dialog-box)
				(let ()
				
					(set! dialog-box (cx-create-panel "Required Settings" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))

					(set! longwallgobs/mine_button_box (cx-create-button-box table "Mine Model" 'radio-mode #t 'col 0))
					(set! longwallgobs/mine_C_radio_button (cx-create-toggle-button longwallgobs/mine_button_box "Mine C Model"))
					(set! longwallgobs/mine_E_radio_button (cx-create-toggle-button longwallgobs/mine_button_box "Mine E Model"))
					(set! longwallgobs/mine_T_radio_button (cx-create-toggle-button longwallgobs/mine_button_box "Mine T Model"))

					(set! longwallgobs/egz_button_box (cx-create-button-box table "" 'radio-mode #f 'row 1 'col 0))
					(set! longwallgobs/egz_radio_button (cx-create-toggle-button longwallgobs/egz_button_box "Explosive Gas Zone Colorization"))

					(set! longwallgobs/required_param_table (cx-create-table table "Required Settings" 'row 0 'col 1))
					(set! longwallgobs/panel-width (cx-create-real-entry longwallgobs/required_param_table "Panel Width" 'row 0))
					(set! longwallgobs/panel-length (cx-create-real-entry longwallgobs/required_param_table "Panel Length" 'row 1))
					(set! longwallgobs/panel-xoffset (cx-create-real-entry longwallgobs/required_param_table "Panel X Offset" 'row 2))
					(set! longwallgobs/panel-yoffset (cx-create-real-entry longwallgobs/required_param_table "Panel Y Offset" 'row 3))
				
				) ;End Of Let Statement
			) ;End Of If Statement
			
			;Call To Open Dialog Box
			(cx-show-panel dialog-box)
		) ;End Of Args Function
	) ;End Of Let Statement
) ;End Of model-type-and-required-settings-box Definition

; Optional Settings Definition
(define optional-settings-box
	;Let Statement, Local Variable Declarations
	(let ((dialog-box #f)
		(table)

			(longwallgobs/optional_param_table)
			(longwallgobs/resistance-scalar)
			(longwallgobs/maximum-resistance)
			(longwallgobs/minimum-resistance)
			(longwallgobs/maximum-porosity)
			(longwallgobs/initial-porosity)
			(longwallgobs/maximum-vsi)

		)

		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)

			(cx-set-real-entry longwallgobs/resistance-scalar (rpgetvar 'longwallgobs/resistance-scalar))
			(cx-set-real-entry longwallgobs/maximum-resistance (rpgetvar 'longwallgobs/maximum-resistance))
			(cx-set-real-entry longwallgobs/minimum-resistance (rpgetvar 'longwallgobs/minimum-resistance))
			(cx-set-real-entry longwallgobs/maximum-porosity (rpgetvar 'longwallgobs/maximum-porosity))
			(cx-set-real-entry longwallgobs/initial-porosity (rpgetvar 'longwallgobs/initial-porosity))
			(cx-set-real-entry longwallgobs/maximum-vsi (rpgetvar 'longwallgobs/maximum-vsi))

		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)

			(rpsetvar 'longwallgobs/resistance-scalar (cx-show-real-entry longwallgobs/resistance-scalar))
			(rpsetvar 'longwallgobs/maximum-resistance (cx-show-real-entry longwallgobs/maximum-resistance))
			(rpsetvar 'longwallgobs/minimum-resistance (cx-show-real-entry longwallgobs/minimum-resistance))
			(rpsetvar 'longwallgobs/maximum-porosity (cx-show-real-entry longwallgobs/maximum-porosity))
			(rpsetvar 'longwallgobs/initial-porosity (cx-show-real-entry longwallgobs/initial-porosity))
			(rpsetvar 'longwallgobs/maximum-vsi (cx-show-real-entry longwallgobs/maximum-vsi))

			(%run-udf-apply 1)

		)

		(lambda args
			(if (not dialog-box)
				(let ()
				
					(set! dialog-box (cx-create-panel "Optional Settings" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))

					(set! longwallgobs/optional_param_table (cx-create-table table "Optional Settings" 'row 0 'col 1))
					(set! longwallgobs/resistance-scalar (cx-create-real-entry longwallgobs/optional_param_table "Resistance Scalar" 'row 0))
					(set! longwallgobs/maximum-resistance (cx-create-real-entry longwallgobs/optional_param_table "Maximum Resistance" 'row 1))
					(set! longwallgobs/minimum-resistance (cx-create-real-entry longwallgobs/optional_param_table "Minimum Resistance" 'row 2))
					(set! longwallgobs/maximum-porosity (cx-create-real-entry longwallgobs/optional_param_table "Maximum Porosity" 'row 3))
					(set! longwallgobs/initial-porosity (cx-create-real-entry longwallgobs/optional_param_table "Initial Porosity" 'row 4))
					(set! longwallgobs/maximum-vsi (cx-create-real-entry longwallgobs/optional_param_table "Maximum VSI" 'row 5))
				
				) ;End Of Let Statement
			) ;End Of If Statement
			
			;Call To Open Dialog Box
			(cx-show-panel dialog-box)
		) ;End Of Args Function
	) ;End Of Let Statement
) ;End Of optional-settings-box Definition

; Zone Selection Definition
(define zone-selection-box
	; Let Statement, Local Varaible Declarations
	(let ((dialog-box #f)
		(table)
			(longwallgobs/zone_button_box)
			(longwallgobs/startup_center_radio_button)	
			(longwallgobs/startup_headgate_radio_button)
			(longwallgobs/startup_tailgate_radio_button)	
			(longwallgobs/gob_center_radio_button)	
			(longwallgobs/mid_headgate_radio_button)	
			(longwallgobs/mid_tailgate_radio_button)	
			(longwallgobs/working_center_radio_button)	
			(longwallgobs/working_headgate_radio_button)	
			(longwallgobs/working_tailgate_radio_button)
			(longwallgobs/zone_names)
	)

		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			(cx-set-toggle-button longwallgobs/startup_center_radio_button (rpgetvar 'longwallgobs/startup_center_radio_button))
			(cx-set-toggle-button longwallgobs/startup_headgate_radio_button (rpgetvar 'longwallgobs/startup_headgate_radio_button))
			(cx-set-toggle-button longwallgobs/startup_tailgate_radio_button (rpgetvar 'longwallgobs/startup_tailgate_radio_button))
			(cx-set-toggle-button longwallgobs/gob_center_radio_button (rpgetvar 'longwallgobs/gob_center_radio_button))
			(cx-set-toggle-button longwallgobs/mid_headgate_radio_button (rpgetvar 'longwallgobs/mid_headgate_radio_button))
			(cx-set-toggle-button longwallgobs/mid_tailgate_radio_button (rpgetvar 'longwallgobs/mid_tailgate_radio_button))
			(cx-set-toggle-button longwallgobs/working_center_radio_button (rpgetvar 'longwallgobs/working_center_radio_button))
			(cx-set-toggle-button longwallgobs/working_headgate_radio_button (rpgetvar 'longwallgobs/working_headgate_radio_button))
			(cx-set-toggle-button longwallgobs/working_tailgate_radio_button (rpgetvar 'longwallgobs/working_tailgate_radio_button))
			(cx-set-list-items longwallgobs/zone_names zone_names)
		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)
			(rpsetvar 'longwallgobs/startup_center_radio_button (cx-show-toggle-button longwallgobs/startup_center_radio_button))
			(rpsetvar 'longwallgobs/startup_headgate_radio_button (cx-show-toggle-button longwallgobs/startup_headgate_radio_button))
			(rpsetvar 'longwallgobs/startup_tailgate_radio_button (cx-show-toggle-button longwallgobs/startup_tailgate_radio_button))
			(rpsetvar 'longwallgobs/gob_center_radio_button (cx-show-toggle-button longwallgobs/gob_center_radio_button))
			(rpsetvar 'longwallgobs/mid_headgate_radio_button (cx-show-toggle-button longwallgobs/mid_headgate_radio_button))
			(rpsetvar 'longwallgobs/mid_tailgate_radio_button  (cx-show-toggle-button longwallgobs/mid_tailgate_radio_button ))
			(rpsetvar 'longwallgobs/working_center_radio_button (cx-show-toggle-button longwallgobs/working_center_radio_button))
			(rpsetvar 'longwallgobs/working_headgate_radio_button (cx-show-toggle-button longwallgobs/working_headgate_radio_button))
			(rpsetvar 'longwallgobs/working_tailgate_radio_button (cx-show-toggle-button longwallgobs/working_tailgate_radio_button))
			(rpsetvar 'longwallgobs/zone_names_selected (cx-show-list-selections longwallgobs/zone_names))
		
			(%run-udf-apply 1)
		)

		(lambda args
			(if (not dialog-box)
				(let ()
				
					(set! dialog-box (cx-create-panel "Zone Selection" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))

					(set! longwallgobs/zone_button_box (cx-create-button-box table "Zone Type" 'radio-mode #t 'col 0))

					(set! longwallgobs/startup_center_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Startup Center"))
					(set! longwallgobs/startup_headgate_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Startup Headgate"))
					(set! longwallgobs/startup_tailgate_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Startup Tailgate"))
					(set! longwallgobs/gob_center_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Gob Center"))
					(set! longwallgobs/mid_headgate_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Mid-Panel Headgate"))
					(set! longwallgobs/mid_tailgate_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Mid-Panel Tailgate"))
					(set! longwallgobs/working_center_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Working Center"))
					(set! longwallgobs/working_headgate_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Working Headgate"))
					(set! longwallgobs/working_tailgate_radio_button (cx-create-toggle-button longwallgobs/zone_button_box "Working Tailgate"))

					(set! longwallgobs/zone_names (cx-create-list table "Zone Selection" 'visible-lines 9 'multiple-selections #f 'row 0 'col 1))

				) ;End Of Let Statement
			) ;End Of If Statement
			;Call To Open Dialog Box
			(cx-show-panel dialog-box)
		) ;End Of Args Function
	) ;End Of Let Statement
) ; End of Zone Selection Definition

(cx-add-menu "Model Mine Gob" #f)
(cx-add-hitem "Model Mine Gob" "Permeability and Porosity" #f #f #t #f)
;Menu Item Added To Above Created "New Menu->New Submenu" Submenu In Fluent
(cx-add-item "Permeability and Porosity" "Mine Model and Required Settings" #\U #f #t model-type-and-required-settings-box)
(cx-add-item "Permeability and Porosity" "Optional Settings" #\U #f #t optional-settings-box)
(cx-add-item "Permeability and Porosity" "Zone Selection" #\U #f #t zone-selection-box)
