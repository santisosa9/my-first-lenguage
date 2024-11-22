# TDS24
Lenguaje de programación propuesto para el proyecto de la materia Taller de Diseño de Software.

# Build
Para construir el compilador, puedes usar el Makefile proporcionado. El Makefile incluye varias reglas para ayudarte a compilar y gestionar el proyecto de manera eficiente.

Puedes ejecutar
```sh
make
```
para construir el compilador, o
```sh
make dbg
```
para construir el compilador con información de depuración.

Después de construir el compilador, puedes ejecutarlo con
```sh
./ctds <input_file>
```


### Makefile Rules

- **executable**: Construye todo el proyecto (es la regla predeterminada).
- **dbg**: Compila el proyecto con información de depuración.
- **clean**: Elimina todos los archivos compilados y binarios.
- **warn**: Compila el proyecto con todas las advertencias habilitadas.
- **we**: Compila el proyecto con todas las advertencias habilitadas y trata las advertencias como errores.


# Decisiones de Implementación
A continuación de enumeran y describen las consideraciones y deciciones tomadas
la hora de diseñar e implementar el compilador tanto del Preproyecto como del
Proyecto del lenguaje TDS24.

### Extension de Info
Info es la información que se almacena en la tabla de símbolos (TS) a lo largo del proceso de compilación.
Info almacenala infomrmacion de cada token en su contexto en el archivo de cófigo fuente.
En el proceso de compilación Info aparece por primera vez en la etapa de
análisis léxico, luego el parser la asigna a un nodo, y luego de la etapa de
parsing y antes de ingresar a la etapa de backend, se crea la TS al hacer el chequeo de tipos y las instancias de Info en pasan a manos de la TS.

Como el lenguaje TDS24 tiene construcciones más complejas que una simple
secuencia lineal de instrucciones, por ejemplo, funciones, para mejorar el
diseño y no tener una sola estructura sobrecargada con información innecesaria
se decidió discriminar entre varios tipos de Info: InfoBase, InfoFunction e
InfoControlFlow.

### Statement vacio

No se puede.

### if-then-else

El nodo izquierdo será la condicion. A la derecha se presentan dos casos:

- Si hay un token del else pasamos a un nuevo nodo then-else donde el hijo izquierdo es el cuerpo del then y el hijo derecho es el cuerpo del else.

- Si no hay un token del else el hijo derecho simplemente será el cuerpo del then.
