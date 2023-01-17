# Optimization of order and inventory sourcing decisions in supply chains with multiple nodes, carriers, shipment options, and products 

## Project description

The goal of the project has been to generalize some experience accumulated in the company in optimization of complex supply chains. The major artifact of the project is the following blog article:

[Optimization of order and inventory sourcing decisions in supply chains with multiple nodes, carriers, shipment options, and products](https://docs.google.com/document/d/1j6iGBpXMlDw3HSrLY1dOHRoe7GEj9Ovu0XGACvF8Stg/edit#heading=h.41yg388c98ye)

This repository contains mainly bare bones code used in preparation of the article. The only intention of this code publication is to make it possible to reproduce some resultes reported in the article. 



## Repository structure

- `./optim` contains the main C-langueage code with the launching bash-script 
- `./libcvk2` contains auxiliary code necessary to compile the project



## Requirements

The notebook is supposed to be run from a Google-drive via Google's Colaboratory (Colab), a free Jupyter notebook environment that runs entirely in the cloud. Besides this, the following requirements are supposed to be satisfied to run the notebook without any modifications:

- Credentials to github repo `https://github.com/griddynamics/rnd-gcp-starter-kits`
- Credentials for the project `gd-gcp-rnd-price-optimization` at Google Cloud Platform.
- File `requirements.txt` represent the libraries and their versions with which the notebook was tested. This by no means excludes the workability of the notebook with other versions of the libraries.

One also can run the notebook using other GCP accounts one has credentials for. This would require to change some global variables in 1-2 cells of the notebook and also to upload the two input CSV-files into a GCP bucket. 

No specific Python packages are supposed to be installed besides the packages installed by default.

File `requirements-all.txt` contains the list of all installed packages and their versions at the moment of testing. File `requirements.txt` contains only packages explicitely exported by the notebook. By no means this implies that the notebook will not operate properly with other versions of the libraries. The general style of coding used in the notebook is oriented at generic utilization of the libraries and, thus, is very robust in respect to usage of other versions of the packages. 


## How to setup and run project

- Ensure to meet all requirements.
- Upload notebook to your Google drive.
- Run the notebook via Google Colab environment.

It is reasonabe to run the notebook consecutively, cell-by-cell, paying attention to the comments within the cell and to the output produced by each cell. 

At first run, a pretrained Vertex AI Forecasting model is used. To perform training of a new model in Vertex AI Forecasting, one has to set the value of the variable `MODEL_NAME` to `None` in the proper cell. 


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

