; mock callback functions
(define (apply-cb) #t)
(define update-cb #f)

; the dialogue box itself is a panel with a single table for organization
(define mine_gob_dialogue_box (cx-create-panel "Mine Gob" apply-cb update-cb))
(define table(cx-create-table mine_gob_dialogue_box ""))

; buttons to choose gob zone type
(define gob_zone_button_box (cx-create-button-box table "Gob Zone Type" 'radio-mode #t 'col 0))

(define startup_center_radio_button (cx-create-toggle-button gob_zone_button_box "Startup Center"))
(define startup_headgate_radio_button (cx-create-toggle-button gob_zone_button_box "Startup Headgate"))
(define startup_tailgate_radio_button (cx-create-toggle-button gob_zone_button_box "Startup Tailgate"))
(define gob_center_radio_button (cx-create-toggle-button gob_zone_button_box "Gob Center"))
(define mid_headgate_radio_button (cx-create-toggle-button gob_zone_button_box "Mid-panel Headgate"))
(define mid_tailgate_radio_button (cx-create-toggle-button gob_zone_button_box "Mid-panel Tailgate"))
(define working_center_radio_button (cx-create-toggle-button gob_zone_button_box "Working Face Center"))
(define working_headgate_radio_button (cx-create-toggle-button gob_zone_button_box "Working Face Headgate"))
(define working_tailgate_radio_button (cx-create-toggle-button gob_zone_button_box "Working Face Tailgate"))

; display the dialogue box
(cx-show-panel mine_gob_dialogue_box)
