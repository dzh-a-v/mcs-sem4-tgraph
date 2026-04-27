(deftemplate answer
  (slot question (type SYMBOL))
  (slot response (type SYMBOL) (allowed-symbols yes no))
)

(deftemplate question
  (slot id (type SYMBOL))
  (slot text (type STRING))
  (slot yes-next (type SYMBOL))
  (slot no-next (type SYMBOL))
)

(deftemplate result
  (slot id (type SYMBOL))
  (slot text (type STRING))
)

(deftemplate ask
  (slot id (type SYMBOL))
)

(deftemplate route
  (slot target (type SYMBOL))
)

(deftemplate conclusion
  (slot text (type STRING))
)

(deffunction ask-yes-no (?text)
  (bind ?resp "")

  (while TRUE do
    (printout t ?text crlf "Введите ответ (да/нет): ")
    (bind ?resp (lowcase (readline)))

    (if (or (eq ?resp "да") (eq ?resp "д") (eq ?resp "yes") (eq ?resp "y"))
      then
      (return yes))

    (if (or (eq ?resp "нет") (eq ?resp "н") (eq ?resp "no") (eq ?resp "n"))
      then
      (return no))

    (printout t "Некорректный ввод. Пожалуйста, введите \"да\" или \"нет\"." crlf crlf)
  )
)

(deffacts knowledge-base
  (question
    (id Q1)
    (text "Центральный объект — звезда главной последовательности?")
    (yes-next Q2)
    (no-next Q3))
  (question
    (id Q2)
    (text "Масса звезды 0.6–1.1 M☉ (G / ранний-средний K-карлик)?")
    (yes-next Q4)
    (no-next Q5))
  (question
    (id Q3)
    (text "Сверхмассивная чёрная дыра?")
    (yes-next Q3a)
    (no-next Q3b))
  (question
    (id Q4)
    (text "Фотометрическая стабильность и умеренный ультрафиолетовый фон?")
    (yes-next Q6)
    (no-next Q7))
  (question
    (id Q5)
    (text "M-карлик массой < 0.6 M☉?")
    (yes-next Q10)
    (no-next Q11))
  (question
    (id Q6)
    (text "Планета находится в консервативной зоне обитаемости?")
    (yes-next Q8)
    (no-next R1))
  (question
    (id Q7)
    (text "Повышенная хромосферная активность?")
    (yes-next Q9)
    (no-next R4))
  (question
    (id Q8)
    (text "Масса планеты 0.5–10 M⊕, есть атмосфера / магнитосфера / внутренняя активность?")
    (yes-next R2)
    (no-next R3))
  (question
    (id Q9)
    (text "У планеты есть сильная магнитосфера?")
    (yes-next R5)
    (no-next R6))
  (question
    (id Q10)
    (text "Приливный захват и высокая активность в первый 1 млрд лет?")
    (yes-next R7)
    (no-next R8))
  (question
    (id Q11)
    (text "Узкая зона обитаемости и быстрая эволюция звезды?")
    (yes-next R9)
    (no-next R10))
  (question
    (id Q3a)
    (text "Спин чёрной дыры экстремальный (a/M≈1)?")
    (yes-next Q3c)
    (no-next R11))
  (question
    (id Q3b)
    (text "Нейтронная звезда или малая чёрная дыра?")
    (yes-next Q3j)
    (no-next Q3k))
  (question
    (id Q3c)
    (text "Значительное замедление времени?")
    (yes-next Q3d)
    (no-next R12))
  (question
    (id Q3d)
    (text "Блюшифт фонового излучения создаёт полезный поток энергии?")
    (yes-next R13)
    (no-next R14))
  (question
    (id Q3j)
    (text "Орбита вне зоны приливного разрушения и жёсткой радиации?")
    (yes-next R20)
    (no-next R21))
  (question
    (id Q3k)
    (text "Планета находится в узкой зоне обитаемости после охлаждения белого карлика?")
    (yes-next R22)
    (no-next R23))

  (result (id R1) (text "Непригодна [3, p.6]"))
  (result (id R2) (text "Высокий потенциал [3, p.11]"))
  (result (id R3) (text "Средний потенциал [3, p.18]"))
  (result (id R4) (text "Непригодна [3, p.19]"))
  (result (id R5) (text "Средний потенциал [3, p.19]"))
  (result (id R6) (text "Низкий потенциал [3, p.19]"))
  (result (id R7) (text "Низкий потенциал [3, p.19]"))
  (result (id R8) (text "Средний потенциал [3, p.18]"))
  (result (id R9) (text "Низкий потенциал [3, p.16]"))
  (result (id R10) (text "Средний потенциал [3, p.16]"))
  (result (id R11) (text "Низкий потенциал [2, p.15]"))
  (result (id R12) (text "Непригодна [2, p.15]"))
  (result (id R13) (text "Низкий потенциал [2, p.27; 3, p.19; 2, p.27; 2, p.11; 2, p.15]"))
  (result (id R14) (text "Непригодна [2, p.15]"))
  (result (id R20) (text "Низкий потенциал [1, p.5]"))
  (result (id R21) (text "Непригодна [2, p.15]"))
  (result (id R22) (text "Низкий потенциал [3, p.11]"))
  (result (id R23) (text "Непригодна [3, p.11]"))

  (ask (id Q1))
)

(defrule ask-question
  ?a <- (ask (id ?qid))
  (question (id ?qid) (text ?text) (yes-next ?yes-next) (no-next ?no-next))
  =>
  (retract ?a)

  (if (eq ?qid Q1)
    then
    (printout t crlf
      "Экспертная система оценки потенциальной обитаемости планеты." crlf
      "Отвечайте на вопросы словами \"да\" или \"нет\"." crlf
      "При необходимости можно использовать и ответы yes/no." crlf crlf))

  (bind ?prompt (str-cat ?qid ": " ?text))
  (bind ?resp (ask-yes-no ?prompt))
  (assert (answer (question ?qid) (response ?resp)))

  (if (eq ?resp yes)
    then
    (bind ?next ?yes-next)
    else
    (bind ?next ?no-next))

  (assert (route (target ?next)))
)

(defrule route-to-question
  ?r <- (route (target ?target))
  (question (id ?target))
  =>
  (retract ?r)
  (assert (ask (id ?target)))
)

(defrule route-to-result
  ?r <- (route (target ?target))
  (result (id ?target) (text ?text))
  =>
  (retract ?r)
  (assert (conclusion (text ?text)))
)

(defrule print-conclusion
  ?c <- (conclusion (text ?text))
  =>
  (retract ?c)
  (printout t crlf "Заключение: " ?text crlf)
)
