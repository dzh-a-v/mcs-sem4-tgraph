# CLIPS Habitability Assessment Project

This CLIPS project implements an expert system for assessing the habitability of planets based on the characteristics of their central star or object. The system follows a decision tree derived from the flowchart in `tree.md`.

## Overview

The system asks a series of yes/no questions about the central object (star or compact object) and the planet's properties to determine the potential habitability level.

## Files

- `habitability.clp`: The main CLIPS program containing all rules, templates, and facts.

## Program Structure

The CLIPS program is intentionally data-driven: the decision tree itself is stored as facts, while the rules only control how to ask questions and move to the next node. This makes the file easier to update when `tree.md` changes.

### Templates

- `answer`: stores one user answer. The `question` slot keeps the question ID (`Q1`, `Q2`, etc.), and the `response` slot stores the normalized answer as `yes` or `no`.
- `question`: describes one internal node of the decision tree. `id` is the node name, `text` is the Russian question shown on screen, and `yes-next` / `no-next` define where the system goes after each possible answer.
- `result`: describes one leaf of the tree. `id` is the result label (`R1`, `R2`, etc.), and `text` is the final conclusion that will be printed.
- `ask`: a control fact meaning “ask this question next”. It is used to start the dialog and to schedule each following question.
- `route`: a temporary control fact meaning “go to this next node”. After the user answers a question, the system creates `route` and then resolves it either to another question or to a final result.
- `conclusion`: stores the final text right before it is printed to the user.

### Function

- `ask-yes-no`: prints the question text, reads input from the user, converts accepted variants (`да`, `нет`, `yes`, `no`, and short forms) to the internal symbols `yes` and `no`, and repeats until a valid answer is entered.

### Facts

- `knowledge-base`: contains the whole knowledge base of the expert system.
- In the `question` facts, every question from `tree.md` is encoded together with its transitions for `yes` and `no`.
- In the `result` facts, every final outcome from `tree.md` is stored together with its textual conclusion and source reference.
- The fact `(ask (id Q1))` is the entry point of the system and starts the interview from the root of the tree.

### Rules

- `ask-question`: the main dialog rule. It reacts to an `ask` fact, prints the introduction for the first step, calls `ask-yes-no`, stores the answer in an `answer` fact, and creates a `route` fact for the next node.
- `route-to-question`: checks whether the next node is another `question`. If yes, it converts `route` into a new `ask` fact.
- `route-to-result`: checks whether the next node is a `result`. If yes, it converts `route` into a `conclusion`.
- `print-conclusion`: prints the final conclusion and finishes the current run.

### Execution Flow

1. After `(reset)`, CLIPS loads all `question` and `result` facts from `knowledge-base` and asserts `(ask (id Q1))`.
2. `ask-question` asks the current question and normalizes the input.
3. The answer is transformed into a `route` fact pointing either to another question or to a result.
4. The system keeps repeating this process until `print-conclusion` outputs the final verdict.

### Why The Structure Looks Like This

- The tree logic is stored in facts instead of hardcoding one separate rule for every question.
- Because of that, updating the expert system usually means editing the `question` and `result` facts, not rewriting the control logic.
- The rules stay short and generic, and the mapping to `tree.md` remains easy to verify.

## How to Run

1. Install CLIPS (CLIPS can be downloaded from https://clipsrules.net/).
2. Load the program: `(load "habitability.clp")`
3. Reset the system: `(reset)`
4. Run the inference engine: `(run)`
5. Answer the questions with `да` or `нет` as prompted. The program also accepts `yes/no` as fallback input.

## Decision Tree

The system evaluates:
- Type of central object (main sequence star, neutron star, black hole, white dwarf, etc.)
- Star properties (mass, stability, activity)
- Planet properties (location in habitable zone, atmosphere retention, magnetosphere)
- Orbital stability

Based on the answers, it provides a conclusion that matches the current tree, such as "Высокий потенциал", "Средний потенциал", "Низкий потенциал", or "Непригодна" together with the source reference.

## Notes

- All questions are in Russian as per the original tree.
- Conclusions are also in Russian.
- The system assumes binary answers to all questions (`да/нет`, `yes/no` is also accepted).
