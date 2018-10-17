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
		(lambda (x y)
			(set-property "thickness" (1)
				(set-property "object-name" "line"
					(list x y)
				)
			)
		)
	)

	(define make-text
		(lambda (x)
			(set-property "position" (make-point 0 0)
				(set-property "object-name" "text"
					(x)
				)
			)
		)
	)
)
