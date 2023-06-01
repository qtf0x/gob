; allocate and initialize UDMs
(ti-menu-load-string "define/user-defined/user-defined-memory 7")
(ti-menu-load-string "solve/initialize/initialize-flow")

; compile and load UDF library
(ti-menu-load-string "define/user-defined/use-built-in-compiler yes")
(ti-menu-load-string "define/user-defined/compiled-functions compile libudf yes gob.c """)
(ti-menu-load-string "define/user-defined/compiled-functions load libudf")

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
(make-new-rpvar 'gob/startup_center_radio_button #t 'boolean)
(make-new-rpvar 'gob/startup_headgate_radio_button #f 'boolean)
(make-new-rpvar 'gob/startup_tailgate_radio_button #f 'boolean)
(make-new-rpvar 'gob/gob_center_radio_button #f 'boolean)
(make-new-rpvar 'gob/mid_headgate_radio_button #f 'boolean)
(make-new-rpvar 'gob/mid_tailgate_radio_button #f 'boolean)
(make-new-rpvar 'gob/working_center_radio_button #f 'boolean)
(make-new-rpvar 'gob/working_headgate_radio_button #f 'boolean)
(make-new-rpvar 'gob/working_tailgate_radio_button #f 'boolean)

; Model Type and Required Settings Definition
(define model-type-and-required-settings-box
	;Let Statement, Local Variable Declarations
	(let ((dialog-box #f)
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
		)

		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			(cx-set-toggle-button gob/mine_C_radio_button (rpgetvar 'gob/mine_C_radio_button))
			(cx-set-toggle-button gob/mine_E_radio_button (rpgetvar 'gob/mine_E_radio_button))
			(cx-set-toggle-button gob/mine_T_radio_button (rpgetvar 'gob/mine_T_radio_button))

			(cx-set-toggle-button gob/egz_radio_button (rpgetvar 'gob/egz_radio_button))

			(cx-set-real-entry gob/panel-width (rpgetvar 'gob/panel-width))
			(cx-set-real-entry gob/panel-length (rpgetvar 'gob/panel-length))
			(cx-set-real-entry gob/panel-xoffset (rpgetvar 'gob/panel-xoffset))
			(cx-set-real-entry gob/panel-yoffset (rpgetvar 'gob/panel-yoffset))
		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)
			(rpsetvar 'gob/mine_C_radio_button (cx-show-toggle-button gob/mine_C_radio_button))
			(rpsetvar 'gob/mine_E_radio_button (cx-show-toggle-button gob/mine_E_radio_button))
			(rpsetvar 'gob/mine_T_radio_button (cx-show-toggle-button gob/mine_T_radio_button))

			(rpsetvar 'gob/egz_radio_button (cx-show-toggle-button gob/egz_radio_button))

			(rpsetvar 'gob/panel-width (cx-show-real-entry gob/panel-width))
			(rpsetvar 'gob/panel-length (cx-show-real-entry gob/panel-length))
			(rpsetvar 'gob/panel-xoffset (cx-show-real-entry gob/panel-xoffset))
			(rpsetvar 'gob/panel-yoffset (cx-show-real-entry gob/panel-yoffset))
		
			(%run-udf-apply 1)
		)

		(lambda args
			(if (not dialog-box)
				(let ()
				
					(set! dialog-box (cx-create-panel "Required Settings" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))

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

			(gob/optional_param_table)
			(gob/resistance-scalar)
			(gob/maximum-resistance)
			(gob/minimum-resistance)
			(gob/maximum-porosity)
			(gob/initial-porosity)
			(gob/maximum-vsi)

		)

		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)

			(cx-set-real-entry gob/resistance-scalar (rpgetvar 'gob/resistance-scalar))
			(cx-set-real-entry gob/maximum-resistance (rpgetvar 'gob/maximum-resistance))
			(cx-set-real-entry gob/minimum-resistance (rpgetvar 'gob/minimum-resistance))
			(cx-set-real-entry gob/maximum-porosity (rpgetvar 'gob/maximum-porosity))
			(cx-set-real-entry gob/initial-porosity (rpgetvar 'gob/initial-porosity))
			(cx-set-real-entry gob/maximum-vsi (rpgetvar 'gob/maximum-vsi))

		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)

			(rpsetvar 'gob/resistance-scalar (cx-show-real-entry gob/resistance-scalar))
			(rpsetvar 'gob/maximum-resistance (cx-show-real-entry gob/maximum-resistance))
			(rpsetvar 'gob/minimum-resistance (cx-show-real-entry gob/minimum-resistance))
			(rpsetvar 'gob/maximum-porosity (cx-show-real-entry gob/maximum-porosity))
			(rpsetvar 'gob/initial-porosity (cx-show-real-entry gob/initial-porosity))
			(rpsetvar 'gob/maximum-vsi (cx-show-real-entry gob/maximum-vsi))

			(%run-udf-apply 1)

		)

		(lambda args
			(if (not dialog-box)
				(let ()
				
					(set! dialog-box (cx-create-panel "Optional Settings" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))

					(set! gob/optional_param_table (cx-create-table table "Optional Settings" 'row 0 'col 1))
					(set! gob/resistance-scalar (cx-create-real-entry gob/optional_param_table "Resistance Scalar" 'row 0))
					(set! gob/maximum-resistance (cx-create-real-entry gob/optional_param_table "Maximum Resistance" 'row 1))
					(set! gob/minimum-resistance (cx-create-real-entry gob/optional_param_table "Minimum Resistance" 'row 2))
					(set! gob/maximum-porosity (cx-create-real-entry gob/optional_param_table "Maximum Porosity" 'row 3))
					(set! gob/initial-porosity (cx-create-real-entry gob/optional_param_table "Initial Porosity" 'row 4))
					(set! gob/maximum-vsi (cx-create-real-entry gob/optional_param_table "Maximum VSI" 'row 5))
				
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
			(gob/zone_button_box)
			(gob/startup_center_radio_button)	
			(gob/startup_headgate_radio_button)
			(gob/startup_tailgate_radio_button)	
			(gob/gob_center_radio_button)	
			(gob/mid_headgate_radio_button)	
			(gob/mid_tailgate_radio_button)	
			(gob/working_center_radio_button)	
			(gob/working_headgate_radio_button)	
			(gob/working_tailgate_radio_button)		
	)

		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			(cx-set-toggle-button gob/startup_center_radio_button (rpgetvar 'gob/startup_center_radio_button))
			(cx-set-toggle-button gob/startup_headgate_radio_button (rpgetvar 'gob/startup_headgate_radio_button))
			(cx-set-toggle-button gob/startup_tailgate_radio_button (rpgetvar 'gob/startup_tailgate_radio_button))
			(cx-set-toggle-button gob/gob_center_radio_button (rpgetvar 'gob/gob_center_radio_button))
			(cx-set-toggle-button gob/mid_headgate_radio_button (rpgetvar 'gob/mid_headgate_radio_button))
			(cx-set-toggle-button gob/mid_tailgate_radio_button (rpgetvar 'gob/mid_tailgate_radio_button))
			(cx-set-toggle-button gob/working_center_radio_button (rpgetvar 'gob/working_center_radio_button))
			(cx-set-toggle-button gob/working_headgate_radio_button (rpgetvar 'gob/working_headgate_radio_button))
			(cx-set-toggle-button gob/working_tailgate_radio_button (rpgetvar 'gob/working_tailgate_radio_button))
		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)
			(rpsetvar 'gob/startup_center_radio_button (cx-show-toggle-button gob/startup_center_radio_button))
			(rpsetvar 'gob/startup_headgate_radio_button (cx-show-toggle-button gob/startup_headgate_radio_button))
			(rpsetvar 'gob/startup_tailgate_radio_button (cx-show-toggle-button gob/startup_tailgate_radio_button))
			(rpsetvar 'gob/gob_center_radio_button (cx-show-toggle-button gob/gob_center_radio_button))
			(rpsetvar 'gob/mid_headgate_radio_button (cx-show-toggle-button gob/mid_headgate_radio_button))
			(rpsetvar 'gob/mid_tailgate_radio_button  (cx-show-toggle-button gob/mid_tailgate_radio_button ))
			(rpsetvar 'gob/working_center_radio_button (cx-show-toggle-button gob/working_center_radio_button))
			(rpsetvar 'gob/working_headgate_radio_button (cx-show-toggle-button gob/working_headgate_radio_button))
			(rpsetvar 'gob/working_tailgate_radio_button (cx-show-toggle-button gob/working_tailgate_radio_button))
		
			(%run-udf-apply 1)
		)

		(lambda args
			(if (not dialog-box)
				(let ()
				
					(set! dialog-box (cx-create-panel "Zone Selection" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))

					(set! gob/zone_button_box (cx-create-button-box table "Zone Type" 'radio-mode #t 'col 0))

					(set! gob/startup_center_radio_button (cx-create-toggle-button gob/zone_button_box "Startup Center"))
					(set! gob/startup_headgate_radio_button (cx-create-toggle-button gob/zone_button_box "Startup Headgate"))
					(set! gob/startup_tailgate_radio_button (cx-create-toggle-button gob/zone_button_box "Startup Tailgate"))
					(set! gob/gob_center_radio_button (cx-create-toggle-button gob/zone_button_box "Gob Center"))
					(set! gob/mid_headgate_radio_button (cx-create-toggle-button gob/zone_button_box "Mid-Panel Headgate"))
					(set! gob/mid_tailgate_radio_button (cx-create-toggle-button gob/zone_button_box "Mid-Panel Tailgate"))
					(set! gob/working_center_radio_button (cx-create-toggle-button gob/zone_button_box "Working Center"))
					(set! gob/working_headgate_radio_button (cx-create-toggle-button gob/zone_button_box "Working Headgate"))
					(set! gob/working_tailgate_radio_button (cx-create-toggle-button gob/zone_button_box "Working Tailgate"))

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
