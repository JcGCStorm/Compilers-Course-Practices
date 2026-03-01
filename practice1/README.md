## English
### Basic structure of a C project.
After searching the internet, I realized that there **is** no consensus **on the** structure for projects in C, but there are some directories that most of the projects have, the directories are __build__, __src__, __tools__ and __tests__. So I decided to use those for my practices. Below I am going to better explain what is going to go in each of the directories, I got everything from the following [page](https://joholl.github.io/pitchfork-website/#tld.build):
- **build**: Here are going to go the compilation results that are ephemeral, which I may discard very quickly, it is only to compile and I should not upload it to Github, so I won't do it.
- **src**: In this folder I will have both .c and .h files
- **tools**: I really like using scripts to automate things, they may not reach the main code but in development I will surely add them.
- **tests**: All projects must have tests to avoid failures or bugs when changing things, so i will add them.
### Branch Strategy.
For this project I will have the following main branches:
- **main**: It will be the branch in which I have the final code, I am not going to upload anything there that is not proven to be fully functional. 
- **dev**: In this branch there will be the newest code but that is only for development, perhaps still with bugs, but almost ready
- **feature/new-functionality**: In this branch will be the new functionalities of the code. The name of the feature will change depending on what i add.
### Methodology.
**I have read and accept the methodology of this course.**

## Spanish
### Estructura básica de un proyecto en C.
Para esto busqué en internet y ví que no había como tal un consenso de la estructura para proyectos en C, pero los directorios que más se repetían eran __build__, __src__, __tools__ y __tests__. Por lo que decidí usar esos para mis prácticas. A continuación voy a explicar mejor que va a ir en cada uno de los directorios, toda la estructura la obtuve de la siguiente [página](https://joholl.github.io/pitchfork-website/#tld.build):
- **build**: Aquí van a ir los resultados de compilación que son efímeros, que quizá deseche muy rápidamente, es solo para compilar y de hecho de donde me estoy basando para hacer la estructura del proyecto mencionan que no debería subirlo a Github, por lo que no lo haré.
- **src**: En esta carpeta vamos a tener tanto los archivos.c como los archivos.h
- **tools**: A mi me gusta mucho usar scripts para automatizar cosas, quizá no lleguen al código main pero en development seguro que los agrego.
- **tests**: Todos los proyectos deben de tener tests para evitar fallos o bugs al cambiar cosas.
### Branch Strategy.
Para este proyecto voy a tener las siguientes ramas principales:
- **main**: Va a ser la rama en la que tenga el código final, no voy a subir nada ahí que no esté comprobado que funciona al 100%. 
- **dev**: En esta rama estará el código más nuevo pero que sea solo de desarrollo, quizá aún con bugs, pero casi listo
- **feat/nueva-funcionalidad**: En esta rama estarán las funcionalidades nuevas del código. El nombre del feature va a ir cambiando según lo que agreguemos.
### Metodología.
**He leído y acepto la metodología de este curso.**
