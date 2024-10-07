# TDS24
TODO: (complete)

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