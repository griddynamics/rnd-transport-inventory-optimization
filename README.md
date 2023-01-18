# Optimization of order and inventory sourcing decisions in supply chains with multiple nodes, carriers, shipment options, and products 

## Project description

The goal of the project has been to generalize some experience accumulated in the company in optimization of complex supply chains. The major artifact of the project is the following blog article:

[Optimization of order and inventory sourcing decisions in supply chains with multiple nodes, carriers, shipment options, and products](https://docs.google.com/document/d/1j6iGBpXMlDw3HSrLY1dOHRoe7GEj9Ovu0XGACvF8Stg/edit#heading=h.41yg388c98ye)

The major results of the article are summarized by the following quote:

> In this article, we analyze several common sourcing optimization scenarios, develop a relatively general framework for representing sourcing problems, and then evaluate and compare two optimization strategies (MIP solvers and metaheuristic (stochastic) optimization) on the problems of different sizes.

This repository contains mainly bare bones code used in preparation of the article. The only intention of this code publication is to make it possible to reproduce the results reported in the article. 


## Repository structure

- `./optim` contains the main C-langueage code with the launching bash-script 

- `./libcvk2` contains auxiliary code necessary to compile the project


## Requirements

The project has been implemented in Linux framework. The requirements are:

- installation of the  *GCC* compiler (installed on default in most Linux distributions); 

- installation of the  *Bash* UNIX-shell (installed on default in most Linux distributions); 

- installation of the  *Python 3* (installed on default in most Linux distributions); 

- installation of the *GSL* (*GNU Scientific Library*);

- installation of *OR-Tools* from *Google* using, for instance, the following [instructinons](https://developers.google.com/optimization/install)

The installation of *OR-Tools* is optinonal. It is necessary only to run the Python code generated for *OR-Tools* by the main code of the project. If only the results of metaheuristic optimization are of intererest, then there is no the necessity to install *OR-Tools*. 

## How to setup and run project

- Ensure to meet the requirements.

- Change the working directory to `./optim` and run the command `./compile`. This should produce 3 files in the same directory:
    - `ortools.prg.py` is a Python code for MIP solution via OR-Tools;
    - `res.csv` contains the values of the objective function (column 3) and the penalty (column 4) by iterations;
    - `log.txt` contains full output of the main program into the stdout stream,which describes the whole metaheuristic optimization in details by iterations (could by cryptic for an occasional user).


## License

```
Copyright 2022 Grid Dynamics International, Inc. All Rights Reserved

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

## Authors

- Volodymyr Koliadin, `vkoliadin@griddynamics.com`
- Ilya Katsov, `ikatsov@griddynamics.com`

