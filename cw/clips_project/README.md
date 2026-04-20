# CLIPS Habitability Assessment Project

This CLIPS project implements an expert system for assessing the habitability of planets based on the characteristics of their central star or object. The system follows a decision tree derived from the flowchart in `tree.md`.

## Overview

The system asks a series of yes/no questions about the central object (star or compact object) and the planet's properties to determine the potential habitability level.

## Files

- `habitability.clp`: The main CLIPS program containing all rules, templates, and facts.

## How to Run

1. Install CLIPS (CLIPS can be downloaded from https://clipsrules.net/).
2. Load the program: `(load "habitability.clp")`
3. Reset the system: `(reset)`
4. Run the inference engine: `(run)`
5. Answer the questions with `yes` or `no` as prompted.

## Decision Tree

The system evaluates:
- Type of central object (main sequence star, neutron star, black hole, white dwarf, etc.)
- Star properties (mass, stability, activity)
- Planet properties (location in habitable zone, atmosphere retention, magnetosphere)
- Orbital stability

Based on the answers, it provides a conclusion on habitability potential ranging from "Высокая потенциальная обитаемость" (high potential) to "Непригодна" (unsuitable).

## Notes

- All questions are in Russian as per the original tree.
- Conclusions are also in Russian.
- The system assumes binary (yes/no) answers to all questions.