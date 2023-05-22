; mock callback functions
(define (apply-cb) #t)
(define update-cb #f)

; the dialogue box itself is a panel with a single table for organization
(define mine_gob_dialogue_box (cx-create-panel "Mine Gob" apply-cb update-cb))
(define table(cx-create-table mine_gob_dialogue_box ""))

; buttons to choose mine model
(define mine_button_box (cx-create-button-box table "Mine Model" 'radio-mode #t 'col 0))

(define mine_C_radio_button (cx-create-toggle-button mine_button_box "Mine C Model"))
(define mine_E_radio_button (cx-create-toggle-button mine_button_box "Mine E Model"))
(define mine_T_radio_button (cx-create-toggle-button mine_button_box "Mine T Model"))

; button to toggle egz
(define egz_button_box (cx-create-button-box table "" 'radio-mode #f 'row 1 'col 0))

(define egz_radio_button (cx-create-toggle-button egz_button_box "Explosive Gas Zone Colorization"))

; table to hold parameter inputs
(define param_table (cx-create-table table "Gob Parameters and Limits" 'row 0 'col 1))

; parameter real inputs
(define min_vsi_input (cx-create-real-entry param_table "Min VSI" 'row 0))
(define max_vsi_input (cx-create-real-entry param_table "Max VSI" 'row 1))
(define init_poro_input (cx-create-real-entry param_table "Initial Porosity of Host Rock" 'row 2))
(define resist_scalar_input (cx-create-real-entry param_table "Resistance Scalar" 'row 3))

; display the dialogue box
(cx-show-panel mine_gob_dialogue_box)
