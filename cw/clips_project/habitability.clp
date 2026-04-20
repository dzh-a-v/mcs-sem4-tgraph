(deftemplate answer
  (slot question (type SYMBOL))
  (slot response (type SYMBOL))
)

(deftemplate conclusion
  (slot result (type STRING))
)

(deftemplate ask
  (slot question (type SYMBOL))
)

(deffacts initial
  (ask (question Q1))
)

(defrule ask-Q1
  ?a <- (ask (question Q1))
  =>
  (retract ?a)
  (printout t "Q1: Центральный объект — звезда главной последовательности (Main Sequence Star)? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q1) (response ?resp)))
  (if (eq ?resp yes) then (assert (ask (question Q2))) else (assert (ask (question Q3))))
)

(defrule ask-Q2
  ?a <- (ask (question Q2))
  =>
  (retract ?a)
  (printout t "Q2: Масса звезды 0.5–1.5 M☉? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q2) (response ?resp)))
  (if (eq ?resp yes) then (assert (ask (question Q4))) else (assert (ask (question Q5))))
)

(defrule ask-Q3
  ?a <- (ask (question Q3))
  =>
  (retract ?a)
  (printout t "Q3: Компактный или неопределённый центральный объект? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q3) (response ?resp)))
  (if (eq ?resp yes) then (assert (ask (question Q13))) else (assert (ask (question Q14))))
)

(defrule ask-Q4
  ?a <- (ask (question Q4))
  =>
  (retract ?a)
  (printout t "Q4: Фотометрическая и магнитная стабильность звезды? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q4) (response ?resp)))
  (if (eq ?resp yes) then (assert (ask (question Q6))) else (assert (ask (question Q7))))
)

(defrule ask-Q5
  ?a <- (ask (question Q5))
  =>
  (retract ?a)
  (printout t "Q5: Массивная или эволюционно короткоживущая звезда? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q5) (response ?resp)))
  (if (eq ?resp yes) then (assert (ask (question Q10))) else (assert (ask (question Q11))))
)

(defrule ask-Q6
  ?a <- (ask (question Q6))
  =>
  (retract ?a)
  (printout t "Q6: Планета расположена в обитаемой зоне (CHZ)? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q6) (response ?resp)))
  (if (eq ?resp yes) then (assert (ask (question Q8))) else (assert (conclusion (result "Низкая обитаемость: выход за пределы обитаемой зоны"))))
)

(defrule ask-Q7
  ?a <- (ask (question Q7))
  =>
  (retract ?a)
  (printout t "Q7: Повышенная хромосферная активность и вспышечность? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q7) (response ?resp)))
  (if (eq ?resp yes) then (assert (ask (question Q9))) else (assert (conclusion (result "Низкая обитаемость"))))
)

(defrule ask-Q8
  ?a <- (ask (question Q8))
  =>
  (retract ?a)
  (printout t "Q8: Планета способна удерживать атмосферу? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q8) (response ?resp)))
  (if (eq ?resp yes) then (assert (conclusion (result "Высокая потенциальная обитаемость"))) else (assert (conclusion (result "Средняя обитаемость"))))
)

(defrule ask-Q9
  ?a <- (ask (question Q9))
  =>
  (retract ?a)
  (printout t "Q9: Наличие магнитосферы планеты? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q9) (response ?resp)))
  (if (eq ?resp yes) then (assert (conclusion (result "Средняя обитаемость"))) else (assert (conclusion (result "Низкая обитаемость из-за атмосферной эрозии"))))
)

(defrule ask-Q10
  ?a <- (ask (question Q10))
  =>
  (retract ?a)
  (printout t "Q10: Риск сверхновой эволюции системы? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q10) (response ?resp)))
  (if (eq ?resp yes) then (assert (conclusion (result "Непригодна: стерилизация среды"))) else (assert (ask (question Q12))))
)

(defrule ask-Q11
  ?a <- (ask (question Q11))
  =>
  (retract ?a)
  (printout t "Q11: Фотометрически переменная звезда? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q11) (response ?resp)))
  (if (eq ?resp yes) then (assert (conclusion (result "Низкая обитаемость: нестабильный радиационный поток"))) else (assert (conclusion (result "Очень низкая обитаемость"))))
)

(defrule ask-Q12
  ?a <- (ask (question Q12))
  =>
  (retract ?a)
  (printout t "Q12: Орбитальная динамическая стабильность? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q12) (response ?resp)))
  (if (eq ?resp yes) then (assert (conclusion (result "Средняя обитаемость при безопасной орбите"))) else (assert (conclusion (result "Низкая обитаемость"))))
)

(defrule ask-Q13
  ?a <- (ask (question Q13))
  =>
  (retract ?a)
  (printout t "Q13: Нейтронная звезда? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q13) (response ?resp)))
  (if (eq ?resp yes) then (assert (ask (question Q15))) else (assert (ask (question Q16))))
)

(defrule ask-Q14
  ?a <- (ask (question Q14))
  =>
  (retract ?a)
  (printout t "Q14: Белый карлик? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q14) (response ?resp)))
  (assert (conclusion (result "Требуется дальнейшее изучение")))
)

(defrule ask-Q15
  ?a <- (ask (question Q15))
  =>
  (retract ?a)
  (printout t "Q15: Планета вне зоны сильного рентгеновского излучения? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q15) (response ?resp)))
  (if (eq ?resp yes) then (assert (conclusion (result "Низкая обитаемость (радиация, концентрированные лучи)"))) else (assert (conclusion (result "Непригодна"))))
)

(defrule ask-Q16
  ?a <- (ask (question Q16))
  =>
  (retract ?a)
  (printout t "Q16: Чёрная дыра? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q16) (response ?resp)))
  (if (eq ?resp yes) then (assert (ask (question Q17))) else (assert (conclusion (result "Требуется дальнейшее изучение"))))
)

(defrule ask-Q17
  ?a <- (ask (question Q17))
  =>
  (retract ?a)
  (printout t "Q17: Планета вне горизонта событий и аккреционного диска? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q17) (response ?resp)))
  (if (eq ?resp yes) then (assert (ask (question Q18))) else (assert (conclusion (result "Непригодна"))))
)

(defrule ask-Q18
  ?a <- (ask (question Q18))
  =>
  (retract ?a)
  (printout t "Q18: Орбитальная динамическая стабильность? (yes/no): ")
  (bind ?resp (read))
  (assert (answer (question Q18) (response ?resp)))
  (if (eq ?resp yes) then (assert (conclusion (result "Низкая обитаемость"))) else (assert (conclusion (result "Непригодна"))))
)

(defrule print-conclusion
  (conclusion (result ?r))
  =>
  (printout t "Заключение: " ?r crlf)
)