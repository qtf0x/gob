(define (apply-cb) #t)
(define update-cb #f)
(define table)
(define myList1)
(define myList2)
(define checkBox1)

(define (make-new-rpvar name default type)
	(if (not (rp-var-object name))
		(rp-var-define name default type #f)))

(make-new-rpvar 'gob/isBool #f 'boolean)

(define (button-cb . args)
    (cx-set-list-items myList2 (cx-show-list-selections myList1))
    (cx-set-list-selections myList2 (cx-show-list-selections myList1))
)
(define (button2-cb . args);                                 |
    ; command to set variables of provided zone              v This is the zone being set up
    (ti-menu-load-string "/define/boundary-conditions/fluid gob-multi-part-gob-gateroad-headgate no no no no no 0 no 0 no 0 no 0 no 0 no 1 none no no no yes no no 1 no 0 no 0 no 0 no 1 no 0 yes yes yes \"udf\" \"set_perm_1_VSI::libudf\" yes yes udf \"set_perm_2_VSI::libudf\" yes yes \"udf\" \"set_perm_3_VSI::libudf\" no yes yes \"udf\" \"set_inertia_1_VSI::libudf\" yes yes \"udf\" \"set_inertia_2_VSI::libudf\" yes yes \"udf\" \"set_inertia_3_VSI::libudf\" 0 0 yes yes \"udf\" \"set_poro_VSI::libudf\" constant 1 no")
)
(define my-dialog-box (cx-create-panel "My Dialog Box" apply-cb update-cb))
(set! table (cx-create-table my-dialog-box "This is an example Dialog Box"))
(set! myList1 (cx-create-list table "List 1" 'visible-lines 3 'multiple-selections #t 'row 0))
(cx-set-list-items myList1 (list "Item 1" "Item 2" "Item 3" "Item 4" "Item 5"))
(set! myList2 (cx-create-list table "List 2" 'visible-lines 5 'multiple-selections #t 'row 1))

(cx-create-button table "Go" 'activate-callback button2-cb 'row 2 'col 1)
(cx-show-panel my-dialog-box)
