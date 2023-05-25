;RP Variable Create Function
(define (make-new-rpvar name default type)
	(if (not (rp-var-object name))
		(rp-var-define name default type #f)))

;RP variable declarations
(make-new-rpvar 'gob/mine_C_radio_button #t 'boolean)
(make-new-rpvar 'gob/mine_E_radio_button #f 'boolean)
(make-new-rpvar 'gob/mine_T_radio_button #f 'boolean)
(make-new-rpvar 'gob/egz_radio_button #f 'boolean)
(make-new-rpvar 'gob/min_vsi_input 0 'real)
(make-new-rpvar 'gob/max_vsi_input 0 'real)
(make-new-rpvar 'gob/init_poro_input 0 'real)
(make-new-rpvar 'gob/resist_scalar_input 0 'real)

(make-new-rpvar 'gob/startup_center_radio_button #t 'boolean)
(make-new-rpvar 'gob/startup_headgate_radio_button #f 'boolean)
(make-new-rpvar 'gob/startup_tailgate_radio_button #f 'boolean)
(make-new-rpvar 'gob/gob_center_radio_button #f 'boolean)
(make-new-rpvar 'gob/mid_headgate_radio_button #f 'boolean)
(make-new-rpvar 'gob/mid_tailgate_radio_button #f 'boolean)
(make-new-rpvar 'gob/working_center_radio_button #f 'boolean)
(make-new-rpvar 'gob/working_headgate_radio_button #f 'boolean)
(make-new-rpvar 'gob/working_tailgate_radio_button #f 'boolean)

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

;Dialog Box Definition
(define gui-dialog-box
	;Let Statement, Local Variable Declarations
	(let ((dialog-box #f)
		(table)
			(gob/mine_button_box)
			(gob/mine_C_radio_button)
			(gob/mine_E_radio_button)
			(gob/mine_T_radio_button)

			(gob/egz_button_box)
			(gob/egz_radio_button)

			(gob/param_table)
			(gob/min_vsi_input)
			(gob/max_vsi_input)
			(gob/init_poro_input)
			(gob/resist_scalar_input)
		)

		; update-cb - invoked when the dialog box is opened
		(define (update-cb . args)
			(cx-set-toggle-button gob/mine_C_radio_button (rpgetvar 'gob/mine_C_radio_button))
			(cx-set-toggle-button gob/mine_E_radio_button (rpgetvar 'gob/mine_E_radio_button))
			(cx-set-toggle-button gob/mine_T_radio_button (rpgetvar 'gob/mine_T_radio_button))

			(cx-set-toggle-button gob/egz_radio_button (rpgetvar 'gob/egz_radio_button))

			(cx-set-real-entry gob/min_vsi_input (rpgetvar 'gob/min_vsi_input))
			(cx-set-real-entry gob/max_vsi_input (rpgetvar 'gob/max_vsi_input))
			(cx-set-real-entry gob/init_poro_input (rpgetvar 'gob/init_poro_input))
			(cx-set-real-entry gob/resist_scalar_input (rpgetvar 'gob/resist_scalar_input))
		)

		; apply-cb - invoked when you click "OK"
		(define (apply-cb . args)
			(rpsetvar 'gob/mine_C_radio_button (cx-show-toggle-button gob/mine_C_radio_button))
			(rpsetvar 'gob/mine_E_radio_button (cx-show-toggle-button gob/mine_E_radio_button))
			(rpsetvar 'gob/mine_T_radio_button (cx-show-toggle-button gob/mine_T_radio_button))

			(rpsetvar 'gob/egz_radio_button (cx-show-toggle-button gob/egz_radio_button))

			(rpsetvar 'gob/min_vsi_input (cx-show-real-entry gob/min_vsi_input))
			(rpsetvar 'gob/max_vsi_input (cx-show-real-entry gob/max_vsi_input))
			(rpsetvar 'gob/init_poro_input (cx-show-real-entry gob/init_poro_input))
			(rpsetvar 'gob/resist_scalar_input (cx-show-real-entry gob/resist_scalar_input))
		
			(%run-udf-apply 1)
		)

		(lambda args
			(if (not dialog-box)
				(let ()
				
					(set! dialog-box (cx-create-panel "Mine Model and Parameters" apply-cb update-cb))
					(set! table (cx-create-table dialog-box ""))

					(set! gob/mine_button_box (cx-create-button-box table "Mine Model" 'radio-mode #t 'col 0))
					(set! gob/mine_C_radio_button (cx-create-toggle-button gob/mine_button_box "Mine C Model"))
					(set! gob/mine_E_radio_button (cx-create-toggle-button gob/mine_button_box "Mine E Model"))
					(set! gob/mine_T_radio_button (cx-create-toggle-button gob/mine_button_box "Mine T Model"))

					(set! gob/egz_button_box (cx-create-button-box table "" 'radio-mode #f 'row 1 'col 0))
					(set! gob/egz_radio_button (cx-create-toggle-button gob/egz_button_box "Explosive Gas Zone Colorization"))

					(set! gob/param_table (cx-create-table table "Gob Parameters and Limits" 'row 0 'col 1))
					(set! gob/min_vsi_input (cx-create-real-entry gob/param_table "Min VSI" 'row 0))
					(set! gob/max_vsi_input (cx-create-real-entry gob/param_table "Max VSI" 'row 1))
					(set! gob/init_poro_input (cx-create-real-entry gob/param_table "Initial Porosity of Host Rock" 'row 2))
					(set! gob/resist_scalar_input (cx-create-real-entry gob/param_table "Resistance Scalar" 'row 3))
				
				) ;End Of Let Statement
			) ;End Of If Statement
			
			;Call To Open Dialog Box
			(cx-show-panel dialog-box)
		) ;End Of Args Function
	) ;End Of Let Statement
) ;End Of GUI-Dialog-Box Definition

(cx-add-menu "Model Mine Gob" #f)
(cx-add-hitem "Model Mine Gob" "Permeability and Porosity" #f #f #t #f)
;Menu Item Added To Above Created "New Menu->New Submenu" Submenu In Fluent
(cx-add-item "Permeability and Porosity" "Mine Model and Parameters" #\U #f #t gui-dialog-box)
(cx-add-item "Permeability and Porosity" "Zone Selection" #\U #f #t zone-selection-box)
