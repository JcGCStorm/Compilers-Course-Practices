## Estructura básica de un programa en C.
Para esto busqué en internet y ví que no había como tal un consenso de la estructura para proyectos en C, pero al menos los directorios que más se repetían eran __build__, __src__, __tools__ y __tests__. Por lo que decidí usar esos para mis prácticas. A continuación voy a explicar mejor que va a ir en cada uno de los directorios, todo lo obtuve de la siguiente [página](https://joholl.github.io/pitchfork-website/#tld.build):
- **build**: Aquí van a ir los resultados de compilación que son efímeros, que quizá deseche muy rápidamente, es solo para compilar y de hecho de donde me estoy basando para hacer la estructura del proyecto mencionan que no debería subirlo a Github, por lo que no lo haré.
- **src**: En esta carpeta vamos a tener tanto los archivos.c como los archivos.h
- **tools**: A mi me gusta mucho usar scripts para automatizar cosas, quizá no lleguen al código main pero en development seguro que los agrego.
- **tests**: Todos los proyectos deben de tener tests para evitar fallos o bugs al cambiar cosas.
## Branch Strategy.
Para este proyecto voy a tener las siguientes ramas principales:
- **main**: Va a ser la rama en la que tenga el código final, no voy a subir nada ahí que no esté comprobado que funciona al 100%. 
- **dev**: En esta rama estará el código más nuevo pero que sea solo de desarrollo, quizá aún con bugs, pero casi listo
- **feat/nueva-funcionalidad**: En esta rama estarán las funcionalidades nuevas del código. El nombre del feature va a ir cambiando según lo que agreguemos.

## Métodología.
He leído y acepto la metodología de este curso.
