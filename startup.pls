(begin
	(define make-point
		(lambda (x y)
			(set-property "size" (0)
				(set-property "object-name" "point"
					(list x y)
				)
			)
		)
	)

	(define make-line
		(lambda (p1 p2)
			(set-property "thickness" (1)
				(set-property "object-name" "line"
					(list p1 p2)
				)
			)
		)
	)

	(define make-text
		(lambda (str)
			(set-property "position" (make-point 0 0)
				(set-property "object-name" "text"
					(str)
				)
			)
		)
	)
)
